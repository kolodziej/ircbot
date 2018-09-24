#ifndef _TCP_PLUGIN_SERVER_HPP
#define _TCP_PLUGIN_SERVER_HPP

#include <array>
#include <string>
#include <tuple>

#include <boost/asio.hpp>

#include "ircbot/core.hpp"
#include "ircbot/tcp_plugin.hpp"

#include "init.pb.h"

namespace asio = boost::asio;

namespace ircbot {

class TcpPluginServer {
 public:
  /** Default constructor
   *
   * \param client shared pointer to client which owns plugins registered by
   * this server
   * \param host hostname/ip addr on which server should listen for connections
   * \pararm port tcp port on which server should listen
   */
  TcpPluginServer(std::shared_ptr<Core> client, const std::string& host,
                  uint16_t port);

  /** start accepting connections from plugins */
  void acceptConnections();

  /** stop acception plugins' connections */
  void stop();

 private:
  /** pointer to client which owns plugins registered by this server */
  std::shared_ptr<Core> m_core;

  /** endpoint on which server is listening for plugins connections */
  asio::ip::tcp::endpoint m_endpoint;

  /** acceptor listening for incoming connections */
  asio::ip::tcp::acceptor m_acceptor;

  /** temporary socket for incoming connections */
  asio::ip::tcp::socket m_socket;

  /** deadline timer for init requests */
  asio::deadline_timer m_deadline_timer;

  /** buffer for incoming requests */
  std::array<char, 8192> m_buffer;

  /** get init request from plugin, authenticate plugin and initialize it */
  void initializePlugin();
};

}  // namespace ircbot

#endif
