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

int send(lua_State* state) {
  int argc = lua_gettop(state);
  // TODO(KolK): report warning if argc > 0
  LuaPlugin* plugin = static_cast<LuaPlugin*>(lua_touserdata(state, 1));

  const char* msg = lua_tostring(state, 2);

  LOG(INFO, "Trying to send message from lua code: ", msg);
}

int cfg(lua_State* state) {
  int argc = lua_gettop(state);
  // TODO(KolK): report warning if argc > 0
  LuaPlugin* plugin = static_cast<LuaPlugin*>(lua_touserdata(state, 1));

}

} // namespace lua_plugin_functions

LuaPlugin::LuaPlugin(Client& client,
                     const std::string& name,
                     const std::string& path) :
  Plugin{client, name} {

  if (luaL_loadfile(m_state(), path.data()) != 0) {
    throw std::runtime_error{"LuaPlugin couldn't load lua script!"};
  }
  luaL_openlibs(m_state());

  lua_pushlightuserdata(m_state(), static_cast<void*>(this));

  lua_newtable(m_state());

  lua_pushstring(m_state(), "__index");
  lua_newtable(m_state());

  lua_pushstring(m_state(), "stop");
  lua_pushcfunction(m_state(), lua_plugin_functions::stop);
  lua_settable(m_state(), -3);

  lua_pushstring(m_state(), "send");
  lua_pushcfunction(m_state(), lua_plugin_functions::send);
  lua_settable(m_state(), -3);

  lua_pushstring(m_state(), "cfg");
  lua_pushcfunction(m_state(), lua_plugin_functions::cfg);
  lua_settable(m_state(), -3);

  lua_settable(m_state(), -3);
  lua_setmetatable(m_state(), -2);

  lua_setglobal(m_state(), "Plugin");
}

void LuaPlugin::run() {
  lua_call(m_state(), 0, 0);
  Plugin::run();
}

void LuaPlugin::onMessage(IRCCommand cmd) {
  LOG(INFO, "Calling for cmd: ", cmd.toString());
  lua_getglobal(m_state(), "onMessage");
  if (lua_type(m_state(), -1) == LUA_TFUNCTION) {
    pushCommandOnLuaStack(cmd);
    lua_call(m_state(), 1, 0);
  } else {
    throw std::runtime_error{"Function onMessage does not exist!"};
  }
}

bool LuaPlugin::filter(const IRCCommand& cmd) {
  lua_getglobal(m_state(), "filter");
  if (lua_type(m_state(), -1) == LUA_TFUNCTION) {
    pushCommandOnLuaStack(cmd);
    lua_call(m_state(), 1, 1);
    
    bool result = lua_toboolean(m_state(), 1);
    return result;
  }

  return true;
}

void LuaPlugin::pushCommandOnLuaStack(const IRCCommand& cmd) {
  lua_newtable(m_state());

  lua_pushstring(m_state(), "servername");
  lua_pushstring(m_state(), cmd.servername.data());
  lua_settable(m_state(), -3);

  lua_pushstring(m_state(), "user");
  lua_pushstring(m_state(), cmd.user.data());
  lua_settable(m_state(), -3);

  lua_pushstring(m_state(), "nick");
  lua_pushstring(m_state(), cmd.nick.data());
  lua_settable(m_state(), -3);

  lua_pushstring(m_state(), "host");
  lua_pushstring(m_state(), cmd.host.data());
  lua_settable(m_state(), -3);

  lua_pushstring(m_state(), "command");
  lua_pushstring(m_state(), cmd.command.data());
  lua_settable(m_state(), -3);

  lua_pushstring(m_state(), "params");
  lua_newtable(m_state());

  for (uint32_t i = 0; i < cmd.params.size(); ++i) {
    lua_pushinteger(m_state(), i);
    lua_pushstring(m_state(), cmd.params[i].data());
    lua_settable(m_state(), -3);
  }

  lua_settable(m_state(), -3); // push params table to command
}

IRCCommand LuaPlugin::popCommandFromLuaStack() {
  IRCCommand cmd;
  int type;

  lua_pushstring(m_state(), "servername");
  lua_gettable(m_state(), -2);
  type = lua_type(m_state(), -1);
  if (type == LUA_TSTRING) {
    const char* str = lua_tostring(m_state(), -1);
    cmd.servername = std::string(str);
  } else if (type != LUA_TNIL) {
    throw std::runtime_error{"Bad type of cmd.servername value!"};
  }

  lua_pushstring(m_state(), "user");
  lua_gettable(m_state(), -2);
  type = lua_type(m_state(), -1);
  if (type == LUA_TSTRING) {
    const char* str = lua_tostring(m_state(), -1);
    cmd.user = std::string(str);
  } else if (type != LUA_TNIL) {
    throw std::runtime_error{"Bad type of cmd.user value!"};
  }

  lua_pushstring(m_state(), "nick");
  lua_gettable(m_state(), -2);
  type = lua_type(m_state(), -1);
  if (type == LUA_TSTRING) {
    const char* str = lua_tostring(m_state(), -1);
    cmd.nick = std::string(str);
  } else if (type != LUA_TNIL) {
    throw std::runtime_error{"Bad type of cmd.nick value!"};
  }

  lua_pushstring(m_state(), "host");
  lua_gettable(m_state(), -2);
  type = lua_type(m_state(), -1);
  if (type == LUA_TSTRING) {
    const char* str = lua_tostring(m_state(), -1);
    cmd.host = std::string(str);
  } else if (type != LUA_TNIL) {
    throw std::runtime_error{"Bad type of cmd.host value!"};
  }

  lua_pushstring(m_state(), "command");
  lua_gettable(m_state(), -2);
  type = lua_type(m_state(), -1);
  if (type == LUA_TSTRING) {
    const char* str = lua_tostring(m_state(), -1);
    cmd.command = std::string(str);
  } else if (type != LUA_TNIL) {
    throw std::runtime_error{"Bad type of cmd.command value!"};
  }

  lua_pushstring(m_state(), "params");
  lua_gettable(m_state(), -2);
  type = lua_type(m_state(), -1);
  if (type == LUA_TTABLE) {

    // pop params table
    // problem with reading length of Lua Table
    
  } else if (type != LUA_TNIL) {
    throw std::runtime_error{"Bad type of cmd.nick value!"};
  }
}
