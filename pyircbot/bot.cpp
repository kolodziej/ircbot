#include "pyircbot/bot.hpp"

#include "ircbot/version.hpp"

Bot::Bot(const std::string& hostname, uint16_t port) :
  m_hostname{hostname},
  m_port{port} {
}

std::string Bot::hostname() const {
  return m_hostname;
}

uint16_t Bot::port() const {
  return m_port;
}
