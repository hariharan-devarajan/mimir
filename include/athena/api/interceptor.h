//
// Created by haridev on 2/14/22.
//

#ifndef ATHENA_INTERCEPTOR_H
#define ATHENA_INTERCEPTOR_H
#include <string>
#include <unistd.h>
#include <vector>
#include <execinfo.h>
#include "mimir/log/logger.h"
#include <mimir/api/mimir_interceptor.h>
#ifdef ATHENA_PRELOAD

void OnExit(void);
extern const char* kPathExclusions[15];
extern const char* kExtensionExclusions[1];
extern std::vector<std::string> track_files;
#define BT_BUF_SIZE 100
inline void print_backtrace(void) {
  int nptrs;
  void* buffer[BT_BUF_SIZE];
  char** strings;

  nptrs = backtrace(buffer, BT_BUF_SIZE);
  fprintf(stdout, "backtrace() returned %d addresses\n", nptrs);

  /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
     would produce similar output to the following: */

  strings = backtrace_symbols(buffer, nptrs);
  if (strings == NULL) {
    perror("backtrace_symbols");
    exit(EXIT_FAILURE);
  }

  for (int j = 0; j < nptrs; j++) fprintf(stdout, "%s\n", strings[j]);

  free(strings);
}

inline std::string GetFilenameFromFD(int fd) {
  const int kMaxSize = 256;
  char proclnk[kMaxSize];
  char filename[kMaxSize];
  snprintf(proclnk, kMaxSize, "/proc/self/fd/%d", fd);
  size_t r = readlink(proclnk, filename, kMaxSize);
  filename[r] = '\0';
  return filename;
}
bool IsTracked(std::string path);
inline bool IsTracked(int fd) {
  std::string file = GetFilenameFromFD(fd);
  return IsTracked(file);
}

#include <dlfcn.h>

#include <cstddef>
/**
 * Typedef and function declaration for intercepted functions.
 */
#define ATHENA_FORWARD_DECL(func_, ret_, args_) \
  typedef ret_(*real_t_##func_##_) args_;       \
  ret_(*real_##func_##_) args_ = NULL;

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
