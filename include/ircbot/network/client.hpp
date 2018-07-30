#ifndef _IRCBOT_NETWORK_CLIENT_HPP
#define _IRCBOT_NETWORK_CLIENT_HPP

#include "ircbot/network/basic_client.hpp"
#include "ircbot/network/buffer.hpp"

#include <array>
#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

template <typename Socket>
class Client : public BasicClient {
 public:
  constexpr static const size_t default_buffer_size = 4096;
  Client(const typename Socket::endpoint_type&);
  Client(Socket&& socket);

  virtual void connect();
  virtual void send(const std::string& data);
  virtual void receive();

 private:
  typename Socket::endpoint_type m_endpoint;
  Socket m_socket;
  std::array<char, default_buffer_size> m_receive_buffer;

  void writeHandler(const boost::system::error_code& ec,
                    std::size_t bytes_transferred);

  void readHandler(const boost::system::error_code& ec,
                   std::size_t bytes_transferred);

  virtual void onWrite() {}
  virtual void onRead(const std::string& data) {}
};

}  // namespace network
}  // namespace ircbot

#include "ircbot/network/client.impl.hpp"

#endif
