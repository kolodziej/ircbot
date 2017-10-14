#ifndef _PLUGIN_HPP
#define _PLUGIN_HPP

#include <memory>

#include "ircbot/irc_command.hpp"
#include "ircbot/logger.hpp"

#define IRCBOT_PLUGIN(PluginName) \
    extern "C" std::unique_ptr<Plugin> getPlugin() { \
      return std::make_unique<PluginName>(); \
    }

class Plugin {
 public:
  Plugin(std::string name);

  std::string name() const;

  virtual void putIncoming(IRCCommand cmd) = 0;
  virtual IRCCommand getOutgoing() = 0;
  virtual size_t outgoingCount() const = 0;
 private:
  std::string m_name;
};

#endif
