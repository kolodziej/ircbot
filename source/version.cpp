#include "ircbot/version.hpp"

#include <sstream>

namespace version {

std::string str() {
  std::stringstream stream;
#ifdef RELEASE_NAME
  stream << RELEASE_NAME << ' ';
#endif
  stream << "v" << major << '.' << minor << '.' << patch << ' ';
#ifdef RELEASE_TYPE
  stream << '[' << RELEASE_TYPE << "] ";
#endif
  stream << "(git-commit " << git_commit << "; git-branch " << git_branch << ')';

  return stream.str();
}

}
