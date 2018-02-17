#include "helloworld.hpp"

#include <stdexcept>

HelloWorld::HelloWorld(Client& client, const std::string& id) :
    SoPlugin{client, id}
{}

std::string HelloWorld::getName() const {
  return "HelloWorld";
}

void HelloWorld::onMessage(IRCCommand cmd) {
  DEBUG("HelloWorld plugin got incoming message!");

  if (cmd.params[1] == "!hello error") {
    throw std::runtime_error{"Hello World plugin error caused by user!"};
  }

  IRCCommand response{
    "PRIVMSG",
    { cmd.nick, cfg().get("config.message", std::string("HelloWorld plugin")) }
  };

  send(response);
}

bool HelloWorld::filter(const IRCCommand& cmd) {
  return (cmd.command == "PRIVMSG");
}
