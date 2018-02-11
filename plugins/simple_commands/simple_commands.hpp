#ifndef _SIMPLE_COMMANDS_PLUGIN_HPP
#define _SIMPLE_COMMANDS_PLUGIN_HPP

#include "ircbot/command_plugin.hpp"

class SimpleCommands : public CommandPlugin {
 public:
  SimpleCommands(Client& client) :
      CommandPlugin{client, "SimpleCommands", '!'} {
    addCommand("help", [this] (const CommandParser::Command& cmd) { helpCommand(cmd); });
  }

 private:
  void helpCommand(const CommandParser::Command& cmd);
};

IRCBOT_PLUGIN(SimpleCommands)

#endif
