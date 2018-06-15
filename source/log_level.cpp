#include "ircbot/log_level.hpp"

#include <map>
#include <stdexcept>

namespace ircbot {

const char* LogLevelDesc(LogLevel level) {
  switch (level) {
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARNING:
      return "WARNING";
    case LogLevel::ERROR:
      return "ERROR";
    case LogLevel::CRITICAL:
      return "CRITICAL";
  }

  return "";
}

LogLevel GetLogLevel(const std::string& desc) {
  static std::map<std::string, LogLevel> levels{
    { "DEBUG", LogLevel::DEBUG },
    { "INFO", LogLevel::INFO },
    { "WARNING", LogLevel::WARNING },
    { "ERROR", LogLevel::ERROR },
    { "CRITICAL", LogLevel::CRITICAL }
  };

  if (not levels.count(desc)) {
    throw std::runtime_error{"Could not convert description to LogLevel!"};
  }

  return levels[desc];
}

} // namespace ircbot
