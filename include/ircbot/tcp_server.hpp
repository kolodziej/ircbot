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

class TcpServer {
 public:
  TcpServer(asio::io_service &io, uint16_t port);

  /** start server */
  void start();

  /** stop server */
  void stop();

  /** \class BaseClient
   *
   * minimum data about client connected to server
   */
  class Client {
   public:
    Client(asio::ip::tcp::socket &&socket);

    void start();
    void stop();

    bool send(const TcpServerProtocol::Message &msg);
    bool send(const std::string &data);

    bool isReceiving() const;

   private:
    void startReceiving();
    void stopReceiving();
    void disconnect();
    void consumeMessage(const TcpServerProtocol::Message &msg);

    void onInitialized();
    void onDisconnect();
    void onPing();
    void onPong();
    void onData(const std::string &data);

   private:
    asio::ip::tcp::socket m_socket;

    asio::io_service::strand m_strand;

    std::vector<char> m_buffer;
  };

  /** creates new client */
  virtual void acceptClient(Client &&client);

 private:
  /** reference to io_service */
  asio::io_service &m_io;

  /** port on which server is listening for connections */
  const uint16_t m_port;

  /** tcp connections acceptor socket */
  asio::ip::tcp::acceptor m_acceptor;

  /** tcp socket for accepted connections */
  asio::ip::tcp::socket m_socket;

  /** vector of clients connected to server */
  std::vector<Client> m_clients;

  /** listen for connections */
  void acceptConnections();
};

}  // namespace ircbot

#endif
