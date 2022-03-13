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
#include <cxxabi.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <mutex>
#include <shared_mutex>
#include <experimental/filesystem>
#include <fstream>
namespace fs = std::experimental::filesystem;

/** Print a demangled stack backtrace of the caller function to FILE* out. */
static inline void print_stacktrace(FILE* out = stderr, unsigned int max_frames = 63) {
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
  fprintf(stderr, "Error: signal %d Waiting:\n", sig);
    fflush(stdout);
  //  getchar();
  print_stacktrace();
  exit(0);
}
extern bool is_mpi();
extern void set_mpi();
extern bool is_tracing();
extern void init_mimir();
extern void finalize_mimir();
namespace mimir {
    class Tracker {
        std::unordered_set<int> _track_fd;
        std::unordered_set<std::string> _track_files;
        std::unordered_set<std::string> _exclude_files;
        mutable std::shared_mutex mutex_exclude_files;


        mutable std::shared_mutex mutex_tracked_files;
        mutable std::shared_mutex mutex_tracked_fd;
    public:
        std::atomic<int> local, remote;
        ~Tracker() {
            mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                                           "Destructing Tracker");
        }
        Tracker(): _track_fd(), _track_files(), _exclude_files(), local(0), remote(0){
            signal(SIGSEGV, handler_mimir);
            signal(SIGABRT, handler_mimir);  // install our handler
            mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                                           "Constructing Tracker");
        }
        void track(int fd) {
            if (!is_tracing()) return;
            std::unique_lock lock(mutex_tracked_fd);
            _track_fd.emplace(fd);
        }
        void track(std::string path) {
            if (!is_tracing()) return;
            std::unique_lock lock(mutex_tracked_files);
            _track_files.emplace(path);
        }

        void exclude(std::string path) {
            if (!is_tracing()) return;
            std::unique_lock lock(mutex_exclude_files);
            _exclude_files.emplace(path);
        }
        void unexclude(std::string path) {
            if (!is_tracing()) return;
            std::unique_lock lock(mutex_exclude_files);
            _exclude_files.erase(path);
        }
        void remove(int fd) {
            if (!is_tracing()) return;
            std::unique_lock lock(mutex_tracked_fd);
            _track_fd.erase(fd);
        }
        void remove(std::string path) {
            if (!is_tracing()) return;
            std::unique_lock lock(mutex_tracked_files);
            _track_files.erase(path);
        }
        bool is_traced(int fd) {
            if (!is_tracing()) return false;
            std::shared_lock lock(mutex_tracked_fd);
            if (fd != -1 && !_track_fd.empty()) {
                auto iter = _track_fd.find(fd);
                if (iter != _track_fd.end()) {
                    mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                                           "Tracking file descriptor %d", fd);
                    return true;
                }
            }
            return false;
        }
        bool is_traced(std::string path) {
            if (!is_tracing()) return false;
            std::shared_lock lock(mutex_tracked_files);
            if (!_track_files.empty()) {
                auto iter = _track_files.find(path);
                if (iter != _track_files.end()) {
                    mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                                           "Tracking file %s", path.c_str());
                    return true;
                }
            }
            return false;
        }
        bool is_excluded(std::string path) {
            if (!is_tracing()) return true;
            std::shared_lock lock(mutex_exclude_files);
            if (!_exclude_files.empty()) {
                auto iter = _exclude_files.find(path);
                if (iter != _exclude_files.end()) {
                    mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                                           "Excluding file %s", path.c_str());
                    return true;
                }
            }
            return false;
        }
    };
}

extern mimir::Tracker* MIMIR_TRACKER();

inline std::vector<std::string> split_string(std::string x, char delim = ' '){
    x += delim; //includes a delimiter at the end so last word is also read

    auto set_splitted = std::unordered_set<std::string>();
    std::string temp = "";
    int count = 0;
    auto splitted = std::vector<std::string>();
    for (int i = 0; i < x.length(); i++) {
        if (x[i] == delim) {
            if (count > 0) {
                auto iter = set_splitted.find(temp);
                if (iter == set_splitted.end()) {
                    set_splitted.emplace(temp);
                    splitted.push_back(temp);
                }
            }
            temp = "";
            i++;
            count ++;
        }
        temp += x[i];
    }
    return splitted;
}
inline mimir::JobConfigurationAdvice load_job_details() {

  using namespace mimir;
  mimir::JobConfigurationAdvice job_conf_advice;
  auto CONFIG = std::getenv("CONFIG_PATH");
  if (CONFIG != nullptr && fs::exists(CONFIG)) {
      std::ifstream input(CONFIG);
      input.seekg(0, std::ios::end);
      size_t size = input.tellg();
      std::string buffer(size, ' ');
      input.seekg(0);
      input.read(&buffer[0], size);
      input.close();
      using json = nlohmann::json;
      json read_json = json::parse(buffer);
      read_json["job"].get_to(job_conf_advice);
      mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                            "Loading job configuration from file %s", CONFIG);
  } else {
      auto SHM = std::getenv("SHM_PATH");
      auto PFS = std::getenv("PFS_PATH");
      auto LSB_HOSTS = std::getenv("LSB_HOSTS");
      if (LSB_HOSTS == nullptr) {
          LSB_HOSTS = "localhost";
      }
      auto node_names = split_string(LSB_HOSTS);

      mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                            "Loading Default job configuration %s", node_names[0].c_str());
      job_conf_advice._job_id = 0;
      job_conf_advice._devices.emplace_back(SHM, 2 * 1024);
      job_conf_advice._devices.emplace_back(PFS, 64 * 1024);
      job_conf_advice._job_time_minutes = 30;
      job_conf_advice._num_cores_per_node = 40;
      job_conf_advice._num_gpus_per_node = 0;
      job_conf_advice._num_nodes = node_names.size();
      job_conf_advice._node_names = node_names;
      job_conf_advice._rpc_port = 8888;
      job_conf_advice._rpc_threads = 8;
      job_conf_advice._priority = 100;
  }
  return job_conf_advice;
}

#endif  // MIMIR_MIMIR_INTERCEPTOR_H
