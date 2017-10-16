#ifndef _HELPERS_HPP
#define _HELPERS_HPP

#include <thread>
#include <string>

namespace helpers {

std::string removeLineFeed(const std::string& str);

void setThreadName(std::thread& thread, const std::string& name);

}

#endif
