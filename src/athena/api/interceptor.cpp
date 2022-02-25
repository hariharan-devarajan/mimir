//
// Created by hariharan on 2/21/22.
//

#include <athena/api/interceptor.h>
#include "mimir/common/data_structure.h"

extern const char* kPathExclusions[15] = {
    "/bin/", "/boot/", "/dev/",  "/etc/",   "/lib/",
    "/opt/", "/proc/", "/sbin/", "/sys/",   "/usr/",
    "/var/", "/run/",  "pipe",   "socket:", "anon_inode:"};

extern const char* kExtensionExclusions[1] = {"conf"};
extern std::vector<std::string> track_files = std::vector<std::string>();

namespace athena {
bool exit = false;
}  // namespace athena
void OnExit(void) { athena::exit = true; }

bool IsTracked(std::string path) {
  if (athena::exit) return false;
  if (path == "/") {
    return false;
  }
  if (path.find("socket:") == 0) {
    return false;
  }
  //  for (const auto& pth : track_files) {
  //    if (path.find(pth) == 0) {
  //      /*mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
  //                                             "Tracking file %s",
  //         path.c_str());*/
  //      return true;
  //    }
  //  }

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
