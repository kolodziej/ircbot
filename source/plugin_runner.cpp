#include "ircbot/plugin_runner.hpp"

void PluginRunner::appendPlugin(Plugin::Ptr plugin) {
  std::lock_guard<std::mutex> lock{m_mtx};
  m_plugins.push_back(plugin);
}

void PluginRunner::insertPlugin(Plugin::Ptr plugin, size_t i) {
  std::lock_guard<std::mutex> lock{m_mtx};
  auto it = m_plugins.begin();
  std::advance(it, i + 1);
  m_plugins.insert(it, plugin);
}

void PluginRunner::removePlugin(size_t i) {
  std::lock_guard<std::mutex> lock{m_mtx};
  auto it = m_plugins.begin();
  std::advance(it, i);
  m_plugins.erase(it);
}
