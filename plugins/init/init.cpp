#include "init.hpp"

#include <stdexcept>

using namespace ircbot;

Init::Init(std::shared_ptr<Core> core)
    : SoPlugin{core}, m_stage{0}, m_alt_nicks_index{0} {}

std::string Init::getName() const { return "Init"; }

void Init::onInit() {
  for (auto nick : getConfig()["config"]["alternative_nicks"]) {
    m_alt_nicks.push_back(nick.as<std::string>());
  }

  sendNickMsg(getConfig()["config"]["nick"].as<std::string>());
  sendUserMsg(getConfig()["config"]["user"].as<std::string>(),
              getConfig()["config"]["real_name"].as<std::string>());
}

void Init::onMessage(IRCMessage cmd) {
  if (cmd.command == "433") {
    LOG(ERROR, "Chosen nick is in use!");
    LOG(INFO, "Trying alternative nicks...");

    LOG(INFO, "Trying nick: ", m_alt_nicks[m_alt_nicks_index]);
    sendNickMsg(m_alt_nicks[m_alt_nicks_index]);
    ++m_alt_nicks_index;
  }
}

bool Init::filter(const IRCMessage& cmd) {
  return (cmd.command == "431" or  // 431 == ERR_NONICKNAMEGIVEN
          cmd.command == "433" or  // 433 == ERR_NICKNAMEINUSE
          cmd.command == "437" or  // 437 == ERR_UNAVAILRESOURCE
          cmd.command == "432" or  // 432 == ERR_ERRONEUSNICKNAME
          cmd.command == "436" or  // 436 == ERR_NICKCOLLISION
          cmd.command == "484");   // 484 == ERR_RESTRICTED
}

void Init::sendNickMsg(const std::string& nick) {
  IRCMessage nickMsg{"NICK", {nick}};

  send(nickMsg);
}

void Init::sendUserMsg(const std::string& user, const std::string& realname) {
  IRCMessage userMsg{"USER", {user, "0", "*", realname}};

  send(userMsg);
}
