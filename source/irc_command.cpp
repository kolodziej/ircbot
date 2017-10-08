#include "ircbot/irc_command.hpp"

#include <sstream>

IRCCommand::operator std::string() const {
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

  stream << "\r\n";
  return stream.str();
}

std::string IRCCommand::toString() const {
  return static_cast<std::string>(*this);
}
