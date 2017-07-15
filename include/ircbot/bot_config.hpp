#ifndef _BOT_CONFIG_HPP
#define _BOT_CONFIG_HPP

#include <string>
#include <vector>
#include <cstdint>

struct BotConfig {
  std::string irc_server;
  uint16_t irc_port;

  std::string cli_address;
  uint16_t cli_port;

  std::vector<std::string> plugins;
};

#endif
