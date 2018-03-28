#ifndef _LUA_PLUGIN_HPP
#define _LUA_PLUGIN_HPP

#include "ircbot/plugin.hpp"

#include "sol.hpp"

class LuaPlugin : public Plugin {
 public:
  LuaPlugin(PluginConfig config);

  std::string getName() const;
  void onInit();
  void onMessage(IRCMessage msg);
  void onNewConfiguration();
  void onShutdown();

 private:
  sol::state m_lua;
};

#endif
