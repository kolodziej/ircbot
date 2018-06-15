#include "ircbot/logger.hpp"

namespace ircbot {

Logger& Logger::getInstance() {
  return m_logger_instance;
}

void Logger::addOutput(LogOutputPtr&& output) {
  m_outputs.push_back(std::move(output));
}

void Logger::log() {
  m_stream << "\n";
}
  
Logger Logger::m_logger_instance;

} // namespace ircbot
