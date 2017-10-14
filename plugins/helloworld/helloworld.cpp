#include "helloworld.hpp"

HelloWorld::HelloWorld() :
    Plugin{"HelloWorld"}
{}

void HelloWorld::putIncoming(IRCCommand cmd) {
  static Logger& logger = Logger::getInstance();
  if (cmd.command == "PRIVMSG") {
    logger(LogLevel::INFO, "HelloWorld plugin: ", cmd.nick);
    m_names.push_back(cmd.nick);
  }
}

IRCCommand HelloWorld::getOutgoing() {
  static Logger& logger = Logger::getInstance();
  auto name = m_names.front();
  m_names.pop_front();
  IRCCommand cmd{
    "PRIVMSG",
    { name, "Hi, it's Hello World plugin :-)" }
  };
  logger(LogLevel::INFO, "HelloWorld plugin sending to ", cmd.user);

  return cmd;
}

size_t HelloWorld::outgoingCount() const {
  return m_names.size();
}
