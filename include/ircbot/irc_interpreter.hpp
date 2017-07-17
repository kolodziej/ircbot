#ifndef _IRC_INTERPRETER_HPP
#define _IRC_INTERPRETER_HPP

#include "ircbot/irc_interpreter_result.hpp"

class IRCInterpreter {
 public:
  IRCInterpreter() = default;

  IRCInterpreterResult run(std::string message);
};

#endif
