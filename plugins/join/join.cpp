#include "join.hpp"

#include <stdexcept>
#include <string>

using namespace ircbot;

Join::Join(PluginConfig config) : SoPlugin{config} {}

std::string Join::getName() const { return "Join"; }

void Join::onInit() {
  for (auto p : getConfig()["config"]["channels"]) {
    auto channel = p.as<std::string>();
    if (not m_channels.count(channel)) {
      LOG(INFO, "Trying to join ", channel);
      m_channels.insert(channel);

      IRCMessage msg{"JOIN", {channel}};
      send(msg);
    }
  }
}

void Join::onMessage(IRCMessage cmd) {
  int responseCode = std::stoi(cmd.command);
  switch (responseCode) {
    case 332:
      LOG(INFO, "Joined to channel: ", cmd.params[0],
          ". Topic: ", cmd.params[1]);
      break;
    default:
      LOG(ERROR, "Error occurred during JOIN command to channel ",
          cmd.params[0], ": ", cmd.params[1]);
      break;
  }
}

bool Join::filter(const IRCMessage& cmd) {
  return (cmd.command == "461" or  // 461 == ERR_NEEDMOREPARAMS
          cmd.command == "473" or  // 473 == ERR_INVITEONLYCHAN
          cmd.command == "474" or  // 474 == ERR_BANNEDFROMCHAN
          cmd.command == "475" or  // 475 == ERR_BADCHANNELKEY
          cmd.command == "471" or  // 471 == ERR_CHANNELISFULL
          cmd.command == "476" or  // 476 == ERR_CHANNELISFULL
          cmd.command == "403" or  // 403 == ERR_NOSUCHCHANNEL
          cmd.command == "405" or  // 405 == ERR_TOOMANYCHANNELS
          cmd.command == "332");   // 332 == RPL_TOPIC
}

void Join::onNewConfiguration() {}
