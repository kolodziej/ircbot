#ifndef _PLUGIN_HELLOWORLD_HPP
#define _PLUGIN_HELLOWORLD_HPP

#include "ircbot/so_plugin.hpp"

#include <deque>
#include <string>

class HelloWorld : public SoPlugin {
 public:
  HelloWorld(Client& client);

  std::string getName() const override;
  void onMessage(IRCCommand cmd) override;
  bool filter(const IRCCommand& cmd) override;
  
 private:
  std::deque<std::string> m_names;
};

IRCBOT_PLUGIN(HelloWorld)

#endif
