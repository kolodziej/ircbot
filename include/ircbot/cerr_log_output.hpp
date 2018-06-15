#ifndef _CERR_LOG_OUTPUT_HPP
#define _CERR_LOG_OUTPUT_HPP

#include "ircbot/log_output.hpp"

namespace ircbot {


/** \class CerrLogOutput
 *
 * \brief Representation of std::cerr 
 *
 * Output for Logger. Prints everything to std::cerr.
 */

class CerrLogOutput : public LogOutput {
 public:
  /** Default constructor 
   *
   * \param level minimum log level that will be logged to this output
   */
  CerrLogOutput(LogLevel level);

  /** Returns reference to output stream (std::ostream) */
  std::ostream& output();
};

} // namespace ircbot

#endif

