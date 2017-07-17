#ifndef _IRC_INTERPRETER_RESULT_HPP
#define _IRC_INTERPRETER_RESULT_HPP

#include <string>
#include <vector>

struct IRCInterpreterResult {
  std::string command;
  std::vector<std::string> params;
};

#endif
