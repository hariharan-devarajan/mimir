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

#include "athena/client/athena_client.h"

namespace fs = std::experimental::filesystem;

int athena::posix_open(std::vector<char> path, int flags, int mode) {
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
ssize_t athena::posix_write(int fd, std::vector<char> buf, size_t count) {
  ssize_t ret = write(fd, buf.data(), count);
  return ret;
}
std::vector<char> athena::posix_read(int fd, size_t count) {
  std::vector<char> data('r', count);
  ssize_t ret = read(fd, data.data(), count);
  return data;
}
