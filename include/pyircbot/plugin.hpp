#ifndef _PYIRCBOT_PLUGIN_HPP
#define _PYIRCBOT_PLUGIN_HPP

#include <functional>
#include <pybind11/pybind11.h>

#include "ircbot/irc_message.hpp"

namespace py = pybind11;

namespace pyircbot {

class Bot;

struct Plugin {
  std::string name;
  std::string token;

  std::function<void(Bot*)> onInit;
  std::function<void(Bot*, IRCMessage)> onMessage;
  std::function<void(Bot*)> onShutdown;
  std::function<void(Bot*)> onRestart;
  std::function<void(Bot*)> onReload;
};

} // namespace pyircbot

#endif
