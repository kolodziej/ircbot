#include "helloworld.hpp"

#include <stdexcept>

using namespace ircbot;

HelloWorld::HelloWorld(PluginConfig config) :
    SoPlugin{config}
{}

std::string HelloWorld::getName() const {
  return "HelloWorld";
}

void HelloWorld::onMessage(IRCMessage cmd) {
  DEBUG("HelloWorld plugin got incoming message!");

  if (cmd.params[1] == "!hello error") {
    throw std::runtime_error{"Hello World plugin error caused by user!"};
  }

  IRCMessage response{
    "PRIVMSG",
    { cmd.nick, cfg().get("config.message", std::string("HelloWorld plugin")) }
  };

  send(response);
}

bool HelloWorld::filter(const IRCMessage& cmd) {
  return (cmd.command == "PRIVMSG");
}
