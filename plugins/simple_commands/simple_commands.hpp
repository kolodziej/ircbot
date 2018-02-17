#ifndef _SIMPLE_COMMANDS_PLUGIN_HPP
#define _SIMPLE_COMMANDS_PLUGIN_HPP

#include "ircbot/so_plugin.hpp"

class SimpleCommands : public SoPlugin {
 public:
  SimpleCommands(Client& client, const std::string& id) :
      SoPlugin{client, id} {
    installCommandParser(
      std::make_shared<CommandParser>(ParserConfig{'!', true})
    );
  }

  std::string getName() const override;
  void onMessage(IRCCommand cmd);

 private:
  void helpCommand(const CommandParser::Command& cmd);
};

IRCBOT_PLUGIN(SimpleCommands)

#endif
