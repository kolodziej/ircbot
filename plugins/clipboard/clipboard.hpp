#ifndef _CLIPBOARD_PLUGIN_HPP
#define _CLIPBOARD_PLUGIN_HPP

#include "ircbot/plugin.hpp"

class Clipboard : public Plugin {
 public:
  Clipboard(Client& client);

  void onMessage(IRCCommand cmd) override;
  bool filter(const IRCCommand& cmd) override;

 private:
  std::map<std::string, std::vector<std::string>> m_clipboard;
};

IRCBOT_PLUGIN(Clipboard)

#endif
