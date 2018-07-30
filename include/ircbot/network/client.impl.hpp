#include "ircbot/network/context_provider.hpp"

#include <stdexcept>

namespace ircbot {
namespace network {

template <typename Socket>
Client<Socket>::Client(const typename Socket::endpoint_type& endpoint)
    : m_endpoint{endpoint},
      m_socket{ContextProvider::getInstance().getContext()} {}

template <typename Socket>
Client<Socket>::Client(Socket&& socket)
    : m_endpoint{socket.remote_endpoint()}, m_socket{std::move(socket)} {}

template <typename Socket>
void Client<Socket>::connect() {
  if (m_socket.is_open()) {
    throw std::runtime_error{"Socket is already opened!"};
  }

  boost::system::error_code ec;
  m_socket.connect(m_endpoint, ec);

  if (ec != boost::system::errc::success) {
    throw std::runtime_error{"Could not connect!"};
  }
}

template <typename Socket>
void Client<Socket>::send(const std::string& data) {
  auto handler = [this](const boost::system::error_code& ec,
                        std::size_t bytes_transferred) {
    writeHandler(ec, bytes_transferred);
  };
  m_socket.async_send(asio::buffer(data), handler);
}

template <typename Socket>
void Client<Socket>::receive() {
  auto handler = [this](const boost::system::error_code& ec,
                        std::size_t bytes_transferred) {
    readHandler(ec, bytes_transferred);
  };
  m_socket.async_receive(
      asio::buffer(m_receive_buffer.data(), m_receive_buffer.size()), handler);
}

template <typename Socket>
void Client<Socket>::writeHandler(const boost::system::error_code& ec,
                                  std::size_t bytes_transferred) {
  if (ec == boost::system::errc::success) {
    onWrite(bytes_transferred);
  }
}

template <typename Socket>
void Client<Socket>::readHandler(const boost::system::error_code& ec,
                                 std::size_t bytes_transferred) {
  if (ec == boost::system::errc::success) {
    onRead(std::string{m_receive_buffer.data(), bytes_transferred});
  }

  receive();
}

}  // namespace network
}  // namespace ircbot
