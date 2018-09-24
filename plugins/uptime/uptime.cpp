#include "uptime.hpp"
#include "ircbot/helpers.hpp"

#include <stdexcept>

using namespace ircbot;

Uptime::Uptime(PluginConfig config) : SoPlugin{config} {}

std::string Uptime::getName() const { return "HelloWorld"; }

void Uptime::onMessage(IRCMessage cmd) {
  using namespace std::chrono;
  using helpers::responseDestination;

  auto uptime = core()->getUptime();

  uint32_t d =
      duration_cast<duration<uint32_t, std::ratio<86400>>>(uptime).count();
  uptime -= duration<uint32_t, std::ratio<86400>>(d);
  uint32_t h = duration_cast<hours>(uptime).count();
  uptime -= hours(h);
  uint32_t m = duration_cast<minutes>(uptime).count();
  uptime -= minutes(m);
  uint32_t s = duration_cast<seconds>(uptime).count();
  uptime -= seconds(s);

  std::stringstream str;
  const char color{0x03}, reset{0x0F};
  str << color << "03,01[Uptime = " << d << " days " << h << "h " << m << "m "
      << s << "s]" << reset;

  IRCMessage response{"PRIVMSG", {responseDestination(cmd), str.str()}};

  send(response);
}

bool Uptime::filter(const IRCMessage& cmd) {
  using helpers::startsWith;
  return (cmd.command == "PRIVMSG" and startsWith(cmd.params[1], "!uptime"));
}
