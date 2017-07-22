#ifndef _IRC_COMMAND_HPP
#define _IRC_COMMAND_HPP

#include <string>
#include <vector>
#include <ostream>

struct IRCCommand {
  std::string nick_servername, user, host;
  std::string command;
  std::vector<std::string> params;

  IRCCommand() = default;
  IRCCommand(std::string command, std::vector<std::string> params,
      std::string nick_servername, std::string user = "", std::string host = "") :
      nick_servername{nick_servername},
      user{user},
      host{host},
      command{command},
      params(params)
  {}

  std::string toString() const;
  operator std::string() const;
};

std::ostream& operator<<(std::ostream& stream,
                         const IRCCommand& rhs);

#endif
