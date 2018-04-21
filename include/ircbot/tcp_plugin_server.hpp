#ifndef _TCP_PLUGIN_SERVER_HPP
#define _TCP_PLUGIN_SERVER_HPP

#include <array>

#include <boost/asio.hpp>

namespace asio = boost::asio;

class TcpPluginServer {
 public:
  /** Default constructor
   *
   * \param client shared pointer to client which owns plugins registered by
   * this server
   * \param host hostname/ip addr on which server should listen for connections
   * \pararm port tcp port on which server should listen
   */
  TcpPluginServer(std::shared_ptr<Client> client,
                  const std::string& host,
                  uint16_t port);

  /** start accepting connections from plugins */
  void acceptConnections();

  /** stop acception plugins' connections */
  void stop();

  /** add plugin client using temporary socket */
  void addPluginClient();

 private:
  /** pointer to client which owns plugins registered by this server */
  std::shared_ptr<Client> m_client;

  /** endpoint on which server is listening for plugins connections */
  asio::ip::tcp::endpoint m_endpoint;

  /** acceptor listening for incoming connections */
  asio::ip::tcp::acceptor m_acceptor;

  /** temporary socket for incoming connections */
  asio::ip::tcp::socket m_socket;

  /** connected plugin clients */
  std::vector<TcpPluginClient> m_clients;
};

#endif
