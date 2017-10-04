#include "ircbot/log_output.hpp"

#include <ostream>

void LogOutput::log(LogLevel level, std::string message) {
  if (level >= m_level)
    m_output << message << std::flush;
}
