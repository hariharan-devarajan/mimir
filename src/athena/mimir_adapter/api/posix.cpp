//
// Created by haridev on 2/14/22.
//

#include <athena/api/interceptor.h>
#include <athena/api/posix.h>
#include <errno.h>
#include <mimir/advice/advice_handler.h>
#include <mimir/advice/file_advice.h>
#include <mimir/common/error_code.h>
#include <mimir/constant.h>
#include <mimir/log/logger.h>
#include <mpi.h>

#include <cmath>
#include <cstring>
#include <experimental/filesystem>

#include "athena/client/posix_athena_client.h"
#include "athena/server/posix_io.h"

namespace fs = std::experimental::filesystem;

MimirStatus file_prefetch(mimir::FileAdvice &advice) {
  if (advice._type._primary != mimir::PrimaryAdviceType::DATA_FILE) {
    return mimir::MIMIR_ONLY_FILE_ALLOWED;
  }
  mimir::MimirKey key;
  std::hash<std::string> hash_str;
  key._id = hash_str(advice._name);
  switch (advice._type._secondary) {
    case mimir::OperationAdviceType::READ_ONLY_FILE:
    case mimir::OperationAdviceType::INPUT_FILE: {
      auto client = athena::PosixClient::Instance();
      int current_rank = 0;
      uint16_t my_server_index = 0;
      if (is_mpi()) MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
      my_server_index = floor(
          current_rank / client->_job_configuration_advice._num_cores_per_node);
      auto dest_server = my_server_index;//key._id % client->_job_configuration_advice._num_nodes;
      bool status = false;
      if (my_server_index != dest_server) {
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_INFO, "Prefetch of file %s in Athena on server %d",
            advice._name.data(), dest_server);
        status =
            client->_rpc
                ->call<RPCLIB_MSGPACK::object_handle>(
                    dest_server, "athena::posix::prefetch", advice._name.data())
                .as<bool>();
      } else {
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_INFO, "Prefetch of file %s in Athena on local server %d",
            advice._name.data(), dest_server);
        status = athena::posix_prefetch(advice._name.data());
      }
      if (status) {
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LoggerType::LOG_INFO, " Prefetch on file %s successful.",
            advice._name.data());
      }
    }
  }
}

int handle_open(const char *path, int flags, int mode, bool enable_rpc) {
  int ret;
  MAP_OR_FAIL(open64);
  bool perform_io = true;
  std::string filename(path);
  if (IsTracked(path)) {
    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO, "Tracking file %s",
                                           path);
    auto client = athena::PosixClient::Instance();
    if (client != nullptr) {
      bool is_read_only = false;
      if (flags & O_RDONLY) {
        is_read_only = true;
      }
      auto file_type = mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                         mimir::OperationAdviceType::NO_OP);
      auto file_advice_handler =
          mimir::AdviceHandler<mimir::FileAdvice>::Instance(file_type);
      mimir::MimirKey file_key;
      std::hash<std::string> hash_str;
      file_key._id = hash_str(std::string(path));
      mimir::MimirKey job_conf_key;
      job_conf_key._id = 0;

      int current_rank = 0;
      if (is_mpi()) MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
      uint16_t my_server_index = floor(
          current_rank / client->_job_configuration_advice._num_cores_per_node);
      uint16_t server_index = my_server_index;
      auto mapped_file = client->mapped_files_find(filename);
      if (!mapped_file.first) {
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_INFO, "No Mapping found for file %s", path);
        if (file_advice_handler->is_advice_present(file_key)) {
          mimir::Logger::Instance("ATHENA")->log(
              mimir::LOG_INFO, "Advice present for file %s", path);
          auto advices = file_advice_handler->resolve_conflicts(file_key);
          for (const auto &advice : advices) {
            mimir::Logger::Instance("ATHENA")->log(
                mimir::LOG_INFO, "Applying advice %d for file %s",
                advice._type._secondary, path);
            switch (advice._type._secondary) {
              case mimir::OperationAdviceType::INDEPENDENT_FILE: {
                mimir::Logger::Instance("ATHENA")->log(
                    mimir::LOG_INFO,
                    "Applying independent file advice for file %s", path);
                auto num_devices =
                    client->_job_configuration_advice._devices.size();
                if (num_devices > 1) {
                  bool updated = false;
                  auto updated_devices = std::vector<mimir::Storage>();
                  mimir::Storage device;
                  for (int i = 0; i < num_devices; ++i) {
                    device = client->_job_configuration_advice._devices[i];
                    if (!updated && ((int) device._capacity_mb -
                                     (int) (device._used_capacity_mb +
                                            advice._size_mb)) >= 0) {
                      fs::create_directories(device._mount_point);
                      filename.replace(0,
                                       client->_job_configuration_advice
                                           ._devices[advice._current_device]
                                           ._mount_point.size(),
                                       device._mount_point);
                      if (!is_read_only || fs::exists(filename)) {
                        device._used_capacity_mb =
                            device._used_capacity_mb + advice._size_mb;
                        mimir::Logger::Instance("ATHENA")->log(
                            mimir::LOG_INFO, "Opening file on %s for file %s",
                            device._mount_point.c_str(), path);
                        updated = true;
                      } else {
                        mimir::Logger::Instance("ATHENA")->log(
                            mimir::LOG_INFO,
                            "File %s is not opened in READONLY and it does not "
                            "exists. Reverting to original path.",
                            path);
                        filename = std::string(path);
                      }
                    }
                    updated_devices.push_back(device);
                  }
                  if (updated) {
                    client->_job_configuration_advice._devices =
                        updated_devices;
                    client->_job_handler->save_advice(
                        job_conf_key, client->_job_configuration_advice);
                    client->mapped_files_emplace(std::string(path), filename);
                  }
                }
                break;
              }
              case mimir::OperationAdviceType::SHARED_FILE: {
                /** make sure we store on shared FS using shared memory
                 * Store the mapping in metadata to enable reading
                 * **/
                mimir::Logger::Instance("ATHENA")->log(
                    mimir::LOG_INFO, "Applying shared file advice for file %s",
                    path);
                server_index = std::hash<std::string>()(filename) %
                               client->_job_configuration_advice._num_nodes;

                if (enable_rpc && my_server_index != server_index) {
                  mimir::Logger::Instance("ATHENA")->log(
                      mimir::LOG_INFO,
                      "Perform RPC on server %d open for file %s", server_index,
                      path);
                  auto rpc_ret =
                      client->_rpc->call<RPCLIB_MSGPACK::object_handle>(
                          server_index, "athena::posix::open", filename, flags,
                          mode);
                  ret = rpc_ret.as<int>();
                  perform_io = false;
                } else {
                  mimir::Logger::Instance("ATHENA")->log(
                      mimir::LOG_INFO, "Perform Local open for file %s", path);
                  auto num_devices =
                      client->_job_configuration_advice._devices.size();
                  if (num_devices > 1) {
                    bool updated = false;
                    auto updated_devices = std::vector<mimir::Storage>();
                    mimir::Storage device;
                    for (int i = 0; i < num_devices; ++i) {
                      device = client->_job_configuration_advice._devices[i];
                      if (!updated && ((int) device._capacity_mb -
                                       (int) (device._used_capacity_mb +
                                              advice._size_mb)) >= 0) {
                        fs::create_directories(device._mount_point);
                        filename.replace(0,
                                         client->_job_configuration_advice
                                             ._devices[advice._current_device]
                                             ._mount_point.size(),
                                         device._mount_point);
                        if (!is_read_only || fs::exists(filename)) {
                          device._used_capacity_mb =
                              device._used_capacity_mb + advice._size_mb;
                          mimir::Logger::Instance("ATHENA")->log(
                              mimir::LOG_INFO, "Opening file on %s for file %s",
                              device._mount_point.c_str(), path);
                          updated = true;
                        } else {
                          mimir::Logger::Instance("ATHENA")->log(
                              mimir::LOG_INFO,
                              "File %s is not opened in READONLY and it does "
                              "not "
                              "exists. Reverting to original path.",
                              path);
                          filename = std::string(path);
                        }
                      }
                      updated_devices.push_back(device);
                    }
                    if (updated) {
                      client->_job_configuration_advice._devices =
                          updated_devices;
                      client->_job_handler->save_advice(
                          job_conf_key, client->_job_configuration_advice);
                      client->mapped_files_emplace(std::string(path),
                                                   filename);
                    }
                  }
                }
                break;
              }
              case mimir::OperationAdviceType::TMP_FILE: {
                /** do nothing **/
                break;
              }
              case mimir::OperationAdviceType::EVENTUAL_CONSISTENCY: {
                /** perform async I/O **/
                break;
              }
              case mimir::OperationAdviceType::WRITE_ONLY_FILE: {
                /** perform async I/O **/
                break;
              }
              case mimir::OperationAdviceType::READ_ONLY_FILE: {
                mimir::Logger::Instance("ATHENA")->log(
                    mimir::LOG_INFO,
                    "Applying read-only file advice for file %s", path);
              }
              case mimir::OperationAdviceType::INPUT_FILE: {
                /** perform prefetching on input files **/
                if (advice._type._secondary ==
                    mimir::OperationAdviceType::INPUT_FILE)
                  mimir::Logger::Instance("ATHENA")->log(
                      mimir::LOG_INFO, "Applying input file advice for file %s",
                      path);
                server_index = std::hash<std::string>()(filename) %
                               client->_job_configuration_advice._num_nodes;

                if (enable_rpc && my_server_index != server_index) {
                  mimir::Logger::Instance("ATHENA")->log(
                      mimir::LOG_INFO,
                      "Perform RPC on server %d open for file %s", server_index,
                      path);
                  auto rpc_ret =
                      client->_rpc->call<RPCLIB_MSGPACK::object_handle>(
                          server_index, "athena::posix::open", filename, flags,
                          mode);
                  ret = rpc_ret.as<int>();
                  perform_io = false;
                } else {
                  mimir::Logger::Instance("ATHENA")->log(
                      mimir::LOG_INFO, "Perform Local open for file %s", path);
                  mimir::Storage device;
                  device = client->_job_configuration_advice._devices[0];
                  filename.replace(0,
                                   client->_job_configuration_advice
                                       ._devices[advice._current_device]
                                       ._mount_point.size(),
                                   device._mount_point);
                }
                break;
              }
              case mimir::OperationAdviceType::PLACEMENT_FILE: {
                /** perform user defined placement on files **/
                server_index = std::hash<std::string>()(filename) %
                               client->_job_configuration_advice._num_nodes;
                if (enable_rpc && my_server_index != server_index) {
                  mimir::Logger::Instance("ATHENA")->log(
                      mimir::LOG_INFO,
                      "Perform RPC on server %d open for file %s", server_index,
                      path);
                  auto rpc_ret =
                      client->_rpc->call<RPCLIB_MSGPACK::object_handle>(
                          server_index, "athena::posix::open", filename, flags,
                          mode);
                  ret = rpc_ret.as<int>();
                  perform_io = false;
                } else {
                  mimir::Logger::Instance("ATHENA")->log(
                      mimir::LOG_INFO, "Perform Local open for file %s", path);
                  mimir::Storage device;
                  if (advice._placement_device != advice._current_device) {
                    device = client->_job_configuration_advice
                        ._devices[advice._placement_device];
                    filename.replace(0,
                                     client->_job_configuration_advice
                                         ._devices[advice._current_device]
                                         ._mount_point.size(),
                                     device._mount_point);
                  }
                }
                break;
              }
              case mimir::OperationAdviceType::OUTPUT_FILE: {
                /** make sure we store on PFS **/
                break;
              }
              case mimir::OperationAdviceType::STRONG_CONSISTENCY: {
                /** make sure we flush I/O **/
                break;
              }
            }
          }
          if (flags & O_DIRECT) {
            flags ^= O_DIRECT;
          }

          if (flags & O_SYNC) {
            flags ^= O_SYNC;
          }
        }
      } else {
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_INFO, "Mapping %s found for file %s",
            mapped_file.second.c_str(), path);
        filename = mapped_file.second;
        if (flags & O_DIRECT) {
          flags ^= O_DIRECT;
        }
        if (flags & O_SYNC) {
          flags ^= O_SYNC;
        }
      }
      if (perform_io) {
        ret = real_open64_(filename.c_str(), flags, mode);
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_INFO, "Opened file %s with fd %d on rank %d",
            filename.c_str(), ret, current_rank);
        if (ret == -1) {
          mimir::Logger::Instance("ATHENA")->log(mimir::LOG_ERROR,
                                                 "Error %s opening file: %s",
                                                 strerror(errno), filename.c_str());
        }
      }
      client->id_server_map_emplace(ret, server_index);
    } else {
      if (perform_io) {
        mimir::Logger::Instance("ATHENA")->log(mimir::LOG_WARN,
                                               "Client not initialized");
        ret = real_open64_(filename.c_str(), flags, mode);
        if (ret == -1) {
          mimir::Logger::Instance("ATHENA")->log(mimir::LOG_ERROR,
                                                 "Error %s opening file: %s",
                                                 strerror(errno), filename.c_str());
        }
      }
    }
  } else {
    if (perform_io) {
      ret = real_open64_(filename.c_str(), flags, mode);
      if (!enable_rpc && ret == -1) {
        mimir::Logger::Instance("ATHENA")->log(mimir::LOG_ERROR,
                                               "Error %s opening file: %s",
                                               strerror(errno), filename.c_str());
      }
    }
  }
  if (!perform_io || strcmp(filename.c_str(), path) != 0) MIMIR_TRACKER()->track(ret);
  return ret;
}

ssize_t handle_read(int fd, void *buf, size_t count, bool enable_rpc) {
  ssize_t ret;
  /* TODO: add logic to read from remote*/
  bool perform_io = true;
  bool is_tracked = false;
  if (enable_rpc && IsTracked(fd)) {
    is_tracked = true;
    auto client = athena::PosixClient::Instance();
    if (client != nullptr) {
      auto iter = client->id_server_map_find(fd);
      if (iter.first) {
        auto file_server_index = iter.second;
        int current_rank = 0;
        if (is_mpi()) MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
        uint16_t my_server_index =
            floor(current_rank /
                  client->_job_configuration_advice._num_cores_per_node);
        if (my_server_index != file_server_index) {
          mimir::Logger::Instance("ATHENA")->log(
              mimir::LOG_INFO,
              "Perform RPC on server %d read for file_descriptor %d",
              file_server_index, fd);
          auto rpc_ret = client->_rpc->call<RPCLIB_MSGPACK::object_handle>(
              file_server_index, "athena::posix::read", fd, count);
          auto data = rpc_ret.as<DATA>();
          memcpy(buf, data.data(), count);
          ret = data.size();
          perform_io = false;
        }
      }
    }
  }
  if (perform_io) {
    MAP_OR_FAIL(read);
    ret = real_read_(fd, buf, count);
    if (is_tracked || !enable_rpc) {
      if (ret == -1) {
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_WARN, "Error %s Reading fd: %d", strerror(errno), fd);
      } else if (ret != count) {
        std::string str = GetFilenameFromFD(fd);
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_WARN, "Error %s Reading fd: %d file %s read only %d of %d",
            strerror(errno), fd, str.c_str(), ret, count);
      }
    }

  }
  return ret;
}

ssize_t handle_write(int fd, const void *buf, size_t count, bool enable_rpc) {
  ssize_t ret;
  bool perform_io = true;
  bool is_tracked = false;
  int current_rank = 0;
  uint16_t my_server_index = 0;
  if (IsTracked(fd) && enable_rpc) {
    if (is_mpi()) MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    is_tracked = true;
    auto client = athena::PosixClient::Instance();
    if (client != nullptr) {
      auto iter = client->id_server_map_find(fd);
      if (iter.first) {
        auto file_server_index = iter.second;
        my_server_index =
            floor(current_rank /
                  client->_job_configuration_advice._num_cores_per_node);
        if (my_server_index != file_server_index) {
          DATA buf_data = DATA((char *) buf, (char *) buf + count);
          ret = client->_rpc
              ->call<RPCLIB_MSGPACK::object_handle>(
                  file_server_index, "athena::posix::write", fd, buf_data,
                  count)
              .as<ssize_t>();
          mimir::Logger::Instance("ATHENA")->log(
              mimir::LOG_INFO,
              "Perform RPC on server %d from rank %d with server_index %d "
              "write for file_descriptor "
              "%d and ret "
              "%d",
              file_server_index, current_rank, my_server_index, fd, ret);
          perform_io = false;
        }
      }
    }
  }
  std::string str = GetFilenameFromFD(fd);
  if (perform_io) {
    MAP_OR_FAIL(write);
    ret = real_write_(fd, buf, count);
    if (is_tracked || !enable_rpc) {
      if (ret == -1) {
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_WARN, "Error %s Writing fd: %d file %s ", strerror(errno), fd, str.c_str());
      } else if (ret != count) {
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_WARN, "Error %s Writing fd: %d file %s write only %d of %d",
            strerror(errno), fd, str.c_str(), ret, count);
      }
    }
  }
  return ret;
}

int handle_close(int fd, bool enable_rpc) {
  int ret;
  bool perform_io = true;
  if (enable_rpc && IsTracked(fd)) {
    auto client = athena::PosixClient::Instance();
    if (client != nullptr) {
      auto iter = client->id_server_map_find(fd);
      if (iter.first) {
        auto file_server_index = iter.second;
        int current_rank = 0;
        if (is_mpi()) MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
        uint16_t my_server_index =
            floor(current_rank /
                  client->_job_configuration_advice._num_cores_per_node);
        if (my_server_index != file_server_index) {
          mimir::Logger::Instance("ATHENA")->log(
              mimir::LOG_INFO,
              "Perform RPC on server %d close for file_descriptor %d",
              file_server_index, fd);
          ret = client->_rpc
              ->call<RPCLIB_MSGPACK::object_handle>(
                  file_server_index, "athena::posix::close", fd)
              .as<int>();
          perform_io = false;
        }
        client->id_server_map_erase(fd);
        MIMIR_TRACKER()->remove(fd);
      }
    }
  }
  if (perform_io) {
    MAP_OR_FAIL(close);
    ret = real_close_(fd);
  }
  return ret;
}

off64_t handle_lseek(int fd, off64_t offset, int whence, bool enable_rpc) {
  off64_t ret;

  bool perform_io = true;
  if (enable_rpc && IsTracked(fd)) {
    auto client = athena::PosixClient::Instance();
    if (client != nullptr) {
      auto iter = client->id_server_map_find(fd);
      if (iter.first) {
        auto file_server_index = iter.second;
        int current_rank = 0;
        if (is_mpi()) MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
        uint16_t my_server_index =
            floor(current_rank /
                  client->_job_configuration_advice._num_cores_per_node);
        if (my_server_index != file_server_index) {
          ret = client->_rpc
              ->call<RPCLIB_MSGPACK::object_handle>(
                  file_server_index, "athena::posix::lseek", fd, offset,
                  whence)
              .as<int>();
          mimir::Logger::Instance("ATHENA")->log(
              mimir::LOG_INFO,
              "Perform RPC on server %d close for file_descriptor %d and ret "
              "%d",
              file_server_index, fd, ret);
          perform_io = false;
        }
      }
    }
  }
  if (perform_io) {
    MAP_OR_FAIL(lseek64);
    ret = real_lseek64_(fd, offset, whence);
  }
  return ret;
}

int ATHENA_DECL(open64)(const char *path, int flags, ...) {
  va_list arg;
  va_start(arg, flags);
  int mode = va_arg(arg, int);
  va_end(arg);
  auto tracer = MIMIR_TRACKER();
  if (tracer != nullptr) tracer->local++;
  return handle_open(path, flags, mode, true);
}

int ATHENA_DECL(open)(const char *path, int flags, ...) {
  int ret;
  va_list arg;
  va_start(arg, flags);
  int mode = va_arg(arg, int);
  va_end(arg);

  auto tracer = MIMIR_TRACKER();
  if (tracer != nullptr) tracer->local++;
  return handle_open(path, flags, mode, true);
}

ssize_t ATHENA_DECL(read)(int fd, void *buf, size_t count) {

  auto tracer = MIMIR_TRACKER();
  if (tracer != nullptr) tracer->local++;
  return handle_read(fd, buf, count, true);
}

ssize_t ATHENA_DECL(write)(int fd, const void *buf, size_t count) {

  auto tracer = MIMIR_TRACKER();
  if (tracer != nullptr) tracer->local++;
  return handle_write(fd, buf, count, true);
}

int ATHENA_DECL(close)(int fd) {

  auto tracer = MIMIR_TRACKER();
  if (tracer != nullptr) tracer->local++;
  return handle_close(fd, true);
}

off64_t ATHENA_DECL(lseek64)(int fd, off64_t offset, int whence) {

  auto tracer = MIMIR_TRACKER();
  if (tracer != nullptr) tracer->local++;
  return handle_lseek(fd, offset, whence, true);
}

off_t ATHENA_DECL(lseek)(int fd, off_t offset, int whence) {

  auto tracer = MIMIR_TRACKER();
  if (tracer != nullptr) tracer->local++;
  return handle_lseek(fd, offset, whence, true);
}