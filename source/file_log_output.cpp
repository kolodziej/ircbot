#include "ircbot/file_log_output.hpp"

#include <stdexcept>

namespace ircbot {

FileLogOutput::FileLogOutput(const std::string& fname, LogLevel level) :
  LogOutput{level},
  m_file{fname} {
  if (not m_file.is_open()) {
    throw std::runtime_error{std::string("Could not open file: ") + fname};
  }
}

FileLogOutput::FileLogOutput(std::ofstream&& file, LogLevel level) :
  LogOutput{level},
  m_file(std::move(file)) {
}

std::ostream& FileLogOutput::output() {
  return m_file;
}

} // namespace ircbot
