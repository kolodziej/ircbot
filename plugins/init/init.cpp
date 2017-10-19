#include "init.hpp"

Init::Init(PluginManager& manager) :
    Plugin{manager, "Init"},
    m_stage{0}
{}

void Init::run() {
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

bool Init::filter(const IRCCommand& cmd) {
  return false;
}
