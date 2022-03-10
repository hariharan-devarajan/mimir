//
// Created by hariharan on 2/21/22.
//

#include <athena/api/interceptor.h>
#include "mimir/common/data_structure.h"

extern const char* kPathExclusions[22] = {"/bin/",
                                          "/boot/",
                                          "/dev/",
                                          "/etc/",
                                          "/lib/",
                                          "/opt/",
                                          "/proc/",
                                          "/sbin/",
                                          "/sys/",
                                          "/tmp",
                                          "/usr/",
                                          "/var/",
                                          "/run/",
                                          "/lib64",
                                          "pipe",
                                          "/tmp/ompi",
                                          "socket:",
                                          "anon_inode:",
                                          "merge_whole-wf.in",
                                          "pegasus",
                                          "mimir",
                                          "/g/g92/haridev/.lsbatch/"};

extern const char* kExtensionExclusions[3] = {"conf", "out", "in"};
extern std::unordered_set<int> track_files = std::unordered_set<int>();
extern std::unordered_set<std::string> untrack_files =
    std::unordered_set<std::string>();

namespace athena {
bool exit = false;
}  // namespace athena
void OnExit(void) { athena::exit = true; }

bool IsTracked(std::string path, int fd) {
  if (athena::exit) return false;
  if (path == "/" || path.find("socket:") == 0) {
    return false;
  }
  if (fd != -1 && !track_files.empty()) {
    auto iter = track_files.find(fd);
    if (iter != track_files.end()) {
      mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                             "Tracking file %d", fd);
      return true;
    }
  }
  if (!untrack_files.empty()) {
    auto iter = untrack_files.find(path);
    if (iter != untrack_files.end()) {
      mimir::Logger::Instance("ATHENA")->log(
          mimir::LOG_INFO, "Untracking file %s", path.c_str());
      return false;
    }
  }
  for (const auto& pth : kPathExclusions) {
    if (path.find(pth) == 0) {
      return false;
    }
  }
  for (const auto& extension : kExtensionExclusions) {
    if (path.substr(path.find_last_of('.') + 1) == extension) {
      return false;
    }
  }
  /*mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO, "Tracking file %s",
                                         path.c_str());*/
  return true;
}
