#ifndef _SIMPLE_COMMANDS_PLUGIN_HPP
#define _SIMPLE_COMMANDS_PLUGIN_HPP

#include "ircbot/command_plugin.hpp"

class SimpleCommands : public CommandPlugin {
 public:
  SimpleCommands(Client& client) :
      CommandPlugin{client, '!'} {
    addCommand("help", [this] (const CommandParser::Command& cmd) { helpCommand(cmd); });
  }

  std::string getName() const override;

 private:
  void helpCommand(const CommandParser::Command& cmd);
};

IRCBOT_PLUGIN(SimpleCommands)

#endif
