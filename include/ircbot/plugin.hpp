#ifndef _PLUGIN_HPP
#define _PLUGIN_HPP

#include <memory>
#include <deque>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "ircbot/plugin_manager.hpp"
#include "ircbot/irc_command.hpp"
#include "ircbot/config.hpp"
#include "ircbot/logger.hpp"

#define IRCBOT_PLUGIN(PluginName) \
    extern "C" std::unique_ptr<Plugin> getPlugin(PluginManager* manager) { \
      return std::make_unique<PluginName>(*manager); \
    }

class Plugin {
 public:
  Plugin(PluginManager& manager, const std::string& name);
  virtual ~Plugin();

  std::string name() const;
  void stop();

  void receive(IRCCommand cmd);

  virtual void run() = 0;
  virtual bool filter(const IRCCommand& cmd);

  void setConfig(Config cfg);
  Config& getConfig();
  const Config& getConfig() const;
  void spawn();

 protected:
  size_t commandsCount() const;
  IRCCommand getCommand();
  void send(const IRCCommand& cmd);
  pt::ptree& cfg();

 private:
  PluginManager& m_manager;
  std::string m_name;
  Config m_cfg;

  std::atomic<bool> m_running;

  std::deque<IRCCommand> m_incoming;
  std::mutex m_incoming_mtx;
  std::condition_variable m_incoming_cv;

  std::thread m_thread;
};

#endif
