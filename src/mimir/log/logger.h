//
// Created by haridev on 2/14/22.
//

#ifndef MIMIR_LOGGER_H
#define MIMIR_LOGGER_H

#include <cstdarg>
#include <memory>

namespace mimir {
enum LoggerType { NO_LOG = 0, LOG_ERROR = 1, LOG_WARN = 2, LOG_INFO = 3 };
class Logger {
 private:
  static std::shared_ptr<Logger> instance;

 public:
  static std::shared_ptr<Logger> Instance() {
    if (instance == nullptr) {
      instance = std::make_shared<Logger>();
    }
    return instance;
  }
  void log(LoggerType type, char* string, ...);
};
}  // namespace mimir

#endif  // MIMIR_LOGGER_H
