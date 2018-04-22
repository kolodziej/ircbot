#ifndef _TCP_PLUGIN_HPP
#define _TCP_PLUGIN_HPP

#include "ircbot/plugin.hpp"

#include <array>

#include <boost/asio.hpp>

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

  void onInit();
  void onMessage(IRCMessage msg);
  void onNewConfiguration();
  bool filter(const IRCMessage& msg);
  void onShutdown();

 private:
  /** asio tcp socket for communication with plugin */
  asio::ip::tcp::socket m_socket;

  /** buffer for received messages */
  std::array<char, 8192> m_buffer;

  /** parse received message
   *
   * \param data received data
   */
  void parseMessage(const std::string& data);
};

#endif
