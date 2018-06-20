#ifndef _HELPERS_HPP
#define _HELPERS_HPP

#include <chrono>
#include <functional>
#include <string>
#include <thread>

namespace ircbot {
namespace helpers {

std::string removeLineFeed(const std::string& str);

void setThreadName(std::thread& thread, const std::string& name);

bool startsWith(const std::string& str, const std::string& start);

bool isIn(char c, const std::string& s);

bool retry(std::function<bool()> func, uint32_t timeout, uint32_t trials,
           double timeout_factor = 1.0);

template <typename Endpoint>
std::string endpointAddress(Endpoint endpoint);

}  // namespace helpers
}  // namespace ircbot

#include "ircbot/helpers.impl.hpp"

#endif
