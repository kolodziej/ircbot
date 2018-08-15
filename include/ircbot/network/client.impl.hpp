#include "ircbot/network/context_provider.hpp"

#include <stdexcept>

#include "ircbot/logger.hpp"

namespace ircbot {
namespace network {

template <typename Protocol>
Client<Protocol>::Client(const typename Protocol::endpoint& endpoint)
    : m_endpoint{endpoint},
      m_socket{ContextProvider::getInstance().getContext()} {}

template <typename Protocol>
Client<Protocol>::Client(typename Protocol::socket&& socket)
    : m_endpoint{socket.remote_endpoint()}, m_socket{std::move(socket)} {}

template <typename Protocol>
Client<Protocol>::~Client() {
  disconnect();
}

template <typename Protocol>
void Client<Protocol>::connect() {
  if (m_socket.is_open()) {
    throw std::runtime_error{"Socket is already opened!"};
  }

  boost::system::error_code ec;
  m_socket.connect(m_endpoint, ec);

  if (ec != boost::system::errc::success) {
    throw std::runtime_error{"Could not connect!"};
  }
}

template <typename Protocol>
void Client<Protocol>::send(const std::string& data) {
  auto handler = [this](const boost::system::error_code& ec,
                        std::size_t bytes_transferred) {
    writeHandler(ec, bytes_transferred);
  };
  m_socket.async_send(asio::buffer(data), handler);
}

template <typename Protocol>
void Client<Protocol>::receive() {
  auto handler = [this](const boost::system::error_code& ec,
                        std::size_t bytes_transferred) {
    readHandler(ec, bytes_transferred);
  };
  m_socket.async_receive(
      asio::buffer(m_receive_buffer.data(), m_receive_buffer.size()), handler);
}

template <typename Protocol>
void Client<Protocol>::disconnect() {
  DEBUG("Trying to disconnect...");
  if (m_socket.is_open()) {
    DEBUG("Shutting down socket...");
    m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
    DEBUG("Canceling asynchronous operations...");
    m_socket.cancel();
    DEBUG("Closing socket...");
    m_socket.close();
  }
}

template <typename Protocol>
typename Protocol::endpoint Client<Protocol>::endpoint() const {
  return m_endpoint;
}

template <typename Protocol>
void Client<Protocol>::writeHandler(const boost::system::error_code& ec,
                                    std::size_t bytes_transferred) {
  if (ec == boost::system::errc::success) {
    onWrite(bytes_transferred);
  }
}

template <typename Protocol>
void Client<Protocol>::readHandler(const boost::system::error_code& ec,
                                   std::size_t bytes_transferred) {
  if (ec == boost::system::errc::success) {
    onRead(std::string{m_receive_buffer.data(), bytes_transferred});
  }

  if (ec != asio::error::operation_aborted) {
    receive();
  }
}

}  // namespace network
}  // namespace ircbot
