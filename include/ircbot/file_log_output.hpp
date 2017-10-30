#ifndef _FILE_LOG_OUTPUT_HPP
#define _FILE_LOG_OUTPUT_HPP

#include "ircbot/log_output.hpp"

#include <fstream>

class FileLogOutput : public LogOutput {
 public:
  FileLogOutput(const std::string& fname, LogLevel level);
  FileLogOutput(std::ofstream&& file, LogLevel level);

  std::ostream& output();

 private:
  std::ofstream m_file;
};

#endif
