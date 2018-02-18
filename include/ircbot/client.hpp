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

/** \class Client
 *
 * \brief Maintains a connection to server and manages plugins.
 *
 * Client is a representation of connection to IRC server. It routes received
 * messages to plugins which are managed also by this class and takes outgoing
 * messages from plugins and delivers them back to server.
 */

class Client : public std::enable_shared_from_this<Client> {
  using PluginVector = std::vector<std::unique_ptr<Plugin>>;
  using PluginVectorIter = PluginVector::iterator;

 public:
  /** Default constructor
   *
   * \param io_service reference to boost::asio::io_service object
   * \param cfg configuration of bot
   */
  Client(asio::io_service& io_service, Config cfg);

  /** Connects to server given in configuration */
  void connect();
  /** Initializes all plugins from configuration */
  void initializePlugins();
  /** Gently disconnects from server */
  void disconnect();

  /** Initializes receiving messages asynchronously */
  void startAsyncReceive();
  /** Cancels receiving messages */
  void stopAsyncReceive();

  /** Initializes admin port using unix socket
   *
   * \param socket_path path to unix socket file on which administration port
   * will be available
   */
  void startAdminPort(const std::string& socket_path);
  /** Deinitializes admin port */
  void stopAdminPort();

  /** Send IRC message
   *
   * \param cmd instance of IRCMessage containing all message details
   */
  void send(IRCMessage cmd);
  /** Send IRC message
   *
   * \param msg string containing message
   */
  void send(std::string msg);

  /** Start client */ 
  void run();
  /** Stop client */
  void stop();
  /** Signal handler */
  void signal(int);

  /** Load plugin marked in configuration with given id 
   *
   * \param pluginId plugin identification string
   */
  PluginVectorIter loadPlugin(const std::string& pluginId);

  /** Load plugin marked in configuration with given id 
   *
   * \param pluginId plugin identification string
   * \param config configuration (may be loaded from configuration file or given
   * by user)
   *
   * \return Iterator to vector of plugins (each plugin is represented by
   * `std::unique_ptr<Plugin>`)
   */
  PluginVectorIter loadPlugin(const std::string& pluginId, Config config);
  std::unique_ptr<SoPlugin> loadSoPlugin(const std::string& fname,
                                         PluginConfig config);
  PluginVectorIter findPlugin(const std::string& pluginId);
  PluginVectorIter addPlugin(std::unique_ptr<Plugin>&& plugin);
  void removePlugin(PluginVectorIter it);
  std::vector<std::string> listPlugins() const;

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
  PluginVector m_plugins;
  std::map<std::string, void*> m_dl_plugins;
  std::mutex m_plugins_mtx;
};

#endif
