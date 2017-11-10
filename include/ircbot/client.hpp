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
#include "ircbot/config.hpp"
#include "ircbot/logger.hpp"

class Plugin;

namespace asio = boost::asio;

class Client {
 public:
  Client(asio::io_service& io_service, Config cfg);

  void connect(std::string host, uint16_t port);
  void initializePlugins();
  void disconnect();

  void startAsyncReceive();
  void stopAsyncReceive();

  void send(IRCCommand cmd);
  void send(std::string msg);

  void run();
  void signal(int);

  void addPlugin(std::unique_ptr<Plugin>&& plugin);
  void removePlugin(const std::string& name);
  std::vector<std::string> listPlugins() const;
  std::unique_ptr<Plugin> loadSoPlugin(const std::string& fname);

  void startPlugins();
  void stopPlugins();

 private:
  asio::io_service& m_io_service;
  asio::ip::tcp::socket m_socket;

  IRCParser m_parser;
  std::array<char, 4096> m_buffer;

  Config m_cfg;

  std::atomic_bool m_running;
  std::vector<std::unique_ptr<Plugin>> m_plugins;
  std::mutex m_plugins_mtx;
};

#endif
