#ifndef _LOGGER_HPP
#define _LOGGER_HPP

#include <vector>
#include <sstream>
#include <mutex>

#include "log_level.hpp"
#include "log_output.hpp"

class Logger {
 public:
  void addOutput(const LogOutput& output);

  template <typename... Args>
  void operator()(LogLevel level, Args... args);

 private:
  template <typename First, typename... Rest>
  void log(First f, Rest... r);

  void log();

 private:
  std::vector<LogOutput> m_outputs;
  std::mutex m_mtx;

  std::stringstream m_stream;
};

#include "ircbot/logger.impl.hpp"

#endif
