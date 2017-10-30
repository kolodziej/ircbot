#ifndef _LOGGER_HPP
#define _LOGGER_HPP

#include <vector>
#include <sstream>
#include <mutex>
#include <memory>

#include "ircbot/log_level.hpp"
#include "ircbot/log_output.hpp"

#include "ircbot/logger_macros.hpp"

class Logger {
 public:
  using LogOutputPtr = std::unique_ptr<LogOutput>;
  void addOutput(LogOutputPtr&& output);

  template <typename... Args>
  void operator()(LogLevel level, Args... args);

  static Logger& getInstance();
 private:
  Logger() = default;

  template <typename First, typename... Rest>
  void log(First f, Rest... r);

  void log();

 private:
  std::vector<LogOutputPtr> m_outputs;
  std::mutex m_mtx;

  std::stringstream m_stream;

 public:
  static Logger m_logger_instance;
};

#include "ircbot/logger.impl.hpp"

#endif
