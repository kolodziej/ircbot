#ifndef _IRC_COMMAND_HPP
#define _IRC_COMMAND_HPP

#include <string>
#include <vector>
#include <ostream>

struct IRCCommand {
  std::string servername;
  std::string user;
  std::string nick;
  std::string host;

  std::string command;
  std::vector<std::string> params;

  operator std::string() const;
  std::string toString() const;
};

#endif
