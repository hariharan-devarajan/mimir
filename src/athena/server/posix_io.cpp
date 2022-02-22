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
int athena::posix_open(std::string path, int mode, int flags) {
  return open(path.c_str(), mode, flags);
}
int athena::posix_close(int fd) { return close(fd); }
off_t athena::posix_lseek(int fd, off_t offset, int whence) {
  return posix_lseek(fd, offset, whence);
}
ssize_t athena::posix_write(int fd, std::string buf, int count) {
  return write(fd, buf.data(), count);
}
std::string athena::posix_read(int fd, int count) {
  std::string buf;
  buf.reserve(count);
  read(fd, buf.data(), count);
  return buf;
}
