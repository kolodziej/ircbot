#ifndef _INIT_PLUGIN_HPP
#define _INIT_PLUGIN_HPP

#include "ircbot/plugin.hpp"

class InitPlugin : public Plugin {
 public:
  InitPlugin();

  void putIncoming(IRCCommand cmd);
  IRCCommand getOutgoing();
  size_t outgoingCount() const;

 private:
  uint32_t m_stage;
};

#endif
