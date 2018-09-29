#ifndef _CORE_HPP
#define _CORE_HPP

#include <array>
#include <atomic>
#include <boost/asio.hpp>
#include <condition_variable>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "ircbot/admin_port.hpp"
#include "ircbot/config.hpp"
#include "ircbot/irc_message.hpp"
#include "ircbot/irc_parser.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/network/context_provider.hpp"

namespace asio = boost::asio;

namespace ircbot {

class Plugin;
class SoPlugin;
class TcpPluginServer;
class PluginGraph;

/** \class Core
 *
 * \brief Maintains a connection to server and manages plugins.
 *
 * Client is a representation of connection to IRC server. It routes received
 * messages to plugins which are managed also by this class and takes outgoing
 * messages from plugins and delivers them back to server.
 */

class Core : public std::enable_shared_from_this<Core> {
  using PluginVector = std::vector<std::unique_ptr<Plugin>>;
  using PluginVectorIter = PluginVector::iterator;

 public:
  /** Result of run function */
  enum class RunResult : uint8_t {
    OK = 0x00u,       /**< exit code 0, disconnected normally */
    CONNECTION_ERROR, /**< some connection error */
    ERROR = 0xffu     /**< exit code 0xff, unknown error */
  };

  /** Default constructor
   *
   * \param cfg configuration of bot
   */
  Core(Config cfg);

  /** Connects to server given in configuration */
  void connect();

  /** Initializes all plugins from configuration */
  void initializePlugins();

  /** Deinitializes all plugins */
  void deinitializePlugins();

  /** Disconnects from server */
  void disconnect();

  /** Initializes receiving messages asynchronously */
  void startAsyncReceive();

  /** Cancels receiving messages */
  void stopAsyncReceive();

  /** Initializes admin port using unix socket */
  void startAdminPort();

  /** Deinitializes admin port */
  void stopAdminPort();

  /** Initializes tcp plugin server */
  void startTcpPluginServer();

  /** Stops tcp plugin server */
  void stopTcpPluginServer();

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

  /** Stop client
   *
   * If client must be stopped due to some error, this function will log this
   * fact and set shouldReconnect flag.
   *
   * \param result RunResult indicating if stop was
   */
  void stop();

  /** Wait for client stop
   *
   * \return returns RunResult value indicating if client was stopped normally
   */
  RunResult waitForStop();

  /** Signal handler */
  void signal(int);

  /** Authenticate plugin using token
   *
   * \param token secret token
   */
  bool authenticatePlugin(const std::string& token);

  /** Get boost::asio::io_service instance by reference
   *
   * \return reference to boost::asio::io_service
   */
  asio::io_service& getIoService();

  /** Get uptime of this client
   *
   * Returns duration from creation of this object's creation
   *
   * \return duration from Client creation
   */
  std::chrono::steady_clock::duration getUptime() const;

 private:
  /** Reference to network::ContextProvider */
  network::ContextProvider& m_context_provider;

  /** TCP socket which is connected to IRC server */
  asio::ip::tcp::socket m_socket;

  IRCParser m_parser;

  /** Buffer for received data */
  std::array<char, 4096> m_buffer;

  Config m_cfg;

  /** instance of PluginGraph for this Core */
  std::unique_ptr<PluginGraph> m_plugin_graph;

  /** instance of AdminPort for this Client */
  std::unique_ptr<AdminPort> m_admin_port;

  /** instance of TcpPluginServer for this Client */
  std::unique_ptr<TcpPluginServer> m_tcp_plugin_server;

  /** Indicates if instance is running */
  std::atomic_bool m_running;

  /** result of run function, will be returned with waitForStop */
  RunResult m_result;

  /** promise that will be set when client will be stopped */
  std::promise<RunResult> m_stop_promise;

  /** Vector of unique pointers to plugins */
  PluginVector m_plugins;

  /** Map of plugin ids to pointers to dynamically loaded libraries containing
   * these plugins. Lets us to safely close (dlclose) them when plugins are
   * reloaded or removed
   */
  std::map<std::string, void*> m_dl_plugins;

  /** Mutex protecting vector of plugins */
  std::mutex m_plugins_mtx;

  /** Time when bot has been started */
  std::chrono::steady_clock::time_point m_start_time;
};

}  // namespace ircbot

#endif
