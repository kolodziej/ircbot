#ifndef _PLUGIN_HELLOWORLD_HPP
#define _PLUGIN_HELLOWORLD_HPP

#include "ircbot/so_plugin.hpp"

#include <deque>
#include <string>

class HelloWorld : public ircbot::SoPlugin {
 public:
  HelloWorld(std::shared_ptr<ircbot::Core> core);

  std::string getName() const override;
  void onMessage(ircbot::IRCMessage cmd) override;
  bool filter(const ircbot::IRCMessage& cmd) override;

 private:
  std::deque<std::string> m_names;
};

IRCBOT_PLUGIN(HelloWorld)

#endif
