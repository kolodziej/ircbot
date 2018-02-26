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

  /** Load plugin from shared object file
   *
   * \param fname shared object filename 
   * \param config plugin configuration
   *
   * \return unique pointer to SoPlugin object
   */
  std::unique_ptr<SoPlugin> loadSoPlugin(const std::string& fname,
                                         PluginConfig config);

  /** Find plugin by id
   *
   * \param pluginId plugin id matching one given in configuration file
   * 
   * \return iterator to element in vector of unique pointers to plugins
   */
  PluginVectorIter findPlugin(const std::string& pluginId);
  
  /** Add plugin to client instance
   *
   * \param plugin r-value reference to unique pointer to Plugin instance
   *
   * \return iterator to inserted plugin 
   */
  PluginVectorIter addPlugin(std::unique_ptr<Plugin>&& plugin);

  /** Removes plugin from client instance
   *
   * \param it iterator to plugin in vector (may be obtained using findPlugin
   * function
   */
  void removePlugin(PluginVectorIter it);

  /* Return list of all plugins' names added to this instance
   *
   * \return vector of plugins' names
   */
  std::vector<std::string> listPlugins() const;

  /** Start all plugins */
  void startPlugins();

  /** Stops all plugins */
  void stopPlugins();

  /** Restart plugin with given id
   *
   * \param pluginId plugin's id given in configuration file
   */
  void restartPlugin(const std::string& pluginId);

  /** Reload plugin with given id
   *
   * \param pluginId plugin's id given in configuration file
   */
  void reloadPlugin(const std::string& pluginId);

  /** Get boost::asio::io_service instance by reference
   *
   * \return reference to boost::asio::io_service
   */
  asio::io_service& getIoService();

 private:
  /** Reference to instance of io_service */
  asio::io_service& m_io_service;

  /** TCP socket which is connected to IRC server */
  asio::ip::tcp::socket m_socket;

  IRCParser m_parser;

  /** Buffer for received data */
  std::array<char, 4096> m_buffer;

  Config m_cfg;
  
  std::unique_ptr<AdminPort> m_admin_port;

  /** Indicates if instance is running */
  std::atomic_bool m_running;

  /** Vector of unique pointers to plugins */
  PluginVector m_plugins;

  /** Map of plugin ids to pointers to dynamically loaded libraries containing
   * these plugins. Lets us to safely close (dlclose) them when plugins are
   * reloaded or removed
   */
  std::map<std::string, void*> m_dl_plugins;

  /** Mutex protecting vector of plugins */
  std::mutex m_plugins_mtx;
};

#endif
