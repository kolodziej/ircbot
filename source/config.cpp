#include "ircbot/config.hpp"

#include <fstream>
#include <stdexcept>

namespace ircbot {

Config::Config(const std::string& fname) : m_fname{fname} { loadFile(); }

void Config::loadFile() {
  if (m_fname.empty()) {
    throw std::logic_error("Could not load! Config's filename is empty!");
  }

  *this = YAML::LoadFile(m_fname);
}

void Config::loadFile(const std::string& fname) {
  m_fname = fname;
  loadFile();
}

void Config::saveFile() {
  if (m_fname.empty()) {
    throw std::logic_error("Could not save! Config's filename is empty!");
  }

  YAML::Emitter emitter;
  emitter << *this;

  std::fstream f{m_fname, std::ios::out};
  f << emitter.c_str();
}

void Config::saveFile(const std::string& fname) {
  m_fname = fname;
  saveFile();
}

}  // namespace ircbot
