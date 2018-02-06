#include "clipboard.hpp"

#include <sstream>

#include "ircbot/utils.hpp"

Clipboard::Clipboard(Client& client) :
    Plugin{client, "Clipboard"},
    m_size{10},
    m_message_size{512} {
}

void Clipboard::onMessage(IRCCommand cmd) {
  using utils::startsWith;

  const auto& data = cmd.params.back();
  if (data.substr(0, 5) == "!clip") {
    LOG(INFO, "Using clipboard plugin");

    if (data[5] == ' ') {
      std::string content = data.substr(6);
      LOG(DEBUG, "Saving data to ", cmd.nick, " clipboard: ", content);
      if (content.size() > m_message_size) {
        IRCCommand msg{
          "PRIVMSG",
          { cmd.nick,
            std::string{"Your message is too big. Limit is "} +
            std::to_string(m_message_size) +
            std::string{" bytes."}
          }
        };
        send(msg);
      } else {
        auto& clipboard = m_clipboard[cmd.nick];
        if (clipboard.size() < m_size) {
          clipboard.push_back(data.substr(6));
        } else {
          IRCCommand msg{
            "PRIVMSG",
            { cmd.nick, "You have no free slots in clipboard! Remove something!" }
          };
          send(msg);
        }
      }
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

void Clipboard::onNewConfiguration() {
  m_size = cfg().get<size_t>("size");
  m_message_size = cfg().get<size_t>("message_size");
}
