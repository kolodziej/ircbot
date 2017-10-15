#include "ircbot/ping_plugin.hpp"

#include "ircbot/logger.hpp"

PingPlugin::PingPlugin(PluginManager& manager) :
    Plugin{manager, "Ping"}
{}

void PingPlugin::run() {
  auto cmd = getCommand();
  IRCCommand response{
    "PONG",
    { cmd.params.back() }
  };
  send(response);
}

bool PingPlugin::filter(const IRCCommand& cmd) {
  return (cmd.command == "PING");
}
