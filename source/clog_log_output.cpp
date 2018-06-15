#include "ircbot/clog_log_output.hpp"

#include <iostream>

namespace ircbot {

ClogLogOutput::ClogLogOutput(LogLevel level) : LogOutput{level} {}

std::ostream& ClogLogOutput::output() { return std::clog; }

}  // namespace ircbot
