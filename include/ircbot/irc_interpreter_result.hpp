#ifndef _IRC_INTERPRETER_RESULT_HPP
#define _IRC_INTERPRETER_RESULT_HPP

#include <string>
#include <vector>
#include <ostream>

struct IRCInterpreterResult {
  std::string command;
  std::vector<std::string> params;
};

std::ostream& operator<<(std::ostream& stream,
                         const IRCInterpreterResult& rhs);

#endif
