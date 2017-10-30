#ifndef _CLOG_LOG_OUTPUT_HPP
#define _CLOG_LOG_OUTPUT_HPP

#include "log_output.hpp"

class ClogLogOutput : public LogOutput {
 public:
  ClogLogOutput(LogLevel level);
  std::ostream& output();
};

#endif
