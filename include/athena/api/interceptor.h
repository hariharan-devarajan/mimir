//
// Created by haridev on 2/14/22.
//

#ifndef ATHENA_INTERCEPTOR_H
#define ATHENA_INTERCEPTOR_H
#ifdef ATHENA_PRELOAD
#include <dlfcn.h>
    /*
     * Declare the function signatures for real functions
     * i.e. The real function point to fwrite would be defined as __athena_fwrite
     */
    #define ATHENA_FORWARD_DECL(name, ret, args) ret(*__mimir_##name) args;

    /* Point __athena_func to the real funciton using dlsym() */
    #define MAP_OR_FAIL(func)                                                   \
        if (!(__athena_##func)) {                                                 \
            __athena_##func = dlsym(RTLD_NEXT, #func);                            \
            if (!(__athena_##func)) {                                             \
                printf("Athena failed to map symbol: %s\n", #func);           \
            }                                                                   \
        }
    /*
     * Call the real funciton
     * Before call the real function, we need to make sure its mapped by dlsym()
     * So, every time we use this marco directly, we need to call MAP_OR_FAIL before it
     */
    #define ATHENA_POSIX_DECL(func) __warp_##func
    #define ATHENA_REAL_CALL(func) __athena_##func

#else
#define ATHENA_FORWARD_DECL(name, ret, args)
#define MAP_OR_FAIL(func)
#define ATHENA_REAL_CALL(func) func
#endif
#endif //ATHENA_INTERCEPTOR_H
