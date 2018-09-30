#include "ircbot/output_plugin.hpp"

#include "ircbot/plugin.hpp"

#include <algorithm>

namespace ircbot {

void OutputPlugin::addOutputPlugin(std::shared_ptr<Plugin> plugin) {
  m_inputs.push_back(plugin);
}

bool OutputPlugin::removeOutputPlugin(std::shared_ptr<Plugin> plugin) {
  auto it = std::find(m_inputs.begin(), m_inputs.end(), plugin);
  if (it != m_inputs.end()) {
    m_inputs.erase(it);
    return true;
  }

  return false;
}

void OutputPlugin::outputMessage(const IRCMessage& message) {
  for (auto& plugin : m_inputs) {
    if (not plugin->preFilter(message)) {
      DEBUG("Plugin ", plugin->getName(), " pre-filtering not passed",
            " for message: ", message.toString(true));
      continue;
    }

    if (plugin->filter(message)) {
      DEBUG("Plugin ", plugin->getName(),
            " filtering passed for message: ", message.toString(true));
      plugin->receive(message);
    }
  }
}

}  // namespace ircbot
