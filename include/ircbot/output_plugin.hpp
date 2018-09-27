#ifndef _IRCBOT_OUTPUT_PLUGIN_HPP
#define _IRCBOT_OUTPUT_PLUGIN_HPP

#include <memory>
#include <vector>

#include "ircbot/plugin.hpp"

namespace ircbot {

/* \class OutputPlugin
 *
 * \brief Base class for all everything that can act as an input for other
 * plugins
 *
 * Plugins that should receive output from something that inherits this class
 * should register in it.
 */

class OutputPlugin {
 public:
  OutputPlugin() = default;
  OutputPlugin(const OutputPlugin&) = default;
  OutputPlugin(OutputPlugin&&) = default;
  OutputPlugin& operator=(const OutputPlugin&) = default;
  OutputPlugin& operator=(OutputPlugin&&) = default;

  /** Add plugin - register plugin to which this should output
   *
   * \param plugin shared pointer to plugin that should be added
   */
  void addPlugin(std::shared_ptr<Plugin> plugin);

  /** Remove plugin
   *
   * \param plugin shared pointer to plugin that should be removed
   * \return returns false if plugin didn't exist in OutputPlugin
   */
  bool removePlugin(std::shared_ptr<Plugin> plugin);

 protected:
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
