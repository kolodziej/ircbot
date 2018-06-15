#include "ircbot/cout_log_output.hpp"

#include <iostream>

namespace ircbot {

CoutLogOutput::CoutLogOutput(LogLevel level) :
  LogOutput{level}
{}

std::ostream& CoutLogOutput::output() {
  return std::cout;
}

} // namespace ircbot
