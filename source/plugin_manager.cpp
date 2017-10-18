#include "ircbot/plugin_manager.hpp"

#include <algorithm>
#include <functional>

#include <dlfcn.h>

#include "ircbot/logger.hpp"
#include "ircbot/plugin.hpp"

void PluginManager::initializePlugins(Config& cfg) {
  for (auto plugin : cfg.tree().get_child("plugins")) {
    LOG(INFO, "Processing plugin: ", plugin.first);
    auto name_pred = [pn = plugin.first] (const std::unique_ptr<Plugin>& p) {
      return p->name() == pn;
    };

    std::string soPath =
      plugin.second.get("soPath", std::string());

    auto it = std::find_if(m_plugins.begin(), m_plugins.end(), name_pred);
    if (it != m_plugins.end()) {
      if (soPath.empty()) {
        std::unique_ptr<Plugin>& it_plugin = *it;
        it_plugin->setConfig(Config(plugin.second));
      } else {
        LOG(WARNING, "Cannot load plugin ", plugin.first, ". Such plugin exists!");
      }

    } else {
      if (soPath.empty()) {
        LOG(WARNING, "Empty soPath for ", plugin.first, " plugin. Omitting!");
        continue;
      }

      LOG(INFO, "Loading SO plugin from: ", soPath);
      auto soPlugin = loadSoPlugin(soPath);
      if (soPlugin != nullptr) {
        soPlugin->setConfig(Config(plugin.second));
        addPlugin(std::move(soPlugin));
      }
    }
  }
}

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

  return names;
}

std::unique_ptr<Plugin> PluginManager::loadSoPlugin(const std::string& fname) {
  void* plugin = dlopen(fname.data(), RTLD_NOW);
  if (plugin == nullptr) {
    LOG(ERROR, "Could not load file ", fname, ": ", dlerror());
    return nullptr;
  }

  std::function<std::unique_ptr<Plugin>(PluginManager*)> func =
      reinterpret_cast<std::unique_ptr<Plugin> (*)(PluginManager*)>
      (dlsym(plugin, "getPlugin"));

  if (func == nullptr) {
    LOG(ERROR, "Could not load plugin from ", fname, ": ", dlerror());
    return nullptr;
  }

  return func(this);
}

void PluginManager::startPlugins() {
  for (auto& plugin : m_plugins) {
    LOG(INFO, "Starting plugin ", plugin->name());
    plugin->spawn();
  }
}

