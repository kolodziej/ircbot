#ifndef _PING_PLUGIN_HPP
#define _PING_PLUGIN_HPP

#include "ircbot/so_plugin.hpp"

class Ping : public SoPlugin {
 public:
  Ping(PluginConfig config);

  std::string getName() const override;
  void onMessage(IRCCommand cmd) override;
  bool filter(const IRCCommand& cmd) override;

 private:
  bool m_ping;
  std::string m_server;
};

IRCBOT_PLUGIN(Ping)

#endif
