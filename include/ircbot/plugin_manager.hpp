#ifndef _PLUGIN_MANAGER_HPP
#define _PLUGIN_MANAGER_HPP

#include <vector>
#include <deque>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "ircbot/irc_command.hpp"
#include "ircbot/config.hpp"

class Plugin;

class PluginManager {
 public:
  PluginManager() = default;

  void initializePlugins(Config& cfg);

  void putIncoming(IRCCommand cmd);
  IRCCommand getOutgoing();

  void addOutgoing(IRCCommand cmd);

  void addPlugin(std::unique_ptr<Plugin>&& plugin);
  void removePlugin(const std::string& name);
  std::vector<std::string> listPlugins() const;
  std::unique_ptr<Plugin> loadSoPlugin(const std::string& fname);

  void startPlugins();

 private:
  std::vector<std::unique_ptr<Plugin>> m_plugins;
  std::mutex m_plugins_mtx;

  std::deque<IRCCommand> m_outgoing;
  std::mutex m_outgoing_mtx;
  std::condition_variable m_outgoing_cv;
};

#endif
