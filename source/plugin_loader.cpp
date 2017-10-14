#include "ircbot/plugin_loader.hpp"

#include <dlfcn.h>

#include "ircbot/logger.hpp"

PluginLoader::PluginLoader(const std::string& dir) :
    m_dir{dir} {
}

void PluginLoader::setDirectory(const std::string& dir) {
  m_dir = dir;
}

std::string PluginLoader::getDirectory() const {
  return m_dir;
}

std::unique_ptr<Plugin> PluginLoader::loadPlugin(const std::string& fname) {
  Logger& logger = Logger::getInstance();
  void* plugin = dlopen((m_dir + "/" + fname).data(), RTLD_NOW);
  if (plugin == nullptr) {
    logger(LogLevel::ERROR, "Could not load file: ", dlerror());
    return nullptr;
  }

  using GetPluginFunc = std::function<std::unique_ptr<Plugin>(void)>;
  GetPluginFunc func = reinterpret_cast<std::unique_ptr<Plugin> (*)()>(dlsym(plugin, "getPlugin"));
  if (func == nullptr) {
    logger(LogLevel::ERROR, "Could not load plugin: ", dlerror());
    return nullptr;
  }

  return func();
}
