//
// Created by hariharan on 2/28/22.
//

#include <athena/api/stdio.h>
#include <fcntl.h>

FILE *ATHENA_DECL(fopen64)(const char *path, const char *mode) {
  FILE *ret;

  if (IsTracked(path)) {
    ret = static_cast<FILE *>(malloc(sizeof(FILE)));

    int flags = 0;
    if (strcmp(mode, "r") == 0 || strcmp(mode, "rb") == 0) flags |= O_RDONLY;
    if (strcmp(mode, "r+") == 0 || strcmp(mode, "w+") == 0)
      flags |= O_WRONLY | O_CREAT;
    if (strcmp(mode, "w+") == 0) flags |= O_TRUNC | O_CREAT;
    if (strcmp(mode, "a+") == 0 || strcmp(mode, "a") == 0)
      flags |= O_APPEND | O_CREAT;
    int mode_i = S_IRWXU | S_IRWXG | S_IRWXO;

    int ret_fd = open(path, flags, mode_i);
    ret->_fileno = ret_fd;
    ret->_flags = flags;
    ret->_flags2 = flags;
    ret->_mode = mode_i;
  } else {
    MAP_OR_FAIL(fopen64)
    ret = real_fopen64_(path, mode);
  }
  return ret;
}

FILE *ATHENA_DECL(fopen)(const char *path, const char *mode) {
  FILE *ret = fopen64(path, mode);
  return ret;
}
int ATHENA_DECL(fclose)(FILE *stream) {
  int ret;
  if (IsTracked(stream->_fileno)) {
    ret = close(stream->_fileno);
  } else {
    MAP_OR_FAIL(fclose)
    ret = real_fclose_(stream);
  }
  return ret;
}
size_t ATHENA_DECL(fread)(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t ret;
  if (IsTracked(stream->_fileno)) {
    ret = read(stream->_fileno, ptr, size * nmemb);
  } else {
    MAP_OR_FAIL(fread)
    ret = real_fread_(ptr, size, nmemb, stream);
  }
  return ret;
}

size_t ATHENA_DECL(fwrite)(const void *ptr, size_t size, size_t nmemb,
                           FILE *stream) {
  size_t ret;
  if (IsTracked(stream->_fileno)) {
    ret = write(stream->_fileno, ptr, size * nmemb);
  } else {
    MAP_OR_FAIL(fwrite)
    ret = real_fwrite_(ptr, size, nmemb, stream);
  }
  return ret;
}