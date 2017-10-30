#ifndef _CLOG_LOG_OUTPUT_HPP
#define _CLOG_LOG_OUTPUT_HPP

#include "ircbot/log_output.hpp"

class CoutLogOutput : public LogOutput {
 public:
  CoutLogOutput(LogLevel level);
  std::ostream& output();
};

#endif
