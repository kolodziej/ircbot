#ifndef _LOG_OUTPUT_HPP
#define _LOG_OUTPUT_HPP

#include <ostream>
#include <string>
#include <mutex>

#include "log_level.hpp"

class LogOutput {
 public:
  LogOutput(std::ostream& output, LogLevel level) :
      m_output{output},
      m_level{level}
  {}
  
  void log(LogLevel level, std::string msg);

 private:
  std::ostream& m_output;
  LogLevel m_level;
  std::mutex m_mtx;
};

#endif
