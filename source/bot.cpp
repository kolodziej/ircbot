#include "ircbot/bot.hpp"

#include "ircbot/session.hpp"
#include "ircbot/irc_interpreter.hpp"
#include "ircbot/logger.hpp"

Bot::Bot(Session& session) :
  m_session{session},
  m_running{true}
{}

void Bot::run_receiver() {
  Logger& logger = Logger::getInstance();

  while (m_running) {
    logger(LogLevel::DEBUG, "Waiting for new message...");
    std::string message = m_session.receive();
    logger(LogLevel::DEBUG, "Message of length ", message.size(), " received.");

    m_received_mtx.lock();
    m_received.push_back(message);
    m_received_mtx.unlock();
    m_received_cv.notify_all();

    logger(LogLevel::DEBUG, "Received message: ", message);
  }
}

void Bot::run_sender() {
  Logger& logger = Logger::getInstance();

  while (m_running) {
    std::unique_lock<std::mutex> lock{m_outgoing_mtx};
    m_outgoing_cv.wait(lock, [this]() { return not this->m_outgoing.empty(); });

    auto message = m_outgoing.front();
    m_outgoing.pop_front();
    lock.unlock();
    m_outgoing_cv.notify_all();

    m_session.send(message);
  }
}

void Bot::run_interpreter() {
  Logger& logger = Logger::getInstance();
  
  IRCInterpreter interpreter;
  while (m_running) {
    std::unique_lock<std::mutex> rlock{m_received_mtx};
    m_received_cv.wait(rlock, [this]() { return not this->m_received.empty(); });

    auto message = m_received.front();
    m_received.pop_front();
    rlock.unlock();
    m_received_cv.notify_all();

    logger(LogLevel::DEBUG, "Interpreting: ", message);
    auto x = interpreter.run(message);
    logger(LogLevel::DEBUG, "Command: ", x.command, "; params: ", x.params);

    m_outgoing_mtx.lock();
    m_outgoing.push_back(message);
    m_outgoing_mtx.unlock();

    m_outgoing_cv.notify_all();
  }
}
