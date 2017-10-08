#include "ircbot/plugin_manager.hpp"

#include <algorithm>

#include "ircbot/logger.hpp"

void PluginManager::putIncoming(IRCCommand cmd) {
  for (auto& plugin : m_plugins) {
    plugin->putIncoming(cmd);
  }
}

std::vector<IRCCommand> PluginManager::getOutgoing() {
  std::vector<IRCCommand> cmds;
  for (auto& plugin : m_plugins) {
    while (plugin->outgoingCount())
      cmds.push_back(plugin->getOutgoing());
  }

  return cmds;
}

void PluginManager::addPlugin(std::unique_ptr<Plugin>&& plugin) {
  Logger& logger = Logger::getInstance();
  logger(LogLevel::INFO, "Adding plugin ", plugin->name());
  m_plugins.push_back(std::move(plugin));
}

void PluginManager::removePlugin(const std::string& name) {
  auto pred = [&name](const auto& plugin) { return plugin->name() == name; };
  auto remove_it = std::find_if(m_plugins.begin(), m_plugins.end(), pred);

  if (remove_it == m_plugins.end()) {
    throw std::runtime_error{"Could not find such plugin!"};
  }

  m_plugins.erase(remove_it);
}

std::vector<std::string> PluginManager::listPlugins() const {
  std::vector<std::string> names;
  for (const auto& plugin : m_plugins) {
    names.push_back(plugin->name());
  }
}
