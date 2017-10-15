#ifndef _INIT_PLUGIN_HPP
#define _INIT_PLUGIN_HPP

#include "ircbot/plugin.hpp"

class InitPlugin : public Plugin {
 public:
  InitPlugin(PluginManager& manager);

  void run() override;
  bool filter(const IRCCommand& cmd) override;

 private:
  uint32_t m_stage;
};

#endif
