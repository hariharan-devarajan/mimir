//
// Created by haridev on 9/6/22.
//

#ifndef MIMIR_MACRO_H
#define MIMIR_MACRO_H
#include <cpp-logger/logger.h>
#define MIMIR_LOGGER cpplogger::Logger::Instance("MIMIR")
#define MIMIR_LOGINFO(format, ...) \
  MIMIR_LOGGER->log(cpplogger::LOG_INFO, format, __VA_ARGS__);
#define MIMIR_LOGWARN(format, ...) \
  MIMIR_LOGGER->log(cpplogger::LOG_WARN, format, __VA_ARGS__);
#define MIMIR_LOGERROR(format, ...) \
  MIMIR_LOGGER->log(cpplogger::LOG_ERROR, format, __VA_ARGS__);
#define MIMIR_LOGPRINT(format, ...) \
  MIMIR_LOGGER->log(cpplogger::LOG_PRINT, format, __VA_ARGS__);
#endif //MIMIR_MACRO_H
