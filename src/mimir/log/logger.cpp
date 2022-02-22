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
  fprintf(stdout, "[%s] %s\n", _app_name.c_str(), buffer);
  va_end(args);
}
