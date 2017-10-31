#include "init.hpp"

Init::Init(PluginManager& manager) :
    Plugin{manager, "Init"},
    m_stage{0}
{}

void Init::run() {
  if (m_stage == 0) {
    sendNickMsg(cfg().get<std::string>("config.nick"));
    sendUserMsg(cfg().get<std::string>("config.user"),
                cfg().get<std::string>("config.real_name"));

    ++m_stage;
  } else if (m_stage == 1) {
    auto cmd = getCommand();
    if (cmd.command == "433") {
      
    }
  }
}

bool Init::filter(const IRCCommand& cmd) {
  return (cmd.command == "431" or // 431 == ERR_NONICKNAMEGIVEN
          cmd.command == "433" or // 433 == ERR_NICKNAMEINUSE
          cmd.command == "437" or // 437 == ERR_UNAVAILRESOURCE
          cmd.command == "432" or // 432 == ERR_ERRONEUSNICKNAME
          cmd.command == "436" or // 436 == ERR_NICKCOLLISION
          cmd.command == "484");  // 484 == ERR_RESTRICTED
}

void Init::sendNickMsg(const std::string& nick) {
  IRCCommand nickMsg{
    "NICK",
    { nick }
  };

  send(nickMsg);
}

void Init::sendUserMsg(const std::string& user,
                       const std::string& realname) {
  IRCCommand userMsg{
    "USER",
    { user,
      "0", "*", 
      realname }
  };

  send(userMsg);
}

