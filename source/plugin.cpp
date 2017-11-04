#include "ircbot/plugin.hpp"

#include <chrono>

#include "ircbot/helpers.hpp"

Plugin::Plugin(Client& client, std::string name) :
    m_client{client},
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

void Plugin::run() {
  using namespace std::literals::chrono_literals;
  while (isRunning()) {
    std::unique_lock<std::mutex> lock{m_incoming_mtx};
    if (m_incoming_cv.wait_for(lock, 500ms,
                               [this] { return commandsCount(); })) {
      IRCCommand cmd = getCommand();    
      onMessage(cmd);
    }
  }
}

bool Plugin::isRunning() const {
  return m_running;
}

size_t Plugin::commandsCount() const {
  return m_incoming.size();
}

IRCCommand Plugin::getCommand() {
  auto cmd = m_incoming.front();
  m_incoming.pop_front();
  return cmd;
}

void Plugin::send(const IRCCommand& cmd) {
  m_client.send(cmd);
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
  m_thread = std::thread([this] { run(); });
  helpers::setThreadName(m_thread, name());
}
