#include "init.hpp"

#include <stdexcept>

Init::Init(PluginConfig config) :
    SoPlugin{config},
    m_stage{0},
    m_alt_nicks_index{0} {
}

std::string Init::getName() const {
  return "Init";
}

void Init::onInit() {
  for (auto nick : cfg().get_child("config.alternative_nicks")) {
    m_alt_nicks.push_back(nick.second.data());
  }

  sendNickMsg(cfg().get<std::string>("config.nick"));
  sendUserMsg(cfg().get<std::string>("config.user"),
              cfg().get<std::string>("config.real_name"));
}

void Init::onMessage(IRCCommand cmd) {
  if (cmd.command == "433") {
    LOG(ERROR, "Chosen nick is in use!");
    LOG(INFO, "Trying alternative nicks...");

    LOG(INFO, "Trying nick: ", m_alt_nicks[m_alt_nicks_index]);
    sendNickMsg(m_alt_nicks[m_alt_nicks_index]);
    ++m_alt_nicks_index;
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

