#include "ircbot/irc_command.hpp"

#include <sstream>

std::string IRCCommand::toString() const {
  std::stringstream stream;

  if (not nick_servername.empty()) {
    stream << ":" << nick_servername;
    
    if (not user.empty())
      stream << "!" << user;
    if (not host.empty())
      stream << "@" << host;
    
    stream << " ";
  }

  stream << command;
  for (size_t i = 0; i < params.size() - 1; ++i)
    stream << params[i];

  stream << ":" << params.back() << "\r\n";
  return stream.str();
}

IRCCommand::operator std::string() const {
  return toString();
}

std::ostream& operator<<(std::ostream& stream,
                         const IRCCommand& rhs) {
  stream << "{Command: " << rhs.command << "; "
         << "Params: ";

  for (const auto& x : rhs.params)
    stream << "\'" << x << "\' ";

  return stream << "}";
}
