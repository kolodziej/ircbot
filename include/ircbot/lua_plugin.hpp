#ifndef _LUA_PLUGIN_HPP
#define _LUA_PLUGIN_HPP

#include "ircbot/plugin.hpp"

#include "ircbot/lua/state.hpp"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace lua_plugin_functions {

int stop(lua_State* state);
int commandsCount(lua_State* state);
int getCommand(lua_State* state);
int send(lua_State* state);
int cfg(lua_State* state);
int log(lua_State* state);

} // namespace lua_plugin_functions

class LuaPlugin : public Plugin {
 public:
  LuaPlugin(Client& client, const std::string& name, const std::string& path);

  void run() override;
  void onMessage(IRCCommand cmd) override;
  bool filter(const IRCCommand& cmd) override;

 protected:
  void pushCommandOnLuaStack(const IRCCommand& cmd);
  IRCCommand popCommandFromLuaStack();

 private:
  lua::State m_state; 

  friend int lua_plugin_functions::stop(lua_State*);
  friend int lua_plugin_functions::commandsCount(lua_State*);
  friend int lua_plugin_functions::getCommand(lua_State*);
  friend int lua_plugin_functions::send(lua_State*);
  friend int lua_plugin_functions::cfg(lua_State*);
};

#endif
