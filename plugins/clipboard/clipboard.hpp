#ifndef _CLIPBOARD_PLUGIN_HPP
#define _CLIPBOARD_PLUGIN_HPP

#include "ircbot/so_plugin.hpp"

class Clipboard : public SoPlugin {
 public:
  Clipboard(PluginConfig config);

  std::string getName() const override;
  void onMessage(IRCCommand cmd) override;
  bool filter(const IRCCommand& cmd) override;
  void onNewConfiguration() override;

 private:
  std::map<std::string, std::vector<std::string>> m_clipboard;

  size_t m_size;
  size_t m_message_size;
};

IRCBOT_PLUGIN(Clipboard)

#endif
