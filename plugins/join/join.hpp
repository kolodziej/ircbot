#ifndef _JOIN_PLUGIN_HPP
#define _JOIN_PLUGIN_HPP

#include "ircbot/so_plugin.hpp"

#include <string>
#include <unordered_set>

class Join : public ircbot::SoPlugin {
 public:
  Join(std::shared_ptr<ircbot::Core> core);

  std::string getName() const override;
  void onInit() override;
  void onMessage(ircbot::IRCMessage cmd) override;
  bool filter(const ircbot::IRCMessage& cmd) override;
  void onNewConfiguration() override;

 private:
  std::unordered_set<std::string> m_channels;
};

IRCBOT_PLUGIN(Join)

#endif
