#include "ircbot/version.hpp"

#include <sstream>

namespace version {

std::string str() {
  std::stringstream stream;
  stream << "v" << major << '.' << minor << '.' << patch;
  stream << " (git-commit " << git_commit << "; branch " << git_branch << ')';

  return stream.str();
}

std::vector<std::string> getBuiltinPluginsList() {
  std::vector<std::string> plugins;
  constexpr const size_t len = sizeof(builtin_plugins) / sizeof(char*);
  for (size_t i = 0; i < len; ++i) {
    plugins.emplace_back(builtin_plugins[i]);
  }

  return plugins;
}

}
