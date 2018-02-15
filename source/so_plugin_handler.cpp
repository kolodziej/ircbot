#include "ircbot/so_plugin_handler.hpp"

SoPluginHandler::SoPluginHandler() :
    getPlugin{nullptr},
    soPluginLibrary{nullptr} {
}

SoPluginHandler::SoPluginHandler(GetPluginFunc getPlugin, void* soPluginLibrary) :
    getPlugin{getPlugin},
    soPluginLibrary{soPluginLibrary} {
}

bool SoPluginHandler::isValid() const {
  return getPlugin != nullptr and soPluginLibrary != nullptr;
}
