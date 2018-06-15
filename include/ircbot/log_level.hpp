#ifndef _LOG_LEVEL_HPP
#define _LOG_LEVEL_HPP

#include <cstdint>
#include <string>

namespace ircbot {

enum class LogLevel : uint8_t {
  DEBUG = 0,
  INFO,
  WARNING,
  ERROR,
  CRITICAL,
};

const char* LogLevelDesc(LogLevel level);

LogLevel GetLogLevel(const std::string& desc);

} // namespace ircbot

#endif
