#ifndef _PLUGIN_CONFIG_HPP
#define _PLUGIN_CONFIG_HPP

#include <memory>
#include <string>

#include "ircbot/config.hpp"

class Client;

struct PluginConfig {
  std::shared_ptr<Client> client;
  std::string id;
  Config config;
};

#endif
