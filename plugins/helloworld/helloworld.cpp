#include "helloworld.hpp"

HelloWorld::HelloWorld(PluginManager& manager) :
    Plugin{manager, "HelloWorld"}
{}

void HelloWorld::run() {
  Logger& logger = Logger::getInstance();

  logger(LogLevel::DEBUG, "HelloWorld plugin is trying to get incoming message...");
  auto cmd = getCommand();
  logger(LogLevel::DEBUG, "HelloWorld plugin got incoming message!");

  IRCCommand response{
    "PRIVMSG",
    { cmd.nick, "Hi, it's HelloWorld plugin :-)" }
  };

  send(response);
}

bool HelloWorld::filter(const IRCCommand& cmd) {
  return (cmd.command == "PRIVMSG");
}
