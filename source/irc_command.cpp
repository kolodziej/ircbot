#include "ircbot/irc_command.hpp"

#include <sstream>


IRCCommand::IRCCommand(const std::string& command,
                       std::initializer_list<std::string> params) :
    command{command},
    params{params} {
}

IRCCommand::operator std::string() const {
  return toString();
}

std::string IRCCommand::toString(bool stripCRLF) const {
  std::stringstream stream;

  if (servername.empty() and not nick.empty()) {
    stream << ':' << nick;
    if (not user.empty())
      stream << '!' << user;

    if (not host.empty())
      stream << '@' << host;

    stream << ' ';
  } else if (not servername.empty()) {
    stream << ':' << servername << ' ';
  }

  stream << command;
  if (not params.empty()) {
    for (size_t i = 0; i < params.size() - 1; ++i)
      stream << ' ' << params[i];

    stream << " :" << params.back();
  }

  if (not stripCRLF)
    stream << "\r\n";

  return stream.str();
}
