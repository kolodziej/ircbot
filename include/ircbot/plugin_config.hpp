#ifndef _PLUGIN_CONFIG_HPP
#define _PLUGIN_CONFIG_HPP

#include <memory>
#include <string>

#include "ircbot/config.hpp"

class Client;

/** \class PluginConfig 
 *
 * \brief Plugin configuration for plugins loaded from shared object files.
 *
 * Keeps base data needed to load plugin from shared object:
 *   - pointer to client instance,
 *   - plugin's id
 *   - plugin's configuration
 */

struct PluginConfig {
  /** pointer to Client instance */
  std::shared_ptr<Client> client;

  /** plugin id - unique in Client's scope */
  std::string id;

  /** plugin configuration - actual configuration of plugin */
  Config config;
};

#endif
