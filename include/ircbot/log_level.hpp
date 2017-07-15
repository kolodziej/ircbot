#ifndef _LOG_LEVEL_HPP
#define _LOG_LEVEL_HPP

#include <cstdint>

enum class LogLevel : uint8_t {
  DEBUG = 0,
  INFO,
  WARNING,
  ERROR,
  CRITICAL,
};

const char* LogLevelDesc(LogLevel level);

#endif
