#include "ircbot/cerr_log_output.hpp"

#include <iostream>

namespace ircbot {

CerrLogOutput::CerrLogOutput(LogLevel level) :
  LogOutput{level} {
}

std::ostream& CerrLogOutput::output() {
  return std::cerr;
}

} // namespace ircbot
