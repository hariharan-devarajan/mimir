//
// Created by hariharan on 2/23/22.
//

#ifndef MIMIR_MIMIR_INTERCEPTOR_H
#define MIMIR_MIMIR_INTERCEPTOR_H
#include <cxxabi.h>
#include <mimir/log/logger.h>
#include <signal.h>

#include <cstring>

#include "job_configuration.h"

extern bool is_mpi();
extern void set_mpi();
extern bool is_exit();
extern void set_exit();
namespace mimir {
    class Tracker {
        static std::shared_ptr<Tracker> _instance;
        std::unordered_set<int> _track_fd;
        std::unordered_set<std::string> _track_files;
        std::unordered_set<std::string> _exclude_files;
    public:
        Tracker(): _track_fd(), _track_files(), _exclude_files() {}
        static std::shared_ptr<Tracker> Instance() {
            if (_instance == nullptr) {
                _instance = std::make_shared<Tracker>();
            }
            return _instance;
        }
        void track(int fd) {
            if (is_exit()) return;
            _track_fd.emplace(fd);
        }
        void track(std::string path) {
            if (is_exit()) return;
            _track_files.emplace(path);
        }

        void exclude(std::string path) {
            if (is_exit()) return;
            _exclude_files.emplace(path);
        }
        void unexclude(std::string path) {
            if (is_exit()) return;
            _exclude_files.erase(path);
        }
        void remove(int fd) {
            if (is_exit()) return;
            _track_fd.erase(fd);
        }
        void remove(std::string path) {
            if (is_exit()) return;
            _track_files.erase(path);
        }
        bool is_traced(int fd) {
            if (is_exit()) return false;
            if (fd != -1 && !_track_fd.empty()) {
                auto iter = _track_fd.find(fd);
                if (iter != _track_fd.end()) {
                    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                                           "Tracking file descriptor %d", fd);
                    return true;
                }
            }
            return false;
        }
        bool is_traced(std::string path) {
            if (is_exit()) return false;
            if (!_track_files.empty()) {
                auto iter = _track_files.find(path);
                if (iter != _track_files.end()) {
                    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                                           "Tracking file %s", path.c_str());
                    return true;
                }
            }
            return false;
        }
        bool is_excluded(std::string path) {
            if (is_exit()) return true;
            if (!_exclude_files.empty()) {
                auto iter = _exclude_files.find(path);
                if (iter != _exclude_files.end()) {
                    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                                           "Excluding file %s", path.c_str());
                    return true;
                }
            }
            return false;
        }
    };
}

#define MIMIR_TRACKER mimir::Tracker::Instance()
#include <cxxabi.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

/** Print a demangled stack backtrace of the caller function to FILE* out. */
static inline void print_stacktrace(FILE* out = stderr,
                                    unsigned int max_frames = 63) {
  fprintf(out, "stack trace:\n");

  // storage array for stack trace address data
  void* addrlist[max_frames + 1];

  // retrieve current stack addresses
  int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

  if (addrlen == 0) {
    fprintf(out, "  <empty, possibly corrupt>\n");
    return;
  }

  // resolve addresses into strings containing "filename(function+address)",
  // this array must be free()-ed
  char** symbollist = backtrace_symbols(addrlist, addrlen);

  // allocate string which will be filled with the demangled function name
  size_t funcnamesize = 256;
  char* funcname = (char*)malloc(funcnamesize);

  // iterate over the returned symbol lines. skip the first, it is the
  // address of this function.
  for (int i = 1; i < addrlen; i++) {
    char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

    // find parentheses and +address offset surrounding the mangled name:
    // ./module(function+0x15c) [0x8048a6d]
    for (char* p = symbollist[i]; *p; ++p) {
      if (*p == '(')
        begin_name = p;
      else if (*p == '+')
        begin_offset = p;
      else if (*p == ')' && begin_offset) {
        end_offset = p;
        break;
      }
    }

    if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
      *begin_name++ = '\0';
      *begin_offset++ = '\0';
      *end_offset = '\0';

      // mangled name is now in [begin_name, begin_offset) and caller
      // offset in [begin_offset, end_offset). now apply
      // __cxa_demangle():

      int status;
      char* ret =
          abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
      if (status == 0) {
        funcname = ret;  // use possibly realloc()-ed string
        fprintf(out, "  %s : %s+%s\n", symbollist[i], funcname, begin_offset);
      } else {
        // demangling failed. Output function name as a C function with
        // no arguments.
        fprintf(out, "  %s : %s()+%s\n", symbollist[i], begin_name,
                begin_offset);
      }
    } else {
      // couldn't parse the line? print the whole line.
      fprintf(out, "  %s\n", symbollist[i]);
    }
  }

  free(funcname);
  free(symbollist);
}

inline void handler_mimir(int sig) {
  void* array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);
  fprintf(stderr, "Error: signal %d:\n", sig);
  print_stacktrace();
  exit(1);
}
inline mimir::JobConfigurationAdvice load_job_details() {
  signal(SIGSEGV, handler_mimir);
  signal(SIGABRT, handler_mimir);  // install our handler
  mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                        "Loading job configuration");
  using namespace mimir;
  auto SHM = std::getenv("SHM_PATH");
  auto PFS = std::getenv("PFS_PATH");
  mimir::JobConfigurationAdvice job_conf_advice;
  job_conf_advice._job_id = 0;
  job_conf_advice._devices.emplace_back(SHM, 1024);
  job_conf_advice._devices.emplace_back(PFS, 128);
  job_conf_advice._job_time_minutes = 30;
  job_conf_advice._num_cores_per_node = 2;
  job_conf_advice._num_gpus_per_node = 0;
  job_conf_advice._num_nodes = 2;
  job_conf_advice._node_names.clear();
  job_conf_advice._node_names.push_back("lassen168");
  job_conf_advice._node_names.push_back("lassen168");
  job_conf_advice._rpc_port = 8888;
  job_conf_advice._rpc_threads = 1;
  job_conf_advice._priority = 100;
  return job_conf_advice;
}

#endif  // MIMIR_MIMIR_INTERCEPTOR_H
