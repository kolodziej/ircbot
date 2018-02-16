#include "simple_commands.hpp"

void SimpleCommands::helpCommand(const CommandParser::Command& cmd) {
  IRCCommand msg{
    "PRIVMSG",
    { "KolK", "help invoked!" }
  };
  send(msg);

  if (cmd.arguments.size()) {
    msg.params = { "KolK", "with arguments: " };
    send(msg);
    for (const auto& x : cmd.arguments) {
      msg.params = { "KolK", x };
      send(msg);
    }
  }
}