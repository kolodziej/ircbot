#ifndef _PLUGIN_HPP
#define _PLUGIN_HPP

#include <memory>
#include <deque>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <unordered_map>

#include "ircbot/client.hpp"
#include "ircbot/plugin_config.hpp"
#include "ircbot/irc_command.hpp"
#include "ircbot/command_parser.hpp"
#include "ircbot/config.hpp"
#include "ircbot/logger.hpp"

class Plugin {
  using CmdFunction = std::function<void(const CommandParser::Command&)>;

 public:
  Plugin(PluginConfig config);
  ~Plugin();

  std::string getId() const;
  virtual std::string getName() const = 0;
  void stop();

  void receive(IRCCommand cmd);

  virtual void onInit() {}
  virtual void run();
  virtual void onMessage(IRCCommand) = 0;
  virtual void onCommand(CommandParser::Command cmd);
  virtual void onNewConfiguration() {}
  virtual bool filter(const IRCCommand& cmd);
  virtual void onShutdown() {}

  void setConfig(Config cfg);
  Config& getConfig();
  const Config& getConfig() const;
  void spawn();

  void installCommandParser(std::shared_ptr<CommandParser> parser);
  bool hasCommandParser() const;
  void deinstallCommandParser();

 protected:
  bool isRunning() const;
  size_t commandsCount() const;
  IRCCommand getCommand();
  void send(const IRCCommand& cmd);
  pt::ptree& cfg();

  void addFunction(const std::string& cmd, CmdFunction);
  void removeFunction(const std::string& cmd);
  void callFunction(const CommandParser::Command& command);

 private:
  std::shared_ptr<Client> m_client;
  const std::string m_id;
  Config m_cfg;

  std::shared_ptr<CommandParser> m_command_parser;
  std::unordered_map<std::string, CmdFunction> m_functions;

  std::atomic<bool> m_running;

  std::deque<IRCCommand> m_incoming;
  std::mutex m_incoming_mtx;
  std::condition_variable m_incoming_cv;

  std::thread m_thread;
};

#endif
