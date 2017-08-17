#include "ircbot/plugin_manager.hpp"

void PluginManager::appendPlugin(std::shared_ptr<Plugin> plugin) {
  std::lock_guard<std::mutex> lock{m_plugins_mtx};
  m_plugins.push_back(plugin);
}

void PluginManager::insertPlugin(std::shared_ptr<Plugin> plugin, size_t i) {
  std::lock_guard<std::mutex> lock{m_plugins_mtx};
  auto it = m_plugins.begin();
  std::advance(it, i + 1);
  m_plugins.insert(it, plugin);
}

void PluginManager::removePlugin(size_t i) {
  std::lock_guard<std::mutex> lock{m_plugins_mtx};
  auto it = m_plugins.begin();
  std::advance(it, i);
  m_plugins.erase(it);
}

void PluginManager::push(const IRCCommand& cmd) {
  for (auto& plugin : m_plugins) {
    plugin->push(cmd);
  }
}
