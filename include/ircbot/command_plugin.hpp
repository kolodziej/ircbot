#ifndef _COMMAND_PLUGIN_HPP
#define _COMMAND_PLUGIN_HPP

#include "ircbot/plugin.hpp"

class CommandPlugin : public Plugin {
 public:
  CommandPlugin();

  virtual void run() override;
  virtual void onMessage() override;
  virtual bool filter(const IRCCommand& cmd) override;
};

#endif
