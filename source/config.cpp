#include "ircbot/config.hpp"

#include <stdexcept>

namespace ircbot {

Config::Config(const std::string& fname) : m_fname{fname} { loadFile(); }

Config::Config(const pt::ptree& pt) : m_pt{pt} {}

void Config::loadFile() {
  if (m_fname.empty()) {
    throw std::logic_error("Could not load! Config's filename is empty!");
  }

  pt::read_json(m_fname, m_pt);
}

void Config::loadFile(const std::string& fname) {
  m_fname = fname;
  loadFile();
}

void Config::saveFile() {
  if (m_fname.empty()) {
    throw std::logic_error("Could not save! Config's filename is empty!");
  }

  pt::write_json(m_fname, m_pt);
}

void Config::saveFile(const std::string& fname) {
  m_fname = fname;
  saveFile();
}

pt::ptree& Config::tree() { return m_pt; }

}  // namespace ircbot
