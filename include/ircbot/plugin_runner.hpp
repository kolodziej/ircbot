#ifndef _PLUGIN_RUNNER_HPP
#define _PLUGIN_RUNNER_HPP

#include <list>
#include <mutex>

#include "ircbot/plugin.hpp"

class PluginRunner {
 public:
  PluginRunner() = default;

  void appendPlugin(Plugin::Ptr);
  void insertPlugin(Plugin::Ptr, size_t);
  void removePlugin(size_t);

  // run plugins

 private:
  std::list<Plugin::Ptr> m_plugins;
  std::mutex m_mtx;
};

#endif
