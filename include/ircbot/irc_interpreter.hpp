#ifndef _IRC_INTERPRETER_HPP
#define _IRC_INTERPRETER_HPP

#include <deque>

#include "ircbot/irc_interpreter_result.hpp"

class IRCInterpreter {
 public:
  enum class State {
    START,
    PREFIX,
    COMMAND,
    PARAMS,
    TRAILING,
    CR,
    CRLF,
  };

  IRCInterpreter();

  size_t parse(std::string message);

  IRCInterpreterResult nextResult();
  size_t resultsNumber() const;

 private:
  State m_state;

  std::string m_command, m_param;
  std::vector<std::string> m_params;
  std::deque<IRCInterpreterResult> m_results;
};

#endif
