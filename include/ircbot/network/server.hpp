#ifndef _IRCBOT_NETWORK_SERVER_HPP
#define _IRCBOT_NETWORK_SERVER_HPP

#include "ircbot/network/context_provider.hpp"

#include <array>
#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

template <typename Client>
class Server {
 public:
  using ClientType = Client;

  Server(const typename ClientType::ProtocolType::endpoint& endpoint);
  virtual ~Server();

  virtual void start();
  virtual void stop();

 private:
  typename ClientType::ProtocolType::endpoint m_endpoint;
  typename ClientType::ProtocolType::acceptor m_acceptor;
  typename ClientType::ProtocolType::socket m_socket;

  std::vector<ClientType> m_clients;

  void startAsyncAccepting();
};

}  // namespace network
}  // namespace ircbot

#include "ircbot/network/server.impl.hpp"

#endif
