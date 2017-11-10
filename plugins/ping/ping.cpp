#include "ping.hpp"

#include "ircbot/logger.hpp"

Ping::Ping(Client& client) :
    Plugin{client, "Ping"}
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
