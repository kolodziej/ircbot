#ifndef _PYIRCBOT_PLUGIN_HPP
#define _PYIRCBOT_PLUGIN_HPP

#include <functional>
#include <pybind11/pybind11.h>

#include "ircbot/irc_message.hpp"
#include "ircbot/command_parser.hpp"

namespace py = pybind11;

struct Plugin {
  std::string name;
  std::string token;

  std::function<void()> onInit;
  std::function<void(IRCMessage)> onMessage;
  std::function<void(CommandParser::Command)> onCommand;
  std::function<void()> onShutdown;
  std::function<void()> onRestart;
  std::function<void()> onReload;
};

#endif
