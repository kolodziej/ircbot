#include "ircbot/helpers.hpp"

namespace helpers {

std::string removeLineFeed(const std::string& str) {
  static const char cr = 0xd;
  static const char lf = 0xa;

  size_t len = str.size();
  if (str.back() == lf) {
    --len;
    if (str[len - 1] == cr) {
      --len;
    }
  }

  return str.substr(0, len);
}

}
