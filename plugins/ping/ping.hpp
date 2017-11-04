#ifndef _PING_PLUGIN_HPP
#define _PING_PLUGIN_HPP

#include "ircbot/plugin.hpp"

class Ping : public Plugin {
 public:
  Ping(PluginManager& manager);

  void onMessage(IRCCommand cmd) override;
  bool filter(const IRCCommand& cmd) override;

 private:
  bool m_ping;
  std::string m_server;
};

IRCBOT_PLUGIN(Ping)

#endif
