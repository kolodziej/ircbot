#ifndef _IRCBOT_NETWORK_SERVER_HPP
#define _IRCBOT_NETWORK_SERVER_HPP

#include "ircbot/network/basic_server.hpp"
#include "ircbot/network/context_provider.hpp"
#include "ircbot/network/endpoint.hpp"
#include "ircbot/network/tcp_client.hpp"

#include <array>
#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

/** \class TcpServer
 *
 * \brief Tcp server implementation
 *
 * TcpServer is used to implement services listening on tcp port. It uses
 * TcpClient based class to represent connected clients.
 */

class TcpServer : public Endpoint<asio::ip::tcp>, public BasicServer {
 public:
  /** constructor
   *
   * \param host hostname/ip of interface on which server will listen
   * \param port tcp port on which server will listen
   */
  TcpServer(const std::string& host, uint16_t port);

  /** constructor
   *
   * \param endpoint endpoint on which server will listen
   */
  TcpServer(const asio::ip::tcp::endpoint& endpoint);

  /** virtual destructor */
  virtual ~TcpServer();

  virtual void start();
  virtual void stop();

 private:
  /** acceptor socket for server */
  asio::ip::tcp::acceptor m_acceptor;

  /** temporary socket for accepted connections */
  asio::ip::tcp::socket m_socket;

  /** vector of pointers to clients connected to server */
  std::vector<std::unique_ptr<TcpClient>> m_clients;

  void startAsyncAccepting();

  /** creates client from socket
   *
   * This function should be implemented by any implementation that inherits
   * from TcpServer.
   */
  virtual std::unique_ptr<TcpClient> createClient(
      asio::ip::tcp::socket&& socket);
};

}  // namespace network
}  // namespace ircbot

#endif
