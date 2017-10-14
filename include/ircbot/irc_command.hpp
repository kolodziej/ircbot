#ifndef _IRC_COMMAND_HPP
#define _IRC_COMMAND_HPP

#include <string>
#include <vector>
#include <ostream>
#include <initializer_list>

struct IRCCommand {
  IRCCommand() = default;
  IRCCommand(const std::string& command,
             std::initializer_list<std::string> params);

  std::string servername;
  std::string user;
  std::string nick;
  std::string host;

  std::string command;
  std::vector<std::string> params;

  operator std::string() const;
  std::string toString(bool stripCRLF = false) const;
};

#endif
