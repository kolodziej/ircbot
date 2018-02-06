#include "clipboard.hpp"

#include <sstream>

#include "ircbot/utils.hpp"

Clipboard::Clipboard(Client& client) :
    Plugin{client, "Clipboard"} {
}

void Clipboard::onMessage(IRCCommand cmd) {
  using utils::startsWith;

  const auto& data = cmd.params.back();
  if (data.substr(0, 5) == "!clip") {
    LOG(INFO, "Using clipboard plugin");

    if (data[5] == ' ') {
      LOG(DEBUG, "Saving data to ", cmd.nick, " clipboard: ", data.substr(6));
      m_clipboard[cmd.nick].push_back(data.substr(6));
    } else if (startsWith(data, "!clip-ls")) {
      IRCCommand msg{
        "PRIVMSG",
        { cmd.nick, "This is your clipboard:" }
      };
      send(msg);

      for (size_t i = 0; i < m_clipboard[cmd.nick].size(); ++i) {
        const std::string& content = m_clipboard[cmd.nick][i];
        std::stringstream stream;
        stream << i << " $ " << content;
        msg.params = { cmd.nick, stream.str() };
        send(msg);
      }
    } else if (startsWith(data, "!clip-help")) {
      IRCCommand msg{
        "PRIVMSG",
        { cmd.nick, "Commands: !clip - save to clipboard,"
        "!clip-help - this message, "
        "!clip-ls - list clipboard content, "
        "!clip-rm $index - remove paste with number $index"}
      };
      send(msg);
    } else if (startsWith(data, "!clip-rm")) {
      std::string number = data.substr(9);
      int index = std::stoi(number);

      auto& clipboard = m_clipboard[cmd.nick];
      if (index < clipboard.size()) {
        clipboard.erase(clipboard.begin() + index);
      } else {
        IRCCommand msg{
          "PRIVMSG",
          { cmd.nick, "Index is too large!" }
        };

        send(msg);
      }
    }
  }
}

bool Clipboard::filter(const IRCCommand& cmd) {
  return cmd.command == "PRIVMSG";
}
