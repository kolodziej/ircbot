#ifndef _PLUGIN_UPTIME_HPP
#define _PLUGIN_UPTIME_HPP

#include "ircbot/so_plugin.hpp"

#include <deque>
#include <string>

class Uptime : public ircbot::SoPlugin {
 public:
  Uptime(std::shared_ptr<ircbot::Core> core);

  std::string getName() const override;
  void onMessage(ircbot::IRCMessage cmd) override;
  bool filter(const ircbot::IRCMessage& cmd) override;

 private:
  std::deque<std::string> m_names;
};

IRCBOT_PLUGIN(Uptime)

#endif
