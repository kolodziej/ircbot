#ifndef _PLUGIN_HPP
#define _PLUGIN_HPP

#include <memory>

class Plugin {
 public:
  using Ptr = std::shared_ptr<Plugin>;

  Plugin() = default;
};

#endif
