//
// Created by hariharan on 2/21/22.
//

#include "posix_io.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <mimir/advice/file_advice.h>

#include <cstdio>
#include <experimental/filesystem>

#include "athena/client/posix_athena_client.h"

namespace fs = std::experimental::filesystem;

int athena::posix_open(DATA path, int flags, int mode) {
  int ret = open(path.data(), flags, mode);
  return ret;
}
int athena::posix_close(int fd) {
    MIMIR_TRACKER()->remote(fd);
  int ret = close(fd);
    MIMIR_TRACKER()->remote_remove(fd);
  return 0;
}
off_t athena::posix_lseek(int fd, off_t offset, int whence) {
    MIMIR_TRACKER()->remote(fd);
  off_t ret = lseek(fd, offset, whence);
    MIMIR_TRACKER()->remote_remove(fd);
  return ret;
}
ssize_t athena::posix_write(int fd, DATA buf, size_t count) {
    MIMIR_TRACKER()->remote(fd);
  ssize_t ret = write(fd, buf.data(), count);
    MIMIR_TRACKER()->remote_remove(fd);
  return ret;
}
DATA athena::posix_read(int fd, size_t count) {
    MIMIR_TRACKER()->remote(fd);
  DATA data_str(count,'r');
  ssize_t ret = read(fd, data_str.data(), count);
  mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO, "read data size %d",
                                         ret);

    MIMIR_TRACKER()->remote_remove(fd);
    if (ret != count) {
        mimir::Logger::Instance("ATHENA")->log(
                mimir::LOG_ERROR, "Error %s Writing fd: %d written only %d of %d",
                strerror(errno), fd, ret, count);
    }
  return data_str;
}
bool athena::posix_prefetch(DATA filename) {
  auto client = athena::PosixClient::Instance();
  fs::create_directories(
      client->_job_configuration_advice._devices[0]._mount_point);
  fs::path new_path =
      client->_job_configuration_advice._devices[0]._mount_point /
      fs::path(filename.data()).filename();
  if (fs::exists(filename.data())) {
    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                           "Copying file %s into file %s",
                                           filename.data(), new_path.c_str());
    MIMIR_TRACKER()->exclude(filename);
    fs::copy(filename.data(), new_path, fs::copy_options::update_existing);
    MIMIR_TRACKER()->unexclude(filename);
    athena::PosixClient::Instance()->mapped_files_emplace(filename,new_path);
  }
  return true;
}
