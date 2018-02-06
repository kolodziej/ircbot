#include "ircbot/utils.hpp"

namespace utils {

bool startsWith(const std::string& str, const std::string& start) {
  return str.substr(0, start.size()) == start;
}

}
