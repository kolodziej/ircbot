#include "ircbot/plugin.hpp"

#include <chrono>

#include "ircbot/helpers.hpp"

Plugin::Plugin(Client& client, const std::string& id) :
    m_client{client},
    m_id{id},
    m_command_parser{nullptr},
    m_running{true}
{}

Plugin::~Plugin() {
  onShutdown();

  if (m_thread.joinable())
    m_thread.join();
}

std::string Plugin::getId() const {
  return m_id;
}

void Plugin::stop() {
  LOG(INFO, "Stopping plugin: ", getName());
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

  onInit();

  while (isRunning()) {
    std::unique_lock<std::mutex> lock{m_incoming_mtx};
    if (m_incoming_cv.wait_for(lock, 500ms,
                               [this] { return commandsCount(); })) {
      IRCCommand cmd = getCommand();    
      onMessage(cmd);
    }
  }
}

void Plugin::onCommand(CommandParser::Command cmd) {

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
  onNewConfiguration();
}

Config& Plugin::getConfig() {
  return m_cfg;
}

const Config& Plugin::getConfig() const {
  return m_cfg;
}

void Plugin::spawn() {
  auto plugin_call = [this] {
    try {
      run();
    } catch (std::exception& exc) {
      LOG(ERROR, "Plugin ", getName(), " caused an exception: ", exc.what());
    }
  };
  m_thread = std::thread(plugin_call);
  helpers::setThreadName(m_thread, getName());
}

void Plugin::installCommandParser(std::shared_ptr<CommandParser> parser) {
  LOG(INFO, "Installing new command parser in plugin ", getId());
  m_command_parser = parser;
}

bool Plugin::hasCommandParser() const {
  return m_command_parser != nullptr;
}

void Plugin::deinstallCommandParser() {
  LOG(INFO, "Removing command parser from plugin ", getId());
  m_command_parser = nullptr;
}

void Plugin::addFunction(const std::string& cmd, CmdFunction f) {
  if (m_functions.count(cmd)) {
    LOG(WARNING, "Overwriting function for command: ", cmd);
  }

  m_functions[cmd] = f;
}

void Plugin::removeFunction(const std::string& cmd) {
  size_t removed = m_functions.erase(cmd);
  if (removed == 0) {
    LOG(WARNING, "Such function (", cmd, ") doesn't exist.");
  }
}

void Plugin::callFunction(const CommandParser::Command& command) {
  if (m_functions.count(command.command)) {
    m_functions[command.command](command);
  } else {
    LOG(INFO, "Function for such command doesn't exist: ", command.command);
  }
}
