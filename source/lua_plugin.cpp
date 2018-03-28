#include "ircbot/lua_plugin.hpp"

LuaPlugin::LuaPlugin(PluginConfig config) :
    Plugin{config} {
  m_lua.open_libraries(sol::lib::base);
}

std::string LuaPlugin::getName() const {
  return "LuaPlugin";
}

void LuaPlugin::onInit() {
  m_lua.script("print('Hello world')");
}

void LuaPlugin::onMessage(IRCMessage msg) {

}

void LuaPlugin::onNewConfiguration() {

}

void LuaPlugin::onShutdown() {

}
