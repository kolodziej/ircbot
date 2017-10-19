#ifndef _VERSION_HPP
#define _VERSION_HPP

#include "version_configuration.hpp"

#include <vector>
#include <map>
#include <string>

class Plugin;

namespace version {

static const uint16_t
  major = VERSION_MAJOR,
  minor = VERSION_MINOR,
  patch = VERSION_PATCH;

static const char* git_branch = GIT_BRANCH;
static const char* git_commit = GIT_COMMIT;

std::string str();

}
#endif
