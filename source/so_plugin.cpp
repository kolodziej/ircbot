#include "ircbot/so_plugin.hpp"

#include <dlfcn.h>

namespace ircbot {

SoPlugin::SoPlugin(PluginConfig config) :
    Plugin{config},
    m_so_library{nullptr} {
}

void SoPlugin::setSoLibrary(void* library) {
  m_so_library = library;
}

void* SoPlugin::getSoLibrary() const {
  return m_so_library;
}

} // namespace ircbot
