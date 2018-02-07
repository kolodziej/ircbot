#include "join.hpp"

#include <string>
#include <stdexcept>

Join::Join(Client& client) :
    Plugin{client, "Join"} {
}

void Join::onInit() {
  for (auto p : cfg().get_child("config.channels")) {
    auto channel = p.second.data();
    if (not m_channels.count(channel)) {
      LOG(INFO, "Trying to join ", channel);
      m_channels.insert(channel);

      IRCCommand msg{
        "JOIN",
        { channel }
      };
      send(msg);
    }
  }
}

void Join::onMessage(IRCCommand cmd) {
  int responseCode = std::stoi(cmd.command);
  switch (responseCode) {
    case 332:
      LOG(INFO, "Joined to channel: ", cmd.params[0], ". Topic: ", cmd.params[1]);
      break;
    default:
      LOG(ERROR, "Error occurred during JOIN command to channel ", cmd.params[0], ": ",
          cmd.params[1]);
      break;
  }
}

bool Join::filter(const IRCCommand& cmd) {
  return (cmd.command == "461" or // 461 == ERR_NEEDMOREPARAMS
          cmd.command == "473" or // 473 == ERR_INVITEONLYCHAN
          cmd.command == "474" or // 474 == ERR_BANNEDFROMCHAN
          cmd.command == "475" or // 475 == ERR_BADCHANNELKEY
          cmd.command == "471" or // 471 == ERR_CHANNELISFULL
          cmd.command == "476" or // 476 == ERR_CHANNELISFULL
          cmd.command == "403" or // 403 == ERR_NOSUCHCHANNEL
          cmd.command == "405" or // 405 == ERR_TOOMANYCHANNELS
          cmd.command == "332");  // 332 == RPL_TOPIC 
}

void Join::onNewConfiguration() {
}
