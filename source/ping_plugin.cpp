#include "ircbot/ping_plugin.hpp"

PingPlugin::PingPlugin() :
    Plugin{"Ping"}
{}

void PingPlugin::putIncoming(IRCCommand cmd) {
  if (cmd.command == "PING") {
    size_t index = cmd.params.size() - 1;
    m_ping = true;
    m_server = cmd.params[index];
  }
}

IRCCommand PingPlugin::getOutgoing() {
  m_ping = false;
  // TODO(KolK): create proper ctors for IRCCommand
  IRCCommand cmd;
  cmd.command = "PONG";
  cmd.params.push_back(m_server);
  return cmd;
}

size_t PingPlugin::outgoingCount() const {
  return m_ping;
}
