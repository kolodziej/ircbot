#ifndef _PYIRCBOT_BOT_HPP
#define _PYIRCBOT_BOT_HPP

#include <array>
#include <atomic>
#include <boost/asio.hpp>
#include <thread>

#include "pyircbot/plugin.hpp"

#include "control.pb.h"
#include "init.pb.h"
#include "irc_message.pb.h"

namespace asio = boost::asio;

namespace pyircbot {

/** \class Bot
 *
 * This class represents connection to Bot from plugin that is connected using
 * TCP.
 */

class Bot {
 public:
  /** Default constructor
   *
   * \param hostname hostname on which plugin IRCBot is listening for plugins'
   * connections
   * \param port TCP port on which IRCBot is listening for plugins' connections
   * \param plugin Plugin object which describes plugin and its callbacks
   */
  Bot(const std::string& hostname, uint16_t port, Plugin plugin);

  /** default destructor
   *
   * Stops and waits for all Bot's io thread to terminate
   */
  ~Bot();

  /** Returns hostname
   *
   * \return string containing hostname on which IRCBot is listening for
   * connections from plugins
   */
  std::string hostname() const;

  /** Returns port
   *
   * \return 16-bit unsigned int containing port on which IRCBot is listening
   * for connections from plugins
   */
  uint16_t port() const;

  /** Checks if there is a connection to IRCBot */
  bool connected() const;

  /** Checks if plugin is running */
  bool isRunning() const;

  /** Starts plugin
   *
   *  - connect to IRCBot
   *  - initialize itself (sends name and token to IRCBot)
   *  - initialize asynchronous receiving messages from IRCBot
   *  - starts boost::asio::io_service in separate thread
   */
  void start();

  /** Stops plugin and closes socket connected to IRCBot */
  void stop();

  /** Waits for termination of thread which is running io_service */
  void wait();

  /** Send bare data to IRCBot
   *
   * Normally should not be used by plugin. Left as public only for development
   * phase.
   */
  void send(const std::string& data);

  /** Send IRCMessage to IRCBot
   *
   * \param sourcemsg IRCMessage instance that should be sent
   */
  void sendIrcMessage(const ircbot::IRCMessage& sourcemsg);

 private:
  /** Connects to m_hostname and m_port */
  void connect();

  /** Starts receiving messages from IRCBot */
  void receive();

  /** Initializes plugin
   *
   * Sends name and token to IRCBot. Token should be secret although it is not a
   * security mechanism.
   *
   * \param name plugin's name
   * \param token authentication token
   */
  void initialize(const std::string& name, const std::string& token);

  /** Method processing received InitResponse message
   *
   * \param resp InitResponse message
   */
  void initResponse(const PluginProtocol::InitResponse& resp);

  /** Method processing received IrcMessage messages
   *
   * \param irc_msg IrcMessage message
   */
  void ircMessage(const PluginProtocol::IrcMessage& irc_msg);

  /** Method processing received ControlRequest messages
   *
   * \param req ControlRequest message
   */
  void controlRequest(const PluginProtocol::ControlRequest& req);

  /** Parse bytes received from IRCBot
   *
   * Takes **bytes** from m_buffer and tries to deserialize them and call proper
   * proper function that process message of given type.
   *
   * \param bytes number of bytes that should be taken from m_buffer
   */
  void parse(size_t bytes);

 private:
  /** hostname on which IRCBot is listening for TcpPlugin's connections */
  const std::string m_hostname;

  /** port on which IRCBot is listening for TcpPlugin's connections */
  const uint16_t m_port;

  /** Plugin description */
  Plugin m_plugin;

  /** boost::asio::io_service instance */
  asio::io_service m_io;

  /** TCP socket which is used to connect plugin to IRCBot */
  asio::ip::tcp::socket m_socket;

  /** Thread for io_service::run method */
  std::thread m_io_thread;

  /** Buffer for received messages */
  std::array<char, 8192> m_buffer;

  /** Indicates that plugin is running */
  bool m_started;
};

}  // namespace pyircbot

#endif
