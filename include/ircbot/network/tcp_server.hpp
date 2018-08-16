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

class TcpServer : public Endpoint<asio::ip::tcp>, public BasicServer {
 public:
  TcpServer(const asio::ip::tcp::endpoint& endpoint);
  virtual ~TcpServer();

  virtual void start();
  virtual void stop();

 private:
  asio::ip::tcp::acceptor m_acceptor;
  asio::ip::tcp::socket m_socket;

  std::vector<std::unique_ptr<TcpClient>> m_clients;

  void startAsyncAccepting();
  virtual std::unique_ptr<TcpClient> createClient(
      asio::ip::tcp::socket&& socket);
};

}  // namespace network
}  // namespace ircbot

#endif
