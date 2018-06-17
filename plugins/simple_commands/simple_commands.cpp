#include "simple_commands.hpp"

using namespace ircbot;

std::string SimpleCommands::getName() const { return "SimpleCommands"; }

void SimpleCommands::helpCommand(const CommandParser::Command& cmd) {
  IRCMessage msg{"PRIVMSG", {"KolK", "help invoked!"}};
  send(msg);

  if (cmd.arguments.size()) {
    msg.params = {"KolK", "with arguments: "};
    send(msg);
    for (const auto& x : cmd.arguments) {
      msg.params = {"KolK", x};
      send(msg);
    }
    msg.params = {"KolK", cmd.irc_message.toString()};
    send(msg);
  }
}
