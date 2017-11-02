#include "ircbot/plugin.hpp"

#include "ircbot/helpers.hpp"

Plugin::Plugin(PluginManager& manager, const std::string& name) :
    m_manager{manager},
    m_name{name},
    m_running{true}
{}

Plugin::~Plugin() {
  if (m_thread.joinable())
    m_thread.join();
}

std::string Plugin::name() const {
  return m_name;
}

void Plugin::stop() {
  m_running = false;
}

void Plugin::receive(IRCCommand cmd) {
  std::unique_lock<std::mutex> lock{m_incoming_mtx};
  m_incoming.push_back(cmd); 
  DEBUG("New message added to plugin's incoming queue");
  lock.unlock();

  m_incoming_cv.notify_all();
}

size_t Plugin::commandsCount() const {
  return m_incoming.size();
}

IRCCommand Plugin::getCommand() {
  std::unique_lock<std::mutex> lock{m_incoming_mtx};
  m_incoming_cv.wait(lock, [this] { return commandsCount() > 0; });
  auto cmd = m_incoming.front();
  m_incoming.pop_front();
  lock.unlock();

  return cmd;
}

void Plugin::send(const IRCCommand& cmd) {
  m_manager.addOutgoing(cmd);
}

pt::ptree& Plugin::cfg() {
  return m_cfg.tree();
}

bool Plugin::filter(const IRCCommand& cmd) {
  return true;
}

void Plugin::setConfig(Config cfg) {
  m_cfg = cfg;
}

Config& Plugin::getConfig() {
  return m_cfg;
}

const Config& Plugin::getConfig() const {
  return m_cfg;
}

void Plugin::spawn() {
  auto loopFunction = [this] {
    while (m_running) {
      run();
    }
  };
  m_thread = std::thread(loopFunction);
  helpers::setThreadName(m_thread, name());
}
