#ifndef _TCP_PLUGIN_HPP
#define _TCP_PLUGIN_HPP

#include "ircbot/plugin.hpp"

#include <array>
#include <future>
#include <boost/asio.hpp>

#include "message.pb.h"

namespace asio = boost::asio;

class TcpPlugin : public Plugin {
 public:
  /** Default constructor
   *
   * \param config plugin configuration
   * \param socket rvalue reference to asio tcp socket on which communication
   * with plugin will be performed
   */
  TcpPlugin(PluginConfig config, asio::ip::tcp::socket&& socket);

  std::string getName() const;

  /** start receiving messages from plugin */
  void startReceiving();

  /** initialize initialization timer */
  void startInitTimer();

  void onInit();
  void onMessage(IRCMessage msg);
  void onNewConfiguration();
  bool filter(const IRCMessage& msg);
  void onShutdown();

  static std::string defaultName(asio::ip::tcp::socket& socket);

 private:
  /** asio tcp socket for communication with plugin */
  asio::ip::tcp::socket m_socket;

  /** initialization timer */
  asio::deadline_timer m_init_timer;

  /** buffer for received messages */
  std::array<char, 8192> m_buffer;

  /** plugin's name **/
  std::string m_name;

  std::promise<void> m_ready_for_shutdown;

  /** parse received message
   *
   * \param data received data
   */
  void parseMessage(const std::string& data);

  /** send message
   *
   * \param msg string containing message
   */
  void sendToPlugin(const std::string& msg);

  void processInitRequest(const PluginProtocol::InitRequest& req);
  void processIrcMessage(const PluginProtocol::IrcMessage& irc_msg);
  void sendInitResponse(const PluginProtocol::InitResponse::Status& status);
  void sendControlRequest(const PluginProtocol::ControlRequest::Type& type,
                          const std::string& msg = {},
                          uint32_t code = 0);
};

#endif
