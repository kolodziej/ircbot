#include "ircbot/helpers.hpp"

#include <pthread.h>

#include "ircbot/logger.hpp"

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

void setThreadName(std::thread& thread, const std::string& name) {
    auto native = thread.native_handle();
    pthread_setname_np(native, name.data());
    LOG(INFO, "Change thread ", std::this_thread::get_id(),
              " name to ", name);
}

bool startsWith(const std::string& str, const std::string& start) {
  return str.substr(0, start.size()) == start;
}

bool isIn(char c, const std::string& s) {
  return s.find(c) != std::string::npos;
}

}
