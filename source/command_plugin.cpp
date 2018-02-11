#include "ircbot/command_plugin.hpp"

CommandPlugin::CommandPlugin(Client& client,
                             std::string name,
                             char prefix) :
    Plugin{client, name},
    m_parser{ParserConfig{prefix, true}} {
}

void CommandPlugin::onMessage(IRCCommand cmd) {
  m_parser.parse(cmd.params.back());
  if (m_parser.commandsCount() > 1) {
    LOG(WARNING, "Too many commands parsed: ", m_parser.commandsCount());
  }

  CommandParser::Command cmd = m_parser.getCommand();
  cmd.additional_arguments = {
    cmd.servername,
    cmd.user,
    cmd.nick,
    cmd.host
  };
  cmd.additional_arguments.insert(
      cmd.additional_arguments.end(),
      cmd.params.begin(),
      cmd.params.end() - 1
  );

  callCommand(cmd);
}

bool CommandPlugin::filter(const IRCCommand& cmd) {
  return (cmd.command == "PRIVMSG" and isCommand(cmd));
}

bool CommandPlugin::isCommand(const IRCCommand& cmd) {
  if (cmd.params.size())
    return cmd.params.last()[0] == m_parser.getConfig().prefix;

  return false;
}

void CommandPlugin::addCommand(const std::string& cmd, CmdFunction f) {
  if (m_functions.count(cmd)) {
    LOG(WARNING, "Overwriting function for command: ", cmd);
  }

  m_functions[cmd] = f;
}

void CommandPlugin::removeCommand(const std::string& cmd) {
  size_t removed = m_functions.erase(cmd);
  if (removed == 0) {
    LOG(WARNING, "Such command (", cmd, ") didn't exist.");
  }
}

void CommandPlugin::callCommand(const CommandParser::Command& command) {
  if (m_functions.count(command.command)) {
    m_functions[command.command](command);
  } else {
    LOG(ERROR, "Function for such command doesn't exist: ", command.command);
  }
}