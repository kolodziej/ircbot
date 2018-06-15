#ifndef _SIMPLE_COMMANDS_PLUGIN_HPP
#define _SIMPLE_COMMANDS_PLUGIN_HPP

#include "ircbot/so_plugin.hpp"

class SimpleCommands : public ircbot::SoPlugin {
 public:
  SimpleCommands(ircbot::PluginConfig config) :
      ircbot::SoPlugin{config} {
    installCommandParser(
      std::make_shared<ircbot::CommandParser>(ircbot::ParserConfig{'!', true})
    );
    addFunction("help", [this] (const ircbot::CommandParser::Command& cmd) {
      helpCommand(cmd);
    });
  }

  std::string getName() const override;

 private:
  void helpCommand(const ircbot::CommandParser::Command& cmd);
};

IRCBOT_PLUGIN(SimpleCommands)

#endif
