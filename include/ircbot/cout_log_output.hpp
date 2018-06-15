#ifndef _COUT_LOG_OUTPUT_HPP
#define _COUT_LOG_OUTPUT_HPP

#include "ircbot/log_output.hpp"

namespace ircbot {

/** \class CoutLogOutput
 *
 * \brief Representation of std::cout
 *
 * Output for Logger. Prints everything to std::cout.
 */

class CoutLogOutput : public LogOutput {
 public:
  /** Default constructor
   *
   * \param level minimum log level that will be logged to this output
   */
  CoutLogOutput(LogLevel level);

  /** Returns reference to output stream (std::ostream) */
  std::ostream& output();
};

}  // namespace ircbot

#endif
