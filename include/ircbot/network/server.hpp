#ifndef _IRCBOT_NETWORK_SERVER_HPP
#define _IRCBOT_NETWORK_SERVER_HPP

#include "ircbot/network/basic_server.hpp"
#include "ircbot/network/client.hpp"

#include <array>
#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

template <typename Socket>
class Server : public BasicServer {
 public:
  Server(const typename Socket::endpoint_type& endpoint);
  void listen();
  void accept();

 private:
  typename Socket::endpoint_type m_endpoint;
  Socket m_acceptor;

  void startAsyncAccepting();
};

}  // namespace network
}  // namespace ircbot

#include "ircbot/network/server.impl.hpp"

#endif
