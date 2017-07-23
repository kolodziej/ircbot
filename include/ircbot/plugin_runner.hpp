#ifndef _PLUGIN_RUNNER_HPP
#define _PLUGIN_RUNNER_HPP

#include <list>
#include <memory>
#include <mutex>

#include "ircbot/plugin.hpp"

class PluginRunner {
 public:
  PluginRunner() = default;

  void appendPlugin(std::shared_ptr<Plugin>);
  void insertPlugin(std::shared_ptr<Plugin>, size_t);
  void removePlugin(size_t);
  
  std::list<IRCCommand> run(const IRCCommand& cmd);

 private:
  std::list<std::shared_ptr<Plugin>> m_plugins;
  std::mutex m_mtx;
};

#endif
