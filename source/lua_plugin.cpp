#include "ircbot/lua_plugin.hpp"

namespace lua_plugin_functions {

int stop(lua_State* state) {
  int argc = lua_gettop(state);
  if (argc >= 1) {
    // TODO(KolK): report warning if argc > 1
    LuaPlugin* plugin = static_cast<LuaPlugin*>(lua_touserdata(state, 1));
    plugin->stop();
  }


  return 0;
}

int commandsCount(lua_State* state) {
  int argc = lua_gettop(state);
  if (argc >= 1) {
    // TODO(KolK): report warning if argc > 0
    LuaPlugin* plugin = static_cast<LuaPlugin*>(lua_touserdata(state, 1));
    lua_pushinteger(state, plugin->commandsCount());
    return 1;
  }

  return 0;
}

int getCommand(lua_State* state) {
  int argc = lua_gettop(state);
  if (argc >= 1) {
    // TODO(KolK): report warning if argc > 0
    LuaPlugin* plugin = static_cast<LuaPlugin*>(lua_touserdata(state, 1));
    auto cmd = plugin->getCommand();

    lua_newtable(state);

    lua_pushstring(state, "servername");
    lua_pushstring(state, cmd.servername.data());
    lua_settable(state, -3);

    lua_pushstring(state, "user");
    lua_pushstring(state, cmd.user.data());
    lua_settable(state, -3);

    lua_pushstring(state, "nick");
    lua_pushstring(state, cmd.nick.data());
    lua_settable(state, -3);

    lua_pushstring(state, "host");
    lua_pushstring(state, cmd.host.data());
    lua_settable(state, -3);

    lua_pushstring(state, "command");
    lua_pushstring(state, cmd.command.data());
    lua_settable(state, -3);

    lua_pushstring(state, "params");
    lua_newtable(state);

    for (uint32_t i = 0; i < cmd.params.size(); ++i) {
      lua_pushinteger(state, i);
      lua_pushstring(state, cmd.params[i].data());
      lua_settable(state, -3);
    }

    lua_settable(state, -3); // push params table to command
    return 1;
  }

  return 0;
}


}

LuaPlugin::LuaPlugin(PluginManager& manager, const std::string& name) :
    Plugin{manager, name} {
}
