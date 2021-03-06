#ifndef _INIT_PLUGIN_HPP
#define _INIT_PLUGIN_HPP

#include "ircbot/so_plugin.hpp"

#include <string>
#include <vector>

class Init : public ircbot::SoPlugin {
 public:
  Init(ircbot::PluginConfig config);

  std::string getName() const override;
  void onInit() override;
  void onMessage(ircbot::IRCMessage cmd) override;
  bool filter(const ircbot::IRCMessage& cmd) override;

 private:
  void sendNickMsg(const std::string& nick);
  void sendUserMsg(const std::string& user, const std::string& realname);

 private:
  uint32_t m_stage;

  std::vector<std::string> m_alt_nicks;
  uint32_t m_alt_nicks_index;
};

IRCBOT_PLUGIN(Init)

#endif
