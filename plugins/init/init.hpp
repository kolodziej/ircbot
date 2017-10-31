#ifndef _INIT_PLUGIN_HPP
#define _INIT_PLUGIN_HPP

#include "ircbot/plugin.hpp"

class Init : public Plugin {
 public:
  Init(PluginManager& manager);

  void run() override;
  bool filter(const IRCCommand& cmd) override;

 private:
  void sendNickMsg(const std::string& nick);
  void sendUserMsg(const std::string& user,
                   const std::string& realname);

 private:
  uint32_t m_stage;
};

IRCBOT_PLUGIN(Init)

#endif
