#ifndef _CLIENT_HPP
#define _CLIENT_HPP

#include <boost/asio.hpp>
#include <string>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <array>

#include "ircbot/irc_parser.hpp"
#include "ircbot/irc_command.hpp"
#include "ircbot/plugin_manager.hpp"
#include "ircbot/config.hpp"
#include "ircbot/logger.hpp"

namespace asio = boost::asio;

class Client {
 public:
  Client(asio::io_service& io_service, Config cfg);

  void init();

  void connect(std::string host, uint16_t port);
  void disconnect();

  void startAsyncReceive();
  void stopAsyncReceive();

  void send(IRCCommand cmd);
  void send(std::string msg);

  void spawn();
  void signal(int);

  PluginManager& pluginManager();

 private:
  void sendLoop();
  void parserLoop();

  asio::io_service& m_io_service;
  asio::ip::tcp::socket m_socket;

  IRCParser m_parser;
  std::array<char, 4096> m_buffer;

  Config m_cfg;

  /* Objects needed to run in separate thread */
  std::thread m_plugin_thread;
  std::thread m_parser_thread;
  std::atomic<bool> m_running;

  PluginManager m_plugins;
};

#endif
