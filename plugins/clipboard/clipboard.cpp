#include "clipboard.hpp"

#include <sstream>

#include "ircbot/helpers.hpp"

Clipboard::Clipboard(Client& client) :
    SoPlugin{client},
    m_size{10},
    m_message_size{512} {
}

std::string Clipboard::getName() const {
  return "Clipboard";
}

void Clipboard::onMessage(IRCCommand cmd) {
  using helpers::startsWith;

  auto name = cmd.params[0];

  // check if name is not a channel
  if (std::string{"+&#!"}.find(name[0]) == std::string::npos) {
    name = cmd.nick;
  }

  const auto& data = cmd.params[1];
  if (data.substr(0, 5) == "!clip") {
    LOG(INFO, "Using clipboard plugin");

    if (data[5] == ' ') {
      std::string content = data.substr(6);
      LOG(DEBUG, "Saving data to ", name, " clipboard: ", content);
      if (content.size() > m_message_size) {
        IRCCommand msg{
          "PRIVMSG",
          { name,
            std::string{"Your message is too big. Limit is "} +
            std::to_string(m_message_size) +
            std::string{" bytes."}
          }
        };
        send(msg);
      } else {
        auto& clipboard = m_clipboard[name];
        if (clipboard.size() < m_size) {
          clipboard.push_back(data.substr(6));
        } else {
          IRCCommand msg{
            "PRIVMSG",
            { name, "You have no free slots in clipboard! Remove something!" }
          };
          send(msg);
        }
      }
    } else if (startsWith(data, "!clip-ls")) {
      IRCCommand msg{
        "PRIVMSG",
        { name, "This is your clipboard:" }
      };
      send(msg);

      for (size_t i = 0; i < m_clipboard[name].size(); ++i) {
        const std::string& content = m_clipboard[name][i];
        std::stringstream stream;
        stream << i << " $ " << content;
        msg.params = { name, stream.str() };
        send(msg);
      }
    } else if (startsWith(data, "!clip-help")) {
      IRCCommand msg{
        "PRIVMSG",
        { name, "Commands: !clip - save to clipboard,"
        "!clip-help - this message, "
        "!clip-ls - list clipboard content, "
        "!clip-rm $index - remove paste with number $index"}
      };
      send(msg);
    } else if (startsWith(data, "!clip-rm")) {
      std::string number = data.substr(9);
      int index = std::stoi(number);

      auto& clipboard = m_clipboard[name];
      if (index < clipboard.size()) {
        clipboard.erase(clipboard.begin() + index);
      } else {
        IRCCommand msg{
          "PRIVMSG",
          { name, "Index is too large!" }
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
  m_size = cfg().get<size_t>("config.size");
  m_message_size = cfg().get<size_t>("config.message_size");
}
