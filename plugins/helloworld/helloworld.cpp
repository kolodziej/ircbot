#include "helloworld.hpp"

HelloWorld::HelloWorld(PluginManager& manager) :
    Plugin{manager, "HelloWorld"}
{}

void HelloWorld::onMessage(IRCCommand cmd) {
  DEBUG("HelloWorld plugin got incoming message!");

  IRCCommand response{
    "PRIVMSG",
    { cmd.nick, cfg().get("config.message", std::string("HelloWorld plugin")) }
  };

  send(response);
}

bool HelloWorld::filter(const IRCCommand& cmd) {
  return (cmd.command == "PRIVMSG");
}
