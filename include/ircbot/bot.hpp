#ifndef _BOT_HPP
#define _BOT_HPP

#include "ircbot/bot_config.hpp"

#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <atomic>

#include "ircbot/session.hpp"
#include "ircbot/bot_config.hpp"
#include "ircbot/plugin_manager.hpp"

class Bot {
 public:
  Bot(Session&, const BotConfig&);

  void run_receiver();
  void run_sender();
  void run_interpreter();
  void run_responder();

 private:
  Session& m_session;
  BotConfig m_config;
  PluginManager m_plugin_manager;

  std::queue<std::string> m_received;
  std::condition_variable m_received_cv;
  std::mutex m_received_mtx;

  std::queue<std::string> m_outgoing;
  std::condition_variable m_outgoing_cv;
  std::mutex m_outgoing_mtx;

  std::atomic<bool> m_running;
};

#endif
