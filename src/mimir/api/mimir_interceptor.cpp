//
// Created by hariharan on 2/23/22.
//

#include <mimir/api/mimir_interceptor.h>

namespace mimir {
    bool is_mpi = false;
    bool exit = false;
}


std::shared_ptr<mimir::Tracker> mimir::Tracker::_instance = nullptr;

extern bool is_mpi() { return mimir::is_mpi; }
extern void set_mpi() { mimir::is_mpi = true; }

extern bool is_exit() { return mimir::exit;}
extern void set_exit() { mimir::exit = true; }