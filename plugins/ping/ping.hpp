#ifndef _PING_PLUGIN_HPP
#define _PING_PLUGIN_HPP

#include "ircbot/so_plugin.hpp"

class Ping : public ircbot::SoPlugin {
 public:
  Ping(ircbot::PluginConfig config);

  std::string getName() const override;
  void onMessage(ircbot::IRCMessage cmd) override;
  bool filter(const ircbot::IRCMessage& cmd) override;

 private:
  bool m_ping;
  std::string m_server;
};

IRCBOT_PLUGIN(Ping)

#endif
