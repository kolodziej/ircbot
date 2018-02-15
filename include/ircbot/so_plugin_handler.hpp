#ifndef _SO_PLUGIN_HANDLER_HPP
#define _SO_PLUGIN_HANDLER_HPP

#include <memory>
#include <functional>

class Client;
class Plugin;

struct SoPluginHandler {
  using GetPluginFuncType = std::unique_ptr<Plugin>(Client*);
  using GetPluginFunc = std::function<GetPluginFuncType>;

  SoPluginHandler();
  SoPluginHandler(GetPluginFunc getPlugin, void* soPluginLibrary);

  GetPluginFunc getPlugin;
  void* soPluginLibrary;

  bool isValid() const;
};

#endif
