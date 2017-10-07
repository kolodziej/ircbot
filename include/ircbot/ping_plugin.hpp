#ifndef _PING_PLUGIN_HPP
#define _PING_PLUGIN_HPP

#include "ircbot/plugin.hpp"

class PingPlugin : public Plugin {
 public:
  PingPlugin();

  void putIncoming(IRCCommand cmd);
  IRCCommand getOutgoing();
  size_t outgoingCount() const;

 private:
  bool m_ping;
  std::string m_server;
};

#endif
