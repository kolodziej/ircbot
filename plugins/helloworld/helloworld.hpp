#ifndef _PLUGIN_HELLOWORLD_HPP
#define _PLUGIN_HELLOWORLD_HPP

#include "ircbot/plugin.hpp"

#include <deque>
#include <string>

class HelloWorld : public Plugin {
 public:
  HelloWorld(Client& client);

  void onMessage(IRCCommand cmd) override;
  bool filter(const IRCCommand& cmd) override;
  
 private:
  std::deque<std::string> m_names;
};

IRCBOT_PLUGIN(HelloWorld)

#endif
