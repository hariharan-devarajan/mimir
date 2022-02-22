//
// Created by haridev on 2/14/22.
//

#ifndef MIMIR_LOGGER_H
#define MIMIR_LOGGER_H

#include <cstdarg>
#include <memory>
#include <unordered_map>

namespace mimir {
enum LoggerType { NO_LOG = 0, LOG_ERROR = 1, LOG_WARN = 2, LOG_INFO = 3 };
class Logger {
 private:
  static std::unordered_map<std::string, std::shared_ptr<Logger>> instance_map;
  std::string _app_name;

 public:
  Logger(std::string app_name) : _app_name(app_name) {}
  static std::shared_ptr<Logger> Instance(std::string app_name = "MIMIR") {
    auto iter = instance_map.find(app_name);
    std::shared_ptr<Logger> instance;
    if (iter == instance_map.end()) {
      instance = std::make_shared<Logger>(app_name);
      instance_map.emplace(app_name, instance);
    } else {
      instance = iter->second;
    }
    return instance;
  }
  void log(LoggerType type, char* string, ...);
};
}  // namespace mimir

#endif  // MIMIR_LOGGER_H
