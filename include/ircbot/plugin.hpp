#ifndef _PLUGIN_HPP
#define _PLUGIN_HPP

#include <memory>

#include "ircbot/irc_command.hpp"

class Plugin {
 public:
  Plugin() = default;

  virtual std::list<IRCCommand> run(const IRCCommand&) = 0;
};

#endif
