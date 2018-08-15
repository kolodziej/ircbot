#ifndef _IRCBOT_NETWORK_SERVER_HPP
#define _IRCBOT_NETWORK_SERVER_HPP

#include "ircbot/network/basic_server.hpp"
#include "ircbot/network/client.hpp"

#include <array>
#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

template <typename Protocol>
class Server : public BasicServer {
 public:
  Server(const typename Protocol::endpoint& endpoint);
  void start();
  void stop();

 private:
  using ClientType = Client<typename Protocol::socket>;
  typename Protocol::endpoint m_endpoint;
  typename Protocol::acceptor m_acceptor;
  typename Protocol::socket m_socket;

  std::vector<ClientType> m_clients;

  void startAsyncAccepting();
};

}  // namespace network
}  // namespace ircbot

#include "ircbot/network/server.impl.hpp"

#endif
