#ifndef _PLUGIN_LOADER_HPP
#define _PLUGIN_LOADER_HPP

#include "ircbot/plugin.hpp"

#include <memory>
#include <functional>

class PluginLoader {
 public:
  PluginLoader() = default;
  PluginLoader(const std::string& dir);

  void setDirectory(const std::string& dir);
  std::string getDirectory() const;

  std::unique_ptr<Plugin> loadPlugin(const std::string& fname);

 private:
  std::string m_dir;
};

#endif
