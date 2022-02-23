//
// Created by haridev on 2/14/22.
//

#include <mimir/log/logger.h>

std::unordered_map<std::string, std::shared_ptr<mimir::Logger>>
    mimir::Logger::instance_map =
        std::unordered_map<std::string, std::shared_ptr<mimir::Logger>>();

void mimir::Logger::log(LoggerType type, char* string, ...) {
  va_list args;
  va_start(args, string);
  char buffer[256];
  int result = vsprintf(buffer, string, args);
  switch (type) {
    case LoggerType::LOG_INFO: {
      if (level >= LoggerType::LOG_INFO)
        fprintf(stdout, "[%s INFO]: %s\n", _app_name.c_str(), buffer);
      break;
    }
    case LoggerType::LOG_WARN: {
      if (level >= LoggerType::LOG_WARN)
        fprintf(stdout, "[%s WARN]: %s\n", _app_name.c_str(), buffer);
      break;
    }
    case LoggerType::LOG_ERROR: {
      if (level >= LoggerType::LOG_ERROR)
        fprintf(stderr, "[%s ERROR]: %s\n", _app_name.c_str(), buffer);
      break;
    }
  }

  va_end(args);
}
