#include "ircbot/logger.hpp"

namespace ircbot {

Logger& Logger::getInstance() { return m_logger_instance; }

size_t Logger::outputsCount() const { return m_outputs.size(); }

void Logger::addOutput(LogOutputPtr&& output) {
  m_outputs.push_back(std::move(output));
}

void Logger::clearOutputs() { m_outputs.clear(); }

void Logger::log() { m_stream << "\n"; }

Logger Logger::m_logger_instance;

}  // namespace ircbot
