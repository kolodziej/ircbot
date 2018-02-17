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
#include "ircbot/irc_message.hpp"
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

  void receive(IRCMessage cmd);

  virtual void onInit() {}
  virtual void run();
  virtual void onMessage(IRCMessage) {}
  virtual void onNewConfiguration() {}
  virtual bool filter(const IRCMessage& cmd);
  virtual void onShutdown() {}

  bool preFilter(const IRCMessage& cmd);

  void installCommandParser(std::shared_ptr<CommandParser> parser);
  bool hasCommandParser() const;
  void deinstallCommandParser();
  bool isCommand(IRCMessage cmd) const;
  void onCommand(CommandParser::Command cmd);

  void setConfig(Config cfg);
  Config& getConfig();
  const Config& getConfig() const;
  void spawn();

 protected:
  bool isRunning() const;
  size_t commandsCount() const;
  IRCMessage getCommand();
  void send(const IRCMessage& msg);
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

  std::deque<IRCMessage> m_incoming;
  std::mutex m_incoming_mtx;
  std::condition_variable m_incoming_cv;

  std::thread m_thread;
};

#endif
