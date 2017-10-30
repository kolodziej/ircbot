#include "ircbot/log_output.hpp"

#include <ostream>

LogOutput::LogOutput(LogLevel level) :
  m_level{level} {
}

void LogOutput::log(LogLevel level, std::string message) {
  if (level >= m_level)
    output() << message << std::flush;
}

LogLevel LogOutput::level() const {
  return m_level;
}
