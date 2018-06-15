#ifndef _PYIRCBOT_PLUGIN_HPP
#define _PYIRCBOT_PLUGIN_HPP

#include <pybind11/pybind11.h>
#include <functional>

#include "ircbot/irc_message.hpp"

namespace py = pybind11;

namespace pyircbot {

class Bot;

/** \class Plugin
 *
 * Struct that contains plugin's name, access token, and set of callbacks which
 * are used to inform plugin about different events.
 */

struct Plugin {
  /** Plugin's name */
  std::string name;

  /** Plugin's access token */
  std::string token;

  /** Function called after successful plugin initialization */
  std::function<void(Bot*)> onInit;

  /** Function called when new IRC message is received */
  std::function<void(Bot*, ircbot::IRCMessage)> onMessage;

  /** Function called on plugin shutdown caused by IRCBot */
  std::function<void(Bot*)> onShutdown;

  /** Function that should restart plugin */
  std::function<void(Bot*)> onRestart;

  /** Function that should reload plugin */
  std::function<void(Bot*)> onReload;
};

}  // namespace pyircbot

#endif
