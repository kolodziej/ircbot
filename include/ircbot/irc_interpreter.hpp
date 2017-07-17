#ifndef _IRC_INTERPRETER_HPP
#define _IRC_INTERPRETER_HPP

#include "ircbot/irc_interpreter_result.hpp"

class IRCInterpreter {
 public:
  IRCInterpreter() = default;

  IRCInterpreterResult parse(std::string message);
};

#endif
