//
// Created by haridev on 2/14/22.
//

#ifndef ATHENA_POSIX_H
#define ATHENA_POSIX_H

#include <unistd.h>
#include <athena/api/interceptor.h>
#include <mimir/api/posix.h>

ATHENA_FORWARD_DECL(open64, int, (const char *path, int flags, ...));
ATHENA_FORWARD_DECL(open, int, (const char *path, int flags, ...));
ATHENA_FORWARD_DECL(close, int, (int fd));
ATHENA_FORWARD_DECL(write, ssize_t, (int fd, const void *buf, size_t count));
ATHENA_FORWARD_DECL(read, ssize_t, (int fd, void *buf, size_t count));
#endif //ATHENA_POSIX_H
