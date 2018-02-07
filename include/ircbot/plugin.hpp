#ifndef _PLUGIN_HPP
#define _PLUGIN_HPP

#include <memory>
#include <deque>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "ircbot/client.hpp"
#include "ircbot/irc_command.hpp"
#include "ircbot/config.hpp"
#include "ircbot/logger.hpp"

#define IRCBOT_PLUGIN(PluginName) \
    extern "C" std::unique_ptr<Plugin> getPlugin(Client* client) { \
      return std::make_unique<PluginName>(*client); \
    }

class Plugin {
 public:
  Plugin(Client& client, std::string name);
  ~Plugin();

  std::string name() const;
  void stop();

  void receive(IRCCommand cmd);

  virtual void onInit() {}
  virtual void run();
  virtual void onMessage(IRCCommand) = 0;
  virtual void onNewConfiguration() {}
  virtual bool filter(const IRCCommand& cmd);
  virtual void onShutdown() {}

  void setConfig(Config cfg);
  Config& getConfig();
  const Config& getConfig() const;
  void spawn();

 protected:
  bool isRunning() const;
  size_t commandsCount() const;
  IRCCommand getCommand();
  void send(const IRCCommand& cmd);
  pt::ptree& cfg();

 private:
  Client& m_client;
  std::string m_name;
  Config m_cfg;

  std::atomic<bool> m_running;

  std::deque<IRCCommand> m_incoming;
  std::mutex m_incoming_mtx;
  std::condition_variable m_incoming_cv;

  std::thread m_thread;
};

#endif
