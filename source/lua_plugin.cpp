#include "ircbot/lua_plugin.hpp"

LuaPlugin::LuaPlugin(PluginManager& manager, const std::string& name) :
    Plugin{manager, name} {
}
