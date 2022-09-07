//
// Created by haridev on 2/14/22.
//

#ifndef ATHENA_INTERCEPTOR_H
#define ATHENA_INTERCEPTOR_H
#include <string>
#include <unistd.h>
#include <unordered_set>
#include <execinfo.h>
#include <mimir/macro.h>
#include <mimir/api/mimir_interceptor.h>
#include "mimir/advice/job_configuration_advice.h"
#include "mimir/api/job_configuration.h"
#ifdef ATHENA_PRELOAD


inline std::string GetFilenameFromFD(int fd) {
  const int kMaxSize = 256;
  char proclnk[kMaxSize];
  char filename[kMaxSize];
  snprintf(proclnk, kMaxSize, "/proc/self/fd/%d", fd);
  size_t r = readlink(proclnk, filename, kMaxSize);
  filename[r] = '\0';
  return filename;
}
bool IsTracked(std::string path, int fd = -1);
inline bool IsTracked(int fd) {
  if (!is_tracing()) return false;
  return IsTracked("", fd);
}

#include <dlfcn.h>

#include <cstddef>
/**
 * Typedef and function declaration for intercepted functions.
 */
#define ATHENA_FORWARD_DECL(func_, ret_, args_) \
  typedef ret_(*real_t_##func_##_) args_;       \
  static ret_(*real_##func_##_) args_ = NULL;

#define ATHENA_DECL(func_) func_
/**
 * The input function is renamed as real_<func_name>_. And a ptr to function
 * is obtained using dlsym.
 */
#define MAP_OR_FAIL(func_)                                         \
  if (!(real_##func_##_)) {                                        \
    real_##func_##_ = (real_t_##func_##_)dlsym(RTLD_NEXT, #func_); \
    if (!(real_##func_##_)) {                                      \
      fprintf(stderr, "ATHENA Adapter failed to map symbol\n");    \
    }                                                              \
  }
#else
#define ATHENA_FORWARD_DECL(name, ret, args)
#define MAP_OR_FAIL(func)
#define ATHENA_DECL(func) func
#endif

#endif  // ATHENA_INTERCEPTOR_H
