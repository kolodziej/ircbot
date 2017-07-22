#ifndef _IRC_INTERPRETER_HPP
#define _IRC_INTERPRETER_HPP

#include <deque>

#include "ircbot/irc_command.hpp"

class IRCInterpreter {
 public:
  enum class State {
    START,
    NICK_SERVERNAME,
    USER,
    HOST,
    COMMAND,
    PARAMS,
    TRAILING,
    CR,
    CRLF,
  };

  IRCInterpreter();

  size_t parse(std::string message);

  IRCCommand nextCommand();
  size_t commandsNumber() const;

 private:
  State m_state;

  std::string m_nick_servername, m_user, m_host;
  std::string m_command, m_param;
  std::vector<std::string> m_params;
  std::deque<IRCCommand> m_commands;
};

#endif
