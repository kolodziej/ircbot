#ifndef _SO_PLUGIN_HPP
#define _SO_PLUGIN_HPP

#include "ircbot/plugin.hpp"

#define IRCBOT_PLUGIN(PluginName) \
    extern "C" std::unique_ptr<ircbot::SoPlugin> getPlugin(ircbot::PluginConfig config) { \
      return std::make_unique<PluginName>(config); \
    }

namespace ircbot {

class SoPlugin : public Plugin {
 public:
  SoPlugin(PluginConfig config);

  void setSoLibrary(void*);
  void* getSoLibrary() const;

 private:
  void* m_so_library;
};

} // namespace ircbot

#endif
