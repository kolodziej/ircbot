#ifndef _CLIENT_HPP
#define _CLIENT_HPP

#include <boost/asio.hpp>
#include <string>
#include <memory>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <array>
#include <map>

#include "ircbot/irc_parser.hpp"
#include "ircbot/irc_message.hpp"
#include "ircbot/config.hpp"
#include "ircbot/plugin_config.hpp"
#include "ircbot/admin_port.hpp"
#include "ircbot/logger.hpp"

class Plugin;
class SoPlugin;

namespace asio = boost::asio;

class Client : public std::enable_shared_from_this<Client> {
 public:
  Client(asio::io_service& io_service, Config cfg);

  void connect();
  void initializePlugins();
  void disconnect();

  void startAsyncReceive();
  void stopAsyncReceive();

  void startAdminPort(const std::string& socket_path);
  void stopAdminPort();

  void send(IRCMessage cmd);
  void send(std::string msg);

  void run();
  void stop();
  void signal(int);

  void addPlugin(std::unique_ptr<Plugin>&& plugin);
  void removePlugin(const std::string& name);
  std::vector<std::string> listPlugins() const;
  std::unique_ptr<SoPlugin> loadSoPlugin(const std::string& fname,
                                         PluginConfig config);

  void startPlugins();
  void stopPlugins();
  void restartPlugin(const std::string& pluginId);
  void reloadPlugin(const std::string& pluginId);

  asio::io_service& getIoService();

 private:
  asio::io_service& m_io_service;
  asio::ip::tcp::socket m_socket;

  IRCParser m_parser;
  std::array<char, 4096> m_buffer;

  Config m_cfg;
  
  std::unique_ptr<AdminPort> m_admin_port;

  std::atomic_bool m_running;
  std::vector<std::unique_ptr<Plugin>> m_plugins;
  std::map<std::string, void*> m_dl_plugins;
  std::mutex m_plugins_mtx;
};

#endif
