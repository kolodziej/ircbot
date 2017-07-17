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

  static Logger& getInstance();

 private:
  Logger() = default;

  template <typename First, typename... Rest>
  void log(First f, Rest... r);

  template <typename... Rest>
  void log(std::string s, Rest... r);

  template <typename T, typename... Rest>
  void log(std::vector<T> s, Rest... r);

  void log();

  std::string filterString(std::string);

 private:
  std::vector<LogOutput> m_outputs;
  std::mutex m_mtx;

  std::stringstream m_stream;

 private:
  static Logger m_logger_instance;
};

#include "ircbot/logger.impl.hpp"

#endif
