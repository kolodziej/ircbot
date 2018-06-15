#ifndef _VERSION_HPP
#define _VERSION_HPP

#include "version_configuration.hpp"

#include <map>
#include <string>
#include <vector>

class Plugin;

namespace ircbot {
namespace version {

static const uint16_t major = VERSION_MAJOR, minor = VERSION_MINOR,
                      patch = VERSION_PATCH;

std::string str();

}  // namespace version
}  // namespace ircbot

#endif
