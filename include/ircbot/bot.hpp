#ifndef _BOT_HPP
#define _BOT_HPP

#include "ircbot/bot_config.hpp"

#include <mutex>
#include <condition_variable>
#include <deque>
#include <string>
#include <atomic>

#include "ircbot/session.hpp"

class Bot {
 public:
  Bot(Session& session);

  void run_receiver();
  void run_sender();
  void run_interpreter();

 private:
  Session& m_session;

  std::deque<std::string> m_received;
  std::condition_variable m_received_cv;
  std::mutex m_received_mtx;

  std::deque<std::string> m_outgoing;
  std::condition_variable m_outgoing_cv;
  std::mutex m_outgoing_mtx;

  std::atomic<bool> m_running;
};

#endif
