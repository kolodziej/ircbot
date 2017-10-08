#ifndef _PLUGIN_MANAGER_HPP
#define _PLUGIN_MANAGER_HPP

#include <vector>
#include <memory>

#include "ircbot/plugin.hpp"
#include "ircbot/irc_command.hpp"

class PluginManager {
 public:
  PluginManager() = default;

  void putIncoming(IRCCommand cmd);
  std::vector<IRCCommand> getOutgoing();

  void addPlugin(std::unique_ptr<Plugin>&& plugin);
  void removePlugin(const std::string& name);
  std::vector<std::string> listPlugins() const;

 private:
  std::vector<std::unique_ptr<Plugin>> m_plugins;
};

#endif
