#ifndef _IRCBOT_NETWORK_CLIENT_HPP
#define _IRCBOT_NETWORK_CLIENT_HPP

#include "ircbot/network/basic_client.hpp"

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

template <typename Socket>
class Client : public BasicClient {
 public:
  virtual void connect();
  virtual void send(const Buffer& buf);
  virtual void receive(const Buffer& buf);

 private:
  asio::io_service& m_io;
  Socket m_socket;
};

} // namespace network
} // namespace ircbot

#endif
