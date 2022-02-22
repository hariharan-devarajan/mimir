//
// Created by hariharan on 2/21/22.
//

#ifndef ATHENA_POSIX_IO_H
#define ATHENA_POSIX_IO_H

#include <string>
namespace athena {
int posix_open(std::string filename, int mode, int flags);
int posix_close(int fd);
off_t posix_lseek(int fd, off_t offset, int whence);
ssize_t posix_write(int fd, std::string buf, size_t count);
std::string posix_read(int fd, size_t count);
}  // namespace athena

#endif  // ATHENA_POSIX_IO_H
