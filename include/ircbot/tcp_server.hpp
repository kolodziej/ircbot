#ifndef _TCP_SERVER_HPP
#define _TCP_SERVER_HPP

#include <boost/asio.hpp>

namespace asio = boost::asio;

#include "tcp_server.pb.h"

namespace ircbot {

/** \class TcpServer
 *
 * \brief General implementation of tcp server
 *
 * Accepts connections on given host and port and maintains list of
 * connected clients.
 */

template <typename ClientData>
class TcpServer {
public:
  TcpServer(asio::io_service& io, uint16_t port);

  /** start server */
  void start();

  /** stop server */
  void stop();

  /** \class Client
   *
   * minimum data about client connected to server
   */
  struct Client {
    asio::ip::tcp::socket m_socket;

    ClientData m_data;

    Client(asio::ip::tcp::socket &&socket, ClientData &&data);
  };

  /** creates new client */
  virtual void acceptClient(Client &&client);

private:
  /** reference to io_service */
  asio::io_service& m_io;

  /** port on which server is listening for connections */
  const uint16_t m_port;

  /** tcp connections acceptor socket */
  asio::ip::tcp::acceptor m_acceptor;

  /** tcp socket for accepted connections */
  asio::ip::tcp::socket m_socket;

  /** vector of clients connected to server */
  std::vector<Client> m_clients;

  /** listen for connections */
  void listen();
};

} // namespace ircbot

#include "tcp_server.impl.hpp"

#endif
