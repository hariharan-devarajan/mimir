//
// Created by haridev on 2/14/22.
//

#ifndef ATHENA_INTERCEPTOR_H
#define ATHENA_INTERCEPTOR_H
#ifdef ATHENA_PRELOAD
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
