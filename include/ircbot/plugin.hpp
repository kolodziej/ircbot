#ifndef _PLUGIN_HPP
#define _PLUGIN_HPP

#include <memory>

#include "ircbot/irc_command.hpp"

class Plugin {
 public:
  Plugin();

  virtual void cmd(const IRCCommand&) = 0;
};

#endif
