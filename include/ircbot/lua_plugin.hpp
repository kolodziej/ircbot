#ifndef _LUA_PLUGIN_HPP
#define _LUA_PLUGIN_HPP

#include "ircbot/plugin.hpp"

#include "ircbot/lua/state.hpp"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


namespace LuaPluginFunctions {

int LuaPluginStop(lua_State* state);
int LuaPluginCommandsCount(lua_State* state);
int LuaPluginGetCommand(lua_State* state);
int LuaPluginSend(lua_State* state);
int LuaPluginCfg(lua_State* state);

}

class LuaPlugin : public Plugin {
 public:
  LuaPlugin(PluginManager& manager, const std::string& name);

 private:
  lua::State m_state; 
  
};

#endif
