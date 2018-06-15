#ifndef _VERSION_HPP
#define _VERSION_HPP

#include "version_configuration.hpp"

#include <vector>
#include <map>
#include <string>

class Plugin;

namespace ircbot {
namespace version {

static const uint16_t
  major = VERSION_MAJOR,
  minor = VERSION_MINOR,
  patch = VERSION_PATCH;

std::string str();

} } // namespace ircbot::version

#endif
