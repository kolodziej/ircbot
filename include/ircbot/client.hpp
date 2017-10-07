#ifndef _CLIENT_HPP
#define _CLIENT_HPP

#include <boost/asio.hpp>
#include <string>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>

#include "ircbot/irc_parser.hpp"
#include "ircbot/irc_command.hpp"
#include "ircbot/logger.hpp"

namespace asio = boost::asio;

class Client {
 public:
  Client(asio::io_service& io_service);
  Client(asio::io_service& io_service, std::string host, uint16_t port);

  void connect(std::string host, uint16_t port);
  void disconnect();

  void send(IRCCommand cmd);
  void send(std::string msg);

  void spawn();

 private:
  void pluginLoop();
  void parserLoop();

  asio::io_service& m_io_service;
  asio::ip::tcp::socket m_socket;
  IRCParser m_parser;

  /* Objects needed to run in separate thread */
  std::thread m_plugin_thread;
  std::thread m_parser_thread;
  std::atomic<bool> m_running;

  /* Logger reference */
  Logger& m_logger;
};

#endif
