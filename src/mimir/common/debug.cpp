//
// Created by hariharan on 3/8/22.
//
#include <mimir/common/debug.h>

#if defined(MIMIR_TRACE) || defined(MIMIR_TIMER)
int mimir::AutoTrace::rank = -1;
int mimir::AutoTrace::item = 0;
#endif