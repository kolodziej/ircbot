#include "simple_commands.hpp"

void SimpleCommands::helpCommand(const CommandParser::Command& cmd) {
  IRCCommand msg{
    "PRIVMSG",
    { "KolK", "help invoked!" }
  };
  send(msg);
}
