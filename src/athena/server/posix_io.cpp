//
// Created by hariharan on 2/21/22.
//

#include "posix_io.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <mimir/advice/file_advice.h>
#include <athena/api/posix.h>

#include <cstdio>
#include <experimental/filesystem>

#include "athena/client/posix_athena_client.h"

namespace fs = std::experimental::filesystem;

int athena::posix_open(DATA path, int flags, int mode) {

  MIMIR_TRACKER()->remote++;
  int ret = handle_open(path.data(), flags, mode, true);
  if (ret == -1) {
    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_ERROR,
                                           "Error %s remote opening file: %s",
                                           strerror(errno), path.data());
  }
  return ret;
}
int athena::posix_close(int fd) {
  MIMIR_TRACKER()->remote++;
  int ret = handle_close(fd, false);
  return 0;
}
off_t athena::posix_lseek(int fd, off_t offset, int whence) {
  MIMIR_TRACKER()->remote++;
  off_t ret = handle_lseek(fd, offset, whence, false);
  return ret;
}
ssize_t athena::posix_write(int fd, DATA buf, size_t count) {
  MIMIR_TRACKER()->remote++;
  ssize_t ret = handle_write(fd, buf.data(), count, false);
  return ret;
}
DATA athena::posix_read(int fd, size_t count) {
  MIMIR_TRACKER()->remote++;
  DATA data_str(count,'r');
  ssize_t ret = handle_read(fd, data_str.data(), count, false);
  data_str.resize(ret);
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
