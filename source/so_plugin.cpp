#include "ircbot/so_plugin.hpp"

SoPlugin::SoPlugin(Client& client, const std::string& id) :
    Plugin{client, id} {
}

void SoPlugin::setDlLibrary(void* library) {
  m_dl_library = library;
}

void* SoPlugin::getDlLibrary() const {
  return m_dl_library;
}
