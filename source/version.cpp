#include "ircbot/version.hpp"

#include <sstream>

namespace version {

std::string str() {
  std::stringstream stream;
  stream << "v" << major << '.' << minor << '.' << patch;
  stream << " (git-commit " << git_commit << "; branch " << git_branch << ')';

  return stream.str();
}

}
