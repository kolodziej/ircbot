#include "ircbot/plugin_graph.hpp"

#include <regex>

namespace ircbot {

PluginGraph::PluginGraph(std::shared_ptr<Core> core, Config config)
    : m_core{core} {}

std::shared_ptr<Core> PluginGraph::core() { return m_core; }

std::shared_ptr<Plugin> PluginGraph::loadPlugin(const std::string& id) {
  std::string type{}, path{};
  std::tie(type, path) = splitId(id);

  if (type == "_") {
    LOG(ERROR, "Could not parse plugin's ID: ", id);
    return nullptr;
  }

  std::shared_ptr<Plugin> plugin{nullptr};

  if (type == "py") {
    plugin = loadPyPlugin(path);
  } else if (type == "so") {
    plugin = loadSoPlugin(path);
  } else {
    LOG(ERROR, "Unsupported plugin type: ", type);
  }

  if (plugin != nullptr) {
    addPlugin(id, plugin);
  } else {
    LOG(ERROR, "Could not load plugin: ", id);
  }

  return plugin;
}

void PluginGraph::addPlugin(const std::string& id,
                            std::shared_ptr<Plugin> plugin) {}

std::pair<std::string, std::string> PluginGraph::splitId(
    const std::string& id) {
  std::regex reg{"([a-z0-9]{1,8})://(.*)", std::regex_constants::extended};
  std::smatch matches;
  if (std::regex_match(id, matches, reg)) {
    if (matches.size() == 3) {
      return std::make_pair(matches[1], matches[2]);
    }
  }

  return std::make_pair(std::string{"_"}, std::string{});
}

std::shared_ptr<Plugin> PluginGraph::loadPyPlugin(const std::string& path) {
  return nullptr;
}

std::shared_ptr<Plugin> PluginGraph::loadSoPlugin(const std::string& path) {
  void* pluginLibrary = dlopen(path, RTLD_NOW);
  if (pluginLibrary == nullptr) {
    LOG(ERROR, "Could not load file ", fname, ": ", dlerror());
    return nullptr;
  }

  void* getPluginFunc = dlsym(pluginLibrary, "getPlugin");
  std::function<std::unique_ptr<SoPlugin>(PluginConfig)> func =
      reinterpret_cast<std::unique_ptr<SoPlugin> (*)(PluginConfig)>(
          getPluginFunc);

  if (func == nullptr) {
    LOG(ERROR, "Could not load plugin from ", fname, ": ", dlerror());
    return nullptr;
  }

  std::unique_ptr<SoPlugin> plugin = func(config);
  m_dl_plugins[plugin->getId()] = pluginLibrary;

  return plugin;
}

}  // namespace ircbot