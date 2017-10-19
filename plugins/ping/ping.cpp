#include "ping.hpp"

#include "ircbot/logger.hpp"

Ping::Ping(PluginManager& manager) :
    Plugin{manager, "Ping"}
{}

void Ping::run() {
  auto cmd = getCommand();
  IRCCommand response{
    "PONG",
    { cmd.params.back() }
  };
  send(response);
}

bool Ping::filter(const IRCCommand& cmd) {
  return (cmd.command == "PING");
}
