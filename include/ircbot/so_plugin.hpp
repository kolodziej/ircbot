#ifndef _SO_PLUGIN_HPP
#define _SO_PLUGIN_HPP

#include "ircbot/plugin.hpp"

#define IRCBOT_PLUGIN(PluginName)                         \
  extern "C" std::shared_ptr<ircbot::SoPlugin> getPlugin( \
      std::shared_ptr<ircbot::Core> core) {               \
    return std::make_shared<PluginName>(core);            \
  }

namespace ircbot {

class SoPlugin : public Plugin {
 public:
  SoPlugin(std::shared_ptr<Core> core);

  void setSoLibrary(void*);
  void* getSoLibrary() const;

 private:
  void* m_so_library;
};

}  // namespace ircbot

#endif
