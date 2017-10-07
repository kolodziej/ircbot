#include "ircbot/plugin.hpp"

Plugin::Plugin(std::string name) :
    m_name{name}
{}

std::string Plugin::name() const {
  return m_name;
}
