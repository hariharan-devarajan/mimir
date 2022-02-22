//
// Created by haridev on 2/14/22.
//

#ifndef ATHENA_POSIX_H
#define ATHENA_POSIX_H

#include <athena/api/interceptor.h>
#include <fcntl.h>
#include <unistd.h>

ATHENA_FORWARD_DECL(open64, int, (const char *path, int flags, ...));
ATHENA_FORWARD_DECL(open, int, (const char *path, int flags, ...));
ATHENA_FORWARD_DECL(close, int, (int fd));
ATHENA_FORWARD_DECL(write, ssize_t, (int fd, const void *buf, size_t count));
ATHENA_FORWARD_DECL(read, ssize_t, (int fd, void *buf, size_t count));
ATHENA_FORWARD_DECL(lseek, off_t, (int fd, off_t offset, int whence));
ATHENA_FORWARD_DECL(lseek64, off64_t, (int fd, off64_t offset, int whence));
#endif  // ATHENA_POSIX_H
