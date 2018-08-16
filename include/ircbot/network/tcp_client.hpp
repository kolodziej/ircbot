#ifndef _IRCBOT_NETWORK_TCP_CLIENT_HPP
#define _IRCBOT_NETWORK_TCP_CLIENT_HPP

#include "ircbot/logger.hpp"
#include "ircbot/network/basic_client.hpp"
#include "ircbot/network/buffer.hpp"
#include "ircbot/network/context_provider.hpp"
#include "ircbot/network/endpoint.hpp"

#include <array>
#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

class TcpClient : public Endpoint<asio::ip::tcp>, public BasicClient {
 public:
  constexpr static const size_t default_buffer_size = 4096;
  TcpClient(const asio::ip::tcp::endpoint&);
  TcpClient(asio::ip::tcp::socket&& socket);
  TcpClient(TcpClient&&) = default;
  virtual ~TcpClient();

  virtual void connect();
  virtual void send(const std::string& data);
  virtual void receive();

  virtual void disconnect();

 protected:
  virtual void onWrite(const size_t bytes_transferred) {}
  virtual void onRead(const std::string& data) {}

 private:
  asio::ip::tcp::socket m_socket;
  std::array<char, default_buffer_size> m_receive_buffer;

  void writeHandler(const boost::system::error_code& ec,
                    std::size_t bytes_transferred);

  void readHandler(const boost::system::error_code& ec,
                   std::size_t bytes_transferred);
};

}  // namespace network
}  // namespace ircbot

#endif
