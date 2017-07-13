#ifndef _LOG_LEVEL_HPP
#define _LOG_LEVEL_HPP

enum class LogLevel : uint8_t {
  DEBUG = 0,
  INFO,
  WARNING,
  ERROR,
  CRITICAL,
};

#define LogLevelDesc(x) #x

#endif
