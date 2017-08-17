#ifndef _PLUGIN_MANAGER_HPP
#define _PLUGIN_MANAGER_HPP

#include <list>
#include <memory>
#include <mutex>
#include <queue>

#include "ircbot/plugin.hpp"

class PluginManager {
 public:
  PluginManager() = default;

  void appendPlugin(std::shared_ptr<Plugin>);
  void insertPlugin(std::shared_ptr<Plugin>, size_t);
  void removePlugin(size_t);
  
  void run(const IRCCommand& cmd);

 private:
  std::list<std::shared_ptr<Plugin>> m_plugins;
  std::mutex m_plugins_mtx;

  std::queue<IRCCommand> m_responses;
  std::mutex m_responses_mtx;
};

#endif
