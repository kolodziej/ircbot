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

std::string Logger::filterString(std::string s) {
  for (char& c : s) {
    switch (c) {
      case '\n':
      case '\r':
        c = '\\';
        break;
    }
  }

  return s;
}
  
Logger Logger::m_logger_instance;
