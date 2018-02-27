#ifndef _LOG_OUTPUT_HPP
#define _LOG_OUTPUT_HPP

#include <ostream>
#include <string>

#include "log_level.hpp"

/** \class LogOutput
 *
 * \brief Base abstract class for all logger outputs
 */

class LogOutput {
 public:
  /** Default constructor
   *
   * \param level minimum log level that will be logged to this output
   */
  LogOutput(LogLevel level);
  
  /** Logging function
   *
   * \param level log level for given message
   * \param msg message
   */
  void log(LogLevel level, std::string msg);

  /** Get minimum log level for this output */
  LogLevel level() const;

  /** Pure virtual function returning reference to output stream */
  virtual std::ostream& output() = 0;

 private:
  /** minmum log level */
  LogLevel m_level;
};

#endif
