#ifndef _PLUGIN_HPP
#define _PLUGIN_HPP

#include <memory>

#include "ircbot/irc_command.hpp"

class Plugin {
 public:
  Plugin();

  void push(const IRCCommand&);

  bool available() const;
  IRCCommand pull();

 protected:

};

#endif
