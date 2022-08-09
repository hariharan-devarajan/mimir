//
// Created by hariharan on 2/23/22.
//

#ifndef MIMIR_MIMIR_INTERCEPTOR_H
#define MIMIR_MIMIR_INTERCEPTOR_H
#include <mimir/log/logger.h>
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

#include "job_configuration.h"

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
            mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                                           "Destructing Tracker");
        }
        Tracker(): _track_fd(), _track_files(), _exclude_files(), local(0), remote(0){
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
                                            "Loading Default job configuration");
      job_conf_advice._job_id = 0;
      job_conf_advice._devices.emplace_back(SHM, 2 * 1024);
      job_conf_advice._devices.emplace_back(PFS, 64 * 1024);
      job_conf_advice._job_time_minutes = 30;
      job_conf_advice._num_cores_per_node = 40;
      job_conf_advice._num_gpus_per_node = 0;
      job_conf_advice._num_nodes = node_names.size();
      job_conf_advice._node_names = node_names;
      job_conf_advice._rpc_port = 8888;
      job_conf_advice._rpc_threads = 1;
      job_conf_advice._priority = 100;
  }
  return job_conf_advice;
}

#endif  // MIMIR_MIMIR_INTERCEPTOR_H
