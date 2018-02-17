#ifndef _SIMPLE_COMMANDS_PLUGIN_HPP
#define _SIMPLE_COMMANDS_PLUGIN_HPP

#include "ircbot/so_plugin.hpp"

class SimpleCommands : public SoPlugin {
 public:
  SimpleCommands(PluginConfig config) :
      SoPlugin{config} {
    installCommandParser(
      std::make_shared<CommandParser>(ParserConfig{'!', true})
    );
    addFunction("help", [this] (const CommandParser::Command& cmd) {
      helpCommand(cmd);
    });
  }

  std::string getName() const override;
  void onMessage(IRCCommand cmd);
  bool filter(const IRCCommand& cmd);

 private:
  void helpCommand(const CommandParser::Command& cmd);
};

IRCBOT_PLUGIN(SimpleCommands)

#endif
