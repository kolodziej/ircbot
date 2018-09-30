#ifndef _IRCBOT_DEPENDENCY_CONTAINER_HPP
#define _IRCBOT_DEPENDENCY_CONTAINER_HPP

#include <memory>
#include <vector>

#include "ircbot/irc_message.hpp"
#include "ircbot/logger.hpp"

namespace ircbot {

/* \class DependencyContainer
 *
 * \brief Base class for all everything that can act as an input for other
 * plugins
 *
 * Plugins that should receive output from something that inherits this class
 * should register in it.
 */

class Plugin;

class DependencyContainer {
 public:
  DependencyContainer() = default;
  DependencyContainer(const DependencyContainer&) = default;
  DependencyContainer(DependencyContainer&&) = default;
  DependencyContainer& operator=(const DependencyContainer&) = default;
  DependencyContainer& operator=(DependencyContainer&&) = default;

  /** Add plugin - register plugin to which this should output
   *
   * \param plugin shared pointer to plugin that should be added
   */
  void addDependentPlugin(std::shared_ptr<Plugin> plugin);

  /** Remove plugin
   *
   * \param plugin shared pointer to plugin that should be removed
   * \return returns false if plugin didn't exist in OutputPlugin
   */
  bool removeDependentPlugin(std::shared_ptr<Plugin> plugin);

  /** Output message to all inputs
   *
   * \param message message that should be sent to all inputs
   */
  void outputMessage(const IRCMessage& message);

 private:
  std::vector<std::shared_ptr<Plugin>> m_inputs;
};

}  // namespace ircbot

#endif
