#ifndef _LOGGER_HPP
#define _LOGGER_HPP

#include <memory>
#include <mutex>
#include <sstream>
#include <vector>

#include "ircbot/log_level.hpp"
#include "ircbot/log_output.hpp"

#include "ircbot/logger_macros.hpp"

namespace ircbot {

/** \class Logger
 *
 * \brief Logging utility used in IRCBot library
 *
 * This class provides an interface to log messages to files, standard outputs
 * and other std::ostream instances. This class implements Singleton pattern.
 */

class Logger {
 public:
  /** std::unique_ptr to LogOutput */
  using LogOutputPtr = std::unique_ptr<LogOutput>;

  /** Add output to logger instance
   *
   * \param output rvalue reference to unique pointer to LogOutput instance
   */
  void addOutput(LogOutputPtr&& output);

  /** Get number of outputs */
  size_t outputsCount() const;

  /** Clear outputs
   *
   * Remove all outputs from Logger
   */
  void clearOutputs();

  /** Template for logging operator
   *
   * \param level level of logged output
   * \param args data that should be printed to output
   */
  template <typename... Args>
  void operator()(LogLevel level, Args... args);

  /** Get reference to the only existing instance of Logger */
  static Logger& getInstance();

 private:
  Logger() = default;

  /** Helper for logger */
  template <typename First, typename... Rest>
  void log(First f, Rest... r);

  /** Base case - no log arguments */
  void log();

 private:
  /** vector of pointers to log outputs */
  std::vector<LogOutputPtr> m_outputs;

  /** mutex providing synchronization for access to output streams */
  std::mutex m_mtx;

  /** Temporary string stream for collecting whole log message before printing
   * it to output
   */
  std::stringstream m_stream;

 public:
  /** Single instance of Logger */
  static Logger m_logger_instance;
};

}  // namespace ircbot

#include "ircbot/logger.impl.hpp"

#endif
