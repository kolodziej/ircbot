#include "ping.hpp"

#include "ircbot/logger.hpp"

Ping::Ping(Client& client, const std::string& id) :
    SoPlugin{client, id}
{}

std::string Ping::getName() const {
  return "Ping";
}

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
