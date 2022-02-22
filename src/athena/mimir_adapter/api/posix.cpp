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

#include "athena/client/athena_client.h"

namespace fs = std::experimental::filesystem;
int ATHENA_DECL(open64)(const char *path, int flags, ...) {
  int ret;
  va_list arg;
  va_start(arg, flags);
  int mode = va_arg(arg, int);
  va_end(arg);
  MAP_OR_FAIL(open64);
  bool perform_io = true;
  std::string filename(path);
  if (IsTracked(path)) {
    auto client = athena::Client::Instance(true);
    if (client != nullptr) {
      athena::Client::Instance(true);

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

      int current_rank;
      MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
      uint16_t my_server_index = ceil(
          current_rank / client->_job_configuration_advice._num_cores_per_node);
      uint16_t server_index = my_server_index;
      auto mapped_file = client->_mapped_files.find(filename);
      if (mapped_file == client->_mapped_files.end()) {
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_INFO, "No Mapping found for file %s", path);
        if (file_advice_handler->is_advice_present(file_key)) {
          auto advices = file_advice_handler->resolve_conflicts(file_key);
          for (const auto &advice : advices) {
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
                    if (!updated && ((int)device._capacity_mb -
                                     (int)(device._used_capacity_mb +
                                           advice._size_mb)) >= 0) {
                      fs::create_directories(device._mount_point);
                      filename.replace(0, advice._device._mount_point.size(),
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
                    client->_mapped_files.emplace(std::string(path), filename);
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

                if (my_server_index != server_index) {
                  mimir::Logger::Instance("ATHENA")->log(
                      mimir::LOG_INFO,
                      "Perform RPC on server %d open for file %s", server_index,
                      path);
                  ret = client->_rpc->call<int>(server_index,
                                                "athena::posix::open", filename,
                                                mode, flags);
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
                      if (!updated && ((int)device._capacity_mb -
                                       (int)(device._used_capacity_mb +
                                             advice._size_mb)) >= 0) {
                        fs::create_directories(device._mount_point);
                        filename.replace(0, advice._device._mount_point.size(),
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
                      client->_mapped_files.emplace(std::string(path),
                                                    filename);
                      track_files.emplace_back(filename);
                      track_files.emplace_back(std::string(path));
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
                /** perform prefetching **/
                break;
              }
              case mimir::OperationAdviceType::INPUT_FILE: {
                /** perform prefetching on input files **/
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
        }
      } else {
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_INFO, "Mapping %s found for file %s",
            mapped_file->second.c_str(), path);
        filename = mapped_file->second;
        if (flags & O_DIRECT) {
          flags ^= O_DIRECT;
        }
      }
      if (perform_io) {
        ret = real_open64_(filename.c_str(), flags, mode);
        mimir::Logger::Instance("ATHENA")->log(
            mimir::LOG_INFO, "Opened file %s with fd %d", path, ret);
        if (ret == -1) {
          mimir::Logger::Instance("ATHENA")->log(mimir::LOG_ERROR,
                                                 "Error %s opening file: %s",
                                                 strerror(errno), path);
        }
      }
      client->_fd_server.emplace(ret, server_index);
    } else {
      if (perform_io) {
        mimir::Logger::Instance("ATHENA")->log(mimir::LOG_WARN,
                                               "Client not initialized");
        ret = real_open64_(filename.c_str(), flags, mode);
        if (ret == -1) {
          mimir::Logger::Instance("ATHENA")->log(mimir::LOG_ERROR,
                                                 "Error %s opening file: %s",
                                                 strerror(errno), path);
        }
      }
    }
  } else {
    if (perform_io) {
      ret = real_open64_(filename.c_str(), flags, mode);
      if (ret == -1) {
        mimir::Logger::Instance("ATHENA")->log(mimir::LOG_ERROR,
                                               "Error %s opening file: %s",
                                               strerror(errno), path);
      }
    }
  }
  return ret;
}

int ATHENA_DECL(open)(const char *path, int flags, ...) {
  int ret;
  va_list arg;
  va_start(arg, flags);
  int mode = va_arg(arg, int);
  va_end(arg);
  ret = open64(path, flags, mode);
  return ret;
}

ssize_t ATHENA_DECL(read)(int fd, void *buf, size_t count) {
  size_t ret;
  /* TODO: add logic to read from remote*/
  bool perform_io = true;
  if (IsTracked(fd)) {
    auto client = athena::Client::Instance();
    if (client != nullptr) {
      auto iter = client->_fd_server.find(fd);
      if (iter != client->_fd_server.end()) {
        auto file_server_index = iter->second;
        int current_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
        uint16_t my_server_index =
            ceil(current_rank /
                 client->_job_configuration_advice._num_cores_per_node);
        if (my_server_index != file_server_index) {
          mimir::Logger::Instance("ATHENA")->log(
              mimir::LOG_INFO,
              "Perform RPC on server %d read for file_descriptor %d",
              file_server_index, fd);
          auto data = client->_rpc->call<std::string>(
              file_server_index, "athena::posix::read", fd, count);
          memcpy(buf, data.c_str(), count);
          ret = data.size();
          perform_io = false;
        }
      }
    }
  }
  if (perform_io) {
    MAP_OR_FAIL(read);
    ret = real_read_(fd, buf, count);
  }
  return ret;
}

ssize_t ATHENA_DECL(write)(int fd, const void *buf, size_t count) {
  ssize_t ret;
  bool perform_io = true;
  bool is_tracked = false;
  int current_rank = 0;
  uint16_t my_server_index = 0;
  if (IsTracked(fd)) {
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    is_tracked = true;
    auto client = athena::Client::Instance();
    if (client != nullptr) {
      auto iter = client->_fd_server.find(fd);
      if (iter != client->_fd_server.end()) {
        auto file_server_index = iter->second;

        my_server_index =
            ceil(current_rank /
                 client->_job_configuration_advice._num_cores_per_node);
        if (my_server_index != file_server_index) {
          ret = client->_rpc->call<ssize_t>(file_server_index,
                                            "athena::posix::write", fd,
                                            std::string((char *)buf), count);
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
  if (perform_io) {
    MAP_OR_FAIL(write);
    ret = real_write_(fd, buf, count);
    if (is_tracked)
      mimir::Logger::Instance("ATHENA")->log(
          mimir::LOG_INFO,
          "Perform Write on Local on rank %d with server_index %d for "
          "file_descriptor %d and ret %d",
          current_rank, my_server_index, fd, ret);
  }
  return ret;
}

int ATHENA_DECL(close)(int fd) {
  int ret;
  bool perform_io = true;
  if (IsTracked(fd)) {
    auto client = athena::Client::Instance();
    if (client != nullptr) {
      auto iter = client->_fd_server.find(fd);
      if (iter != client->_fd_server.end()) {
        auto file_server_index = iter->second;
        int current_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
        uint16_t my_server_index =
            ceil(current_rank /
                 client->_job_configuration_advice._num_cores_per_node);
        if (my_server_index != file_server_index) {
          mimir::Logger::Instance("ATHENA")->log(
              mimir::LOG_INFO,
              "Perform RPC on server %d close for file_descriptor %d",
              file_server_index, fd);
          ret = client->_rpc->call<int>(file_server_index,
                                        "athena::posix::close", fd);
          perform_io = false;
        }
      }
    }
  }
  if (perform_io) {
    MAP_OR_FAIL(close);
    ret = real_close_(fd);
  }
  return ret;
}

off64_t ATHENA_DECL(lseek64)(int fd, off64_t offset, int whence) {
  off64_t ret;

  bool perform_io = true;
  if (IsTracked(fd)) {
    auto client = athena::Client::Instance();
    if (client != nullptr) {
      auto iter = client->_fd_server.find(fd);
      if (iter != client->_fd_server.end()) {
        auto file_server_index = iter->second;
        int current_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
        uint16_t my_server_index =
            ceil(current_rank /
                 client->_job_configuration_advice._num_cores_per_node);
        if (my_server_index != file_server_index) {
          ret = client->_rpc->call<int>(
              file_server_index, "athena::posix::lseek", fd, offset, whence);
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

off_t ATHENA_DECL(lseek)(int fd, off_t offset, int whence) {
  off_t ret;
  ret = lseek64(fd, offset, whence);
  return ret;
}