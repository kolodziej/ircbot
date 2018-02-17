#ifndef _SO_PLUGIN_HPP
#define _SO_PLUGIN_HPP

#include "ircbot/plugin.hpp"

#define IRCBOT_PLUGIN(PluginName) \
    extern "C" std::unique_ptr<SoPlugin> getPlugin(PluginConfig config) { \
      return std::make_unique<PluginName>(config); \
    }

class SoPlugin : public Plugin {
 public:
  SoPlugin(PluginConfig config);

  void setDlLibrary(void*);
  void* getDlLibrary() const;

 private:
  void* m_dl_library;
};

#endif
