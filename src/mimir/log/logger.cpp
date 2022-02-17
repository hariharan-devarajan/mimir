//
// Created by haridev on 2/14/22.
//

#include <mimir/log/logger.h>

std::shared_ptr<mimir::Logger> mimir::Logger::instance = nullptr;

void mimir::Logger::log(LoggerType type, char* string, ...) {
  va_list args;
  va_start(args, string);
  char buffer[256];
  int result = vsprintf(buffer, string, args);
  fprintf(stdout, "[MIMIR] %s\n", buffer);
  va_end(args);
}
