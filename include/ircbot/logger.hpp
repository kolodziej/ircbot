#ifndef _LOGGER_HPP
#define _LOGGER_HPP

#include <vector>
#include <sstream>
#include <mutex>

#include "log_output.hpp"

class Logger {
 public:
  void addOutput(const LogOutput& output);

  template <typename... Args>
  void operator()(Args... args);

 private:
  template <typename First, typename... Rest>
  void log(std::stringstream& stream, First f, Rest... r);

  void log(std::stringstream& stream);

 private:
  std::vector<LogOutput> m_outputs;
  std::mutex m_mtx;
};

#endif
