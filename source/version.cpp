#include "ircbot/version.hpp"

#include <sstream>

namespace ircbot { namespace version {

static const char* git_branch = GIT_BRANCH;
static const char* git_commit = GIT_COMMIT;

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

} } // namespace ircbot::version
