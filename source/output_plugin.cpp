#include "ircbot/output_plugin.hpp"

#include <algorithm>

namespace ircbot {

void OutputPlugin::addPlugin(std::shared_ptr<Plugin> plugin) {
  m_inputs.push_back(plugin);
}

bool OutputPlugin::removePlugin(std::shared_ptr<Plugin> plugin) {
  auto it = std::find(m_inputs.begin(), m_inputs.end(), plugin);
  if (it != m_inputs.end()) {
    m_inputs.erase(it);
    return true;
  }

  return false;
}

void OutputPlugin::outputMessage(const IRCMessage& message) {
  for (auto& plugin : m_inputs) {
    plugin->receive(message);
  }
}

}  // namespace ircbot
