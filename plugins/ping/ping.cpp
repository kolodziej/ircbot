#include "ping.hpp"

#include "ircbot/logger.hpp"

using namespace ircbot;

Ping::Ping(PluginConfig config) :
    SoPlugin{config}
{}

std::string Ping::getName() const {
  return "Ping";
}

void Ping::onMessage(IRCMessage cmd) {
  IRCMessage response{
    "PONG",
    { cmd.params.back() }
  };
  send(response);
}

bool Ping::filter(const IRCMessage& cmd) {
  return (cmd.command == "PING");
}
