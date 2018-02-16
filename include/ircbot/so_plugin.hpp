#ifndef _SO_PLUGIN_HPP
#define _SO_PLUGIN_HPP

#include "ircbot/plugin.hpp"

#define IRCBOT_PLUGIN(PluginName) \
    extern "C" std::unique_ptr<SoPlugin> getPlugin(Client* client) { \
      return std::make_unique<PluginName>(*client); \
    }

class SoPlugin : public Plugin {
 public:
  SoPlugin(Client& client);

  void setDlLibrary(void*);
  void* getDlLibrary() const;

 private:
  void* m_dl_library;
};

#endif
