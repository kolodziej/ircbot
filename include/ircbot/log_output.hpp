#ifndef _LOG_OUTPUT_HPP
#define _LOG_OUTPUT_HPP

#include <ostream>
#include <string>

#include "log_level.hpp"

class LogOutput {
 public:
  LogOutput(LogLevel level);
  
  void log(LogLevel level, std::string msg);
  LogLevel level() const;

  virtual std::ostream& output() = 0;

 private:
  LogLevel m_level;
};

#endif
