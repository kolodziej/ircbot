#include "ircbot/network/tcp_client.hpp"

namespace ircbot {
namespace network {

TcpClient::TcpClient(const asio::ip::tcp::endpoint& endpoint)
    : Endpoint<asio::ip::tcp>{endpoint},
      m_socket{ContextProvider::getInstance().getContext()} {}

TcpClient::TcpClient(asio::ip::tcp::socket&& socket)
    : Endpoint<asio::ip::tcp>{socket.remote_endpoint()},
      m_socket{std::move(socket)} {}

TcpClient::~TcpClient() { disconnect(); }

void TcpClient::connect() {
  if (m_socket.is_open()) {
    throw std::runtime_error{"Socket is already opened!"};
  }

  boost::system::error_code ec;
  m_socket.connect(endpoint(), ec);

  if (ec != boost::system::errc::success) {
    throw std::runtime_error{"Could not connect!"};
  }
}

void TcpClient::send(const std::string& data) {
  auto handler = [this](const boost::system::error_code& ec,
                        std::size_t bytes_transferred) {
    writeHandler(ec, bytes_transferred);
  };
  m_socket.async_send(asio::buffer(data), handler);
}

void TcpClient::receive() {
  auto handler = [this](const boost::system::error_code& ec,
                        std::size_t bytes_transferred) {
    readHandler(ec, bytes_transferred);
  };
  m_socket.async_receive(
      asio::buffer(m_receive_buffer.data(), m_receive_buffer.size()), handler);
}

void TcpClient::disconnect() {
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

void TcpClient::writeHandler(const boost::system::error_code& ec,
                             std::size_t bytes_transferred) {
  if (ec == boost::system::errc::success) {
    onWrite(bytes_transferred);
  }
}

void TcpClient::readHandler(const boost::system::error_code& ec,
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
