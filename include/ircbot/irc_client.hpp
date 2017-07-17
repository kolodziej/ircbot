#ifndef _IRC_CLIENT_HPP
#define _IRC_CLIENT_HPP

#include "ircbot/irc_interpreter_result.hpp"

class IRCClient {
 public:
  IRCClient() = default;

  std::string initialize(std::string, std::string, std::string);
  std::string getResponse(const IRCInterpreterResult&);

 private:
  std::string ping(const IRCInterpreterResult&);
};

#endif
