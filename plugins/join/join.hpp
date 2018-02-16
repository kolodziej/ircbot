#ifndef _JOIN_PLUGIN_HPP
#define _JOIN_PLUGIN_HPP

#include "ircbot/so_plugin.hpp"

#include <unordered_set>
#include <string>

class Join : public SoPlugin {
 public:
  Join(Client& client);

  std::string getName() const override;
  void onInit() override;
  void onMessage(IRCCommand cmd) override;
  bool filter(const IRCCommand& cmd) override;
  void onNewConfiguration() override;
 
 private:
  std::unordered_set<std::string> m_channels;
};

IRCBOT_PLUGIN(Join)

#endif
