#include "ircbot/init_plugin.hpp"

InitPlugin::InitPlugin() :
    Plugin{"Init"},
    m_stage{0}
{}

void InitPlugin::putIncoming(IRCCommand cmd) {
  return;
}

IRCCommand InitPlugin::getOutgoing() {
  IRCCommand cmd;
  if (m_stage == 0) {
    cmd.command = "NICK";
    cmd.params = { "KolK1" };
    ++m_stage;
  } else if (m_stage == 1) {
    cmd.command = "USER";
    cmd.params = {
      "KolK1",
      "KolK1",
      "KolK1",
      "KolK1"
    };
    ++m_stage;
  }

  return cmd;
}

size_t InitPlugin::outgoingCount() const {
  return 2 - m_stage;
}
