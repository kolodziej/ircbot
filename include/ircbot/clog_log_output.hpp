#ifndef _CLOG_LOG_OUTPUT_HPP
#define _CLOG_LOG_OUTPUT_HPP

#include "log_output.hpp"

namespace ircbot {

/** \class ClogLogOutput
 *
 * \brief Representation of std::clog
 *
 * Output for Logger. Prints everything to std::clog.
 */

class ClogLogOutput : public LogOutput {
 public:
  /** Default constructor
   *
   * \param level minimum log level that will be logged to this output
   */
  ClogLogOutput(LogLevel level);

  /** Returns reference to output stream (std::ostream) */
  std::ostream& output();
};

} // namespace ircbot

#endif
