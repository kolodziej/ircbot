#ifndef _CERR_LOG_OUTPUT_HPP
#define _CERR_LOG_OUTPUT_HPP

#include "ircbot/log_output.hpp"

class CerrLogOutput : public LogOutput {
 public:
  CerrLogOutput(LogLevel level);
  std::ostream& output();
};

#endif

