#ifndef _PING_PLUGIN_HPP
#define _PING_PLUGIN_HPP

#include "ircbot/plugin.hpp"

class PingPlugin : public Plugin {
 public:
  PingPlugin(PluginManager& manager);

  void run() override;
  bool filter(const IRCCommand& cmd) override;

 private:
  bool m_ping;
  std::string m_server;
};

#endif