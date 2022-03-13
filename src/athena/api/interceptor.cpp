//
// Created by hariharan on 2/21/22.
//

#include <athena/api/interceptor.h>
#include "mimir/common/data_structure.h"
bool IsTracked(std::string path, int fd) {
  if (!is_tracing()) return false;
  if (path.empty()) {
      if (fd != -1) {
          auto is_traced_fd = MIMIR_TRACKER()->is_traced(fd);
          if (is_traced_fd) return true;
      }
  } else {
      auto is_excluded = MIMIR_TRACKER()->is_excluded(path);
      if (is_excluded) return false;
      auto is_traced = MIMIR_TRACKER()->is_traced(path);
      if (is_traced) return true;
  }
  return false;
}
