#include "ircbot/irc_message.hpp"

#include <sstream>

namespace ircbot {

IRCMessage::IRCMessage(const std::string& command,
                       std::initializer_list<std::string> params) :
    command{command},
    params{params} {
}

IRCMessage::operator std::string() const {
  return toString();
}

std::string IRCMessage::toString(bool stripCRLF) const {
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

IRCMessage IRCMessage::fromProtobuf(const PluginProtocol::IrcMessage& pb_msg) {
  IRCMessage msg;
  msg.servername = pb_msg.servername();
  msg.user = pb_msg.user();
  msg.nick = pb_msg.nick();
  msg.host = pb_msg.host();
  msg.command = pb_msg.command();
  for (int i = 0; i < pb_msg.params_size(); ++i) {
    msg.params.push_back(pb_msg.params(i));
  }

  return msg;
}

} // namespace ircbot
