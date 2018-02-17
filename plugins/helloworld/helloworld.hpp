#ifndef _PLUGIN_HELLOWORLD_HPP
#define _PLUGIN_HELLOWORLD_HPP

#include "ircbot/so_plugin.hpp"

#include <deque>
#include <string>

class HelloWorld : public SoPlugin {
 public:
  HelloWorld(PluginConfig config);

  std::string getName() const override;
  void onMessage(IRCMessage cmd) override;
  bool filter(const IRCMessage& cmd) override;
  
 private:
  std::deque<std::string> m_names;
};

IRCBOT_PLUGIN(HelloWorld)

#endif
