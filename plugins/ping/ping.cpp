#include "ping.hpp"

#include "ircbot/logger.hpp"

Ping::Ping(PluginManager& manager) :
    Plugin{manager, "Ping"}
{}

void Ping::onMessage(IRCCommand cmd) {
  IRCCommand response{
    "PONG",
    { cmd.params.back() }
  };
  send(response);
}

bool Ping::filter(const IRCCommand& cmd) {
  return (cmd.command == "PING");
}
