#include "ircbot/logger.hpp"

Logger& Logger::getInstance() {
  return m_logger_instance;
}

void Logger::addOutput(const LogOutput& output) {
  m_outputs.push_back(output);
}

void Logger::log() {
  m_stream << "\n";
}
  
Logger Logger::m_logger_instance;
