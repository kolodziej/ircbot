#include "ircbot/plugin_manager.hpp"

#include <algorithm>
#include <functional>

#include <dlfcn.h>

#include "ircbot/logger.hpp"
#include "ircbot/plugin.hpp"

void PluginManager::putIncoming(IRCCommand cmd) {
  for (auto& plugin : m_plugins) {
    if (plugin->filter(cmd)) {
      DEBUG("Plugin ", plugin->name(),
            " filtering passed for command: ", cmd.toString(true));
      plugin->receive(cmd);
    }
  }
}

IRCCommand PluginManager::getOutgoing() {
  std::unique_lock<std::mutex> lock{m_outgoing_mtx};
  DEBUG("Trying to get message to send...");
  m_outgoing_cv.wait(lock, [this] { return m_outgoing.size() > 0; });
  DEBUG("Getting message from queue...");
  auto cmd = m_outgoing.front();
  m_outgoing.pop_front();
  lock.unlock();
  return cmd;
}

void PluginManager::addOutgoing(IRCCommand cmd) {
  std::unique_lock<std::mutex> lock{m_outgoing_mtx};
  DEBUG("PluginManager is adding message to send queue");
  m_outgoing.push_back(cmd);
  lock.unlock();

  m_outgoing_cv.notify_all();
}

void PluginManager::addPlugin(std::unique_ptr<Plugin>&& plugin) {
  plugin->spawn();
  LOG(INFO, "Adding plugin ", plugin->name());
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

std::unique_ptr<Plugin> PluginManager::loadSoPlugin(const std::string& fname) {
  void* plugin = dlopen(fname.data(), RTLD_NOW);
  if (plugin == nullptr) {
    LOG(ERROR, "Could not load file: ", dlerror());
    return nullptr;
  }

  std::function<std::unique_ptr<Plugin>(PluginManager*)> func =
      reinterpret_cast<std::unique_ptr<Plugin> (*)(PluginManager*)>
      (dlsym(plugin, "getPlugin"));

  if (func == nullptr) {
    LOG(ERROR, "Could not load plugin: ", dlerror());
    return nullptr;
  }

  return func(this);
}
