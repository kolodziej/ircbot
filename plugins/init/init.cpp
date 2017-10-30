#include "init.hpp"

Init::Init(PluginManager& manager) :
    Plugin{manager, "Init"},
    m_stage{0}
{}

void Init::run() {
  IRCCommand nickMsg{
    "NICK",
    { cfg().get<std::string>("config.nick") }
  };

  IRCCommand userMsg{
    "USER",
    { cfg().get<std::string>("config.user"),
      "0", "*", 
      cfg().get<std::string>("config.real_name") }
  };
  send(nickMsg);
  send(userMsg);
  stop();
}

bool Init::filter(const IRCCommand& cmd) {
  return false;
}
