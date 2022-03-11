//
// Created by hariharan on 2/23/22.
//

#include <mimir/api/mimir_interceptor.h>

namespace mimir {
    bool is_mpi = false;
    bool is_tracing = false;
    mimir::Tracker* tracker_instance = nullptr;
}

extern bool is_mpi() { return mimir::is_mpi; }
extern void set_mpi() { mimir::is_mpi = true; }

extern bool is_tracing() { return mimir::is_tracing;}

extern void init_mimir() {
    mimir::is_tracing = true;
    mimir::tracker_instance = new mimir::Tracker();
}
extern void finalize_mimir() {
    mimir::is_tracing = false;
    delete mimir::tracker_instance;
}

extern mimir::Tracker* MIMIR_TRACKER() {
    return mimir::tracker_instance;
}