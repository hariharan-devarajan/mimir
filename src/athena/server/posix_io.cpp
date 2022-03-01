//
// Created by hariharan on 2/21/22.
//

#include "posix_io.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <mimir/advice/file_advice.h>

#include <cstdio>
#include <experimental/filesystem>
#include <thallium.hpp>

#include "athena/client/posix_athena_client.h"

namespace fs = std::experimental::filesystem;

int athena::posix_open(DATA path, int flags, int mode) {
  int ret = open(path.data(), flags, mode);
  return ret;
}
int athena::posix_close(int fd) {
  int ret = close(fd);
  return ret;
}
off_t athena::posix_lseek(int fd, off_t offset, int whence) {
  off_t ret = lseek(fd, offset, whence);
  return ret;
}
ssize_t athena::posix_write(int fd, DATA buf, size_t count) {
  ssize_t ret = write(fd, buf.data(), count);
  return ret;
}
DATA athena::posix_read(int fd, size_t count) {
  DATA data_str;
  char* data = (char*)malloc(count);
  ssize_t ret = read(fd, data, count);
  data_str.assign(data, ret);
  free(data);
  mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO, "read data size %d",
                                         data_str.size());
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
    fs::copy(filename.data(), new_path, fs::copy_options::update_existing);
  }
  return true;
}
