#ifndef _PING_PLUGIN_HPP
#define _PING_PLUGIN_HPP

#include "ircbot/plugin.hpp"

class PingPlugin : public Plugin {
 public:
  PingPlugin();

  void putIncoming(IRCCommand cmd) override;
  IRCCommand getOutgoing() override;
  size_t outgoingCount() const override;

 private:
  bool m_ping;
  std::string m_server;
};

#endif
