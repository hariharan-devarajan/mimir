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


bool IsTracked(std::string path, int fd) {
  if (!is_tracing()) return false;
  if (path == "/" || path.find("socket:") == 0) {
    return false;
  }
    auto is_remote = MIMIR_TRACKER()->is_remote(path);
    if (is_remote) return false;
    auto is_remote_fd = MIMIR_TRACKER()->is_remote(fd);
    if (is_remote_fd) return false;
  auto is_excluded = MIMIR_TRACKER()->is_excluded(path);
  if (is_excluded) return false;
  auto is_traced = MIMIR_TRACKER()->is_traced(path);
  if (is_traced) return true;
  auto is_traced_fd = MIMIR_TRACKER()->is_traced(fd);
  if (is_traced_fd) return true;
  return false;
}
