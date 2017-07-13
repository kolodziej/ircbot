#include "ircbot/logger.hpp"

void Logger::addOutput(const LogOutput& output) {
  m_outputs.push_back(output);
}

void Logger::log(std::stringstream& stream) {
  stream << std::endl;
}
  
