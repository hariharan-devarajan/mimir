//
// Created by hariharan on 2/23/22.
//

#include <mimir/api/mimir_interceptor.h>

namespace mimir {
bool is_mpi = false;
}

extern bool is_mpi() { return mimir::is_mpi; }
extern void set_mpi() { mimir::is_mpi = true; }