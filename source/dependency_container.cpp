#include "ircbot/dependency_container.hpp"

#include "ircbot/plugin.hpp"

#include <algorithm>

namespace ircbot {

void DependencyContainer::addDependentPlugin(std::shared_ptr<Plugin> plugin) {
  m_inputs.push_back(plugin);
}

bool DependencyContainer::removeDependentPlugin(
    std::shared_ptr<Plugin> plugin) {
  auto it = std::find(m_inputs.begin(), m_inputs.end(), plugin);
  if (it != m_inputs.end()) {
    m_inputs.erase(it);
    return true;
  }

  return false;
}

void DependencyContainer::outputMessage(const IRCMessage& message) {
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
