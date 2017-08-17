#ifndef _PING_PLUGIN_HPP
#define _PING_PLUGIN_HPP

#include "plugin.hpp"

class PingPlugin : public Plugin {
  public:
    PingPlugin();

    void cmd(const IRCCommand&) override;
};

#endif
