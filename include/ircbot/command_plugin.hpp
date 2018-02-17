#ifndef _COMMAND_PLUGIN_HPP
#define _COMMAND_PLUGIN_HPP

#include "ircbot/so_plugin.hpp"

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#include "ircbot/client.hpp"
#include "ircbot/command_parser.hpp"

class CommandPlugin : public SoPlugin {
 public:
  using CmdFunction = std::function<void(const CommandParser::Command&)>;
  CommandPlugin(Client& client, const std::string& id, char prefix);

  virtual void onMessage(IRCCommand cmd) override;
  virtual bool filter(const IRCCommand& cmd) override;
  virtual bool isCommand(const IRCCommand& cmd);

 protected:
  void addCommand(const std::string& cmd, CmdFunction);
  void removeCommand(const std::string& cmd);
  void callCommand(const CommandParser::Command& command);
  void runParsedCommands();

  CommandParser m_parser;
  std::unordered_map<std::string, CmdFunction> m_functions;
};

#endif
