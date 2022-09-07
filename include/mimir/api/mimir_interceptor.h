//
// Created by hariharan on 2/23/22.
//

#ifndef MIMIR_MIMIR_INTERCEPTOR_H
#define MIMIR_MIMIR_INTERCEPTOR_H
#include <mimir/advice/config.h>
#include <mimir/common/error_code.h>
#include <mimir/macro.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <experimental/filesystem>
#include <fstream>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

namespace fs = std::experimental::filesystem;

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
    MIMIR_LOGINFO("Destructing Tracker","");
  }
  Tracker()
      : _track_fd(), _track_files(), _exclude_files(), local(0), remote(0) {
    MIMIR_LOGINFO("Constructing Tracker","");
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
        MIMIR_LOGINFO("Tracking file descriptor %d", fd);
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
        MIMIR_LOGINFO("Tracking file %s", path.c_str());
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
        MIMIR_LOGINFO("Excluding file %s", path.c_str());
        return true;
      }
    }
    return false;
  }
};
}  // namespace mimir

extern mimir::Tracker* MIMIR_TRACKER();
extern mimir::Config* MIMIR_CONFIG();
extern MimirStatus mimir_init_config(bool is_mpi = false);
extern MimirStatus mimir_finalize_config();

extern MimirStatus insert_loaded_intents();
extern MimirStatus remove_loaded_intents();

inline mimir::JobConfigurationAdvice load_job_details() {
  assert(mimir_init_config() == 0);
  return MIMIR_CONFIG()->_job_config;
}

#endif  // MIMIR_MIMIR_INTERCEPTOR_H
