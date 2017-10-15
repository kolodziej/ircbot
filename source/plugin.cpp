#include "ircbot/plugin.hpp"

Plugin::Plugin(PluginManager& manager, std::string name) :
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
  Logger& logger = Logger::getInstance();

  std::unique_lock<std::mutex> lock{m_incoming_mtx};
  m_incoming.push_back(cmd); 
  logger(LogLevel::DEBUG, "New message added to plugin's incoming queue");
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

bool Plugin::filter(const IRCCommand& cmd) {
  return true;
}

void Plugin::spawn() {
  auto loopFunction = [this] {
    while (m_running) {
      run();
    }
  };
  m_thread = std::thread(loopFunction);
}
