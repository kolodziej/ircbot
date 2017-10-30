#include "ircbot/cout_log_output.hpp"

#include <iostream>

CoutLogOutput::CoutLogOutput(LogLevel level) :
  LogOutput{level}
{}

std::ostream& CoutLogOutput::output() {
  return std::cout;
}
