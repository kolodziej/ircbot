#include "ircbot/plugin_runner.hpp"

void PluginRunner::appendPlugin(std::shared_ptr<Plugin> plugin) {
  std::lock_guard<std::mutex> lock{m_mtx};
  m_plugins.push_back(plugin);
}

void PluginRunner::insertPlugin(std::shared_ptr<Plugin> plugin, size_t i) {
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

std::list<IRCCommand> PluginRunner::run(const IRCCommand& cmd) {
  std::list<IRCCommand> commands;

  for (auto& plugin : m_plugins) {
    auto p_commands = plugin->run(cmd);
    commands.insert(commands.end(), p_commands.begin(), p_commands.end());
  }

  return commands;
}
