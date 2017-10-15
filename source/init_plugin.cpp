#include "ircbot/init_plugin.hpp"

InitPlugin::InitPlugin(PluginManager& manager) :
    Plugin{manager, "Init"},
    m_stage{0}
{}

void InitPlugin::run() {
  IRCCommand nickMsg{
    "NICK",
    { "KolK_IRCBot" }
  };

  IRCCommand userMsg{
    "USER",
    { "KolK_IRCBot", "0", "*", "kolodziej.it/ircbot" }
  };
  send(nickMsg);
  send(userMsg);
  stop();
}

bool InitPlugin::filter(const IRCCommand& cmd) {
  return false;
}
