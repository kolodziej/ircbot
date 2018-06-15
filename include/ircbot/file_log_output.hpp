#ifndef _FILE_LOG_OUTPUT_HPP
#define _FILE_LOG_OUTPUT_HPP

#include "ircbot/log_output.hpp"

#include <fstream>

namespace ircbot {

/** \class FileLogOutput
 *
 * \brief File stream output for logger
 *
 * This class provides file stream output for logger messages.
 */

class FileLogOutput : public LogOutput {
 public:
  /** Constructor
   *
   * \param fname path to log file
   * \param level minimum log level that will be logged to this output
   */
  FileLogOutput(const std::string& fname, LogLevel level);

  /** Constructor - handles opened file
   *
   * \param file rvalue reference to output file stream; log output becomes
   * owner of this object
   * \param level minimum log level that will be logged to this output
   */
  FileLogOutput(std::ofstream&& file, LogLevel level);

  /** Return reference to output stream */
  std::ostream& output();

 private:
  /** Log file */
  std::ofstream m_file;
};

}  // namespace ircbot

#endif
