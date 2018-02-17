#include "ircbot/so_plugin.hpp"

SoPlugin::SoPlugin(PluginConfig config) :
    Plugin{config} {
}

void SoPlugin::setDlLibrary(void* library) {
  m_dl_library = library;
}

void* SoPlugin::getDlLibrary() const {
  return m_dl_library;
}
