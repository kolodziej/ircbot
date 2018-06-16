#include "ircbot/tcp_server.hpp"

namespace ircbot {

template <typename ClientData>
TcpServer<ClientData>::TcpServer(asio::io_service &io, uint16_t port)
    : m_io{io}, m_port{port}, m_acceptor{m_io}, m_socket{m_io} {}

template <typename ClientData>
void TcpServer<ClientData>::start() {
  asio::ip::tcp::endpoint endpoint{asio::ip::tcp::v4(), m_port};

  try {
    m_acceptor.open(endpoint.protocol());
    m_acceptor.bind(endpoint);

    listen();
  } catch (const boost::system::system_error &err) {

  }
}

template <typename ClientData>
void TcpServer<ClientData>::stop() {
  // stop accepting
  // close all connections
}

template <typename ClientData>
void TcpServer<ClientData>::listen() {
  auto acceptHandler = [this](const boost::system::error_code &ec) {
    if (ec == 0) {
      Client client{std::move(m_socket)};
      acceptClient(std::move(client));
    }

    listen();
  };

  m_acceptor.async_accept(m_socket, acceptHandler);
}

template <typename ClientData>
TcpServer<ClientData>::Client::Client(asio::ip::tcp::socket &&socket,
                                      ClientData &&data)
    : m_socket{std::move(socket)}, m_data{std::move(data)} {}

template <typename ClientData>
void TcpServer<ClientData>::acceptClient(Client&& client) {
  m_clients.push_back(std::move(client));
}

} // namespace ircbot
