#ifndef _PYIRCBOT_PLUGIN_HPP
#define _PYIRCBOT_PLUGIN_HPP

#include <pybind11/pybind11.h>

#include <functional>

namespace py = pybind11;

struct Plugin {
  std::function<void(const std::string& msg)> onMessageCallback;
};

#endif
