#include "ircbot/tcp_server.hpp"

#include <boost/endian/arithmetic.hpp>

namespace ircbot {

TcpServer::TcpServer(asio::io_service &io, uint16_t port)
    : m_io{io}, m_port{port}, m_acceptor{m_io}, m_socket{m_io} {}

void TcpServer::start() {
  asio::ip::tcp::endpoint endpoint{asio::ip::tcp::v4(), m_port};

  try {
    m_acceptor.open(endpoint.protocol());
    m_acceptor.bind(endpoint);

    listen();
  } catch (const boost::system::system_error &err) {

  }
}

void TcpServer::stop() {
  // stop accepting
  // close all connections
}

void TcpServer::listen() {
  auto acceptHandler = [this](const boost::system::error_code &ec) {
    if (ec == 0) {
      Client client{std::move(m_socket)};
      acceptClient(std::move(client));
    }

    listen();
  };

  m_acceptor.async_accept(m_socket, acceptHandler);
}

TcpServer::Client::Client(asio::ip::tcp::socket &&socket)
    : m_socket{std::move(socket)} {}

void TcpServer::Client::startReceiving() {
  using boost::endian::big_uint32_t;

  static constexpr const std::size_t headerSize = sizeof(big_uint32_t);
  auto recvHandler = [this](const boost::system::error_code &ec,
                            std::size_t bytes) {
    if (ec == 0) {
      // consume data
      startReceiving();
    }
  };

  char buffer[headerSize];
  std::size_t bytes = asio::read(m_socket, asio::buffer(buffer, headerSize),
                                 asio::transfer_exactly(headerSize));

  if (bytes == headerSize) {

    big_uint32_t payloadSize = *reinterpret_cast<uint32_t*>(buffer);
    m_buffer.resize(payloadSize);
    asio::async_read(m_socket, asio::buffer(m_buffer),
                     asio::transfer_exactly(payloadSize), recvHandler);
  }
}

void TcpServer::acceptClient(Client&& client) {
  m_clients.push_back(std::move(client));
}

} // namespace ircbot
