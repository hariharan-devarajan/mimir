//
// Created by hariharan on 2/28/22.
//

#ifndef MIMIR_STDIO_H
#define MIMIR_STDIO_H

#include <athena/api/interceptor.h>

#include <cstdio>

ATHENA_FORWARD_DECL(fopen, FILE *, (const char *path, const char *mode));
ATHENA_FORWARD_DECL(fopen64, FILE *, (const char *path, const char *mode));
ATHENA_FORWARD_DECL(fclose, int, (FILE * fp));
ATHENA_FORWARD_DECL(fread, size_t,
                    (void *ptr, size_t size, size_t nmemb, FILE *stream));
ATHENA_FORWARD_DECL(fwrite, size_t,
                    (const void *ptr, size_t size, size_t nmemb, FILE *stream));

#endif  // MIMIR_STDIO_H
