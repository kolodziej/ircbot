#include "ircbot/clog_log_output.hpp"

#include <iostream>

ClogLogOutput::ClogLogOutput(LogLevel level) :
  LogOutput{level} {
}

std::ostream& ClogLogOutput::output() {
  return std::clog;
}
