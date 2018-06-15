#ifndef _HELPERS_HPP
#define _HELPERS_HPP

#include <string>
#include <thread>

namespace ircbot {
namespace helpers {

std::string removeLineFeed(const std::string& str);

void setThreadName(std::thread& thread, const std::string& name);

bool startsWith(const std::string& str, const std::string& start);

bool isIn(char c, const std::string& s);

}  // namespace helpers
}  // namespace ircbot

#endif
