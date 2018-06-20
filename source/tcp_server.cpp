#include "ircbot/tcp_server.hpp"

#include <boost/endian/arithmetic.hpp>

#include "ircbot/helpers.hpp"
#include "ircbot/logger.hpp"
#include "tcp_server.pb.h"

namespace ircbot {

TcpServer::TcpServer(asio::io_service &io, uint16_t port)
    : m_io{io}, m_port{port}, m_acceptor{m_io}, m_socket{m_io} {}

void TcpServer::start() {
  asio::ip::tcp::endpoint endpoint{asio::ip::tcp::v4(), m_port};

  try {
    DEBUG("Opening socket for acceptor");
    m_acceptor.open(endpoint.protocol());
    DEBUG("Binding acceptor's socket to endpoint");
    m_acceptor.bind(endpoint);

    LOG(INFO, "Start listening on port ", m_port);
    m_acceptor.listen();

    acceptConnections();
  } catch (const boost::system::system_error &err) {
    LOG(ERROR, "Could not start TcpServer on ", m_port, ": ", err.what());
  }
}

void TcpServer::stop() {
  // stop accepting
  m_acceptor.close();

  // close all connections
  for (auto &client : m_clients) {
    client.stop();
  }
}

void TcpServer::acceptConnections() {
  auto acceptHandler = [this](const boost::system::error_code &ec) {
    using helpers::endpointAddress;
    if (ec == 0) {
      LOG(INFO, "Accepting new connection from ",
          endpointAddress(m_socket.remote_endpoint()));
      Client client{std::move(m_socket)};
      acceptClient(std::move(client));
    } else {
      LOG(ERROR,
          "An error occurred during accepting a connectiion! Error code: ", ec);
    }

    acceptConnections();
  };

  LOG(INFO, "Waiting for connection...");
  m_acceptor.async_accept(m_socket, acceptHandler);
}

TcpServer::Client::Client(asio::ip::tcp::socket &&socket)
    : m_socket{std::move(socket)}, m_strand{m_socket.get_io_service()} {}

void TcpServer::Client::start() {
  using namespace TcpServerProtocol;
  // init message
  Message init_msg;
  init_msg.set_type(Message::INIT);

  auto send_init = [this, &init_msg] { return send(init_msg); };
  if (not helpers::retry(send_init, 500, 5, 1.2)) {
    throw std::runtime_error{"could not send init message!"};
  }

  startReceiving();
}

void TcpServer::Client::stop() {}

bool TcpServer::Client::send(const TcpServerProtocol::Message &msg) {
  std::string msg_data;
  msg.SerializeToString(&msg_data);

  return send(msg_data);
}

bool TcpServer::Client::send(const std::string &data) {
  try {
    DEBUG("Trying to send ", data.size(), " bytes.");
    asio::write(m_socket, asio::buffer(data));
    LOG(INFO, "Sent ", data.size(), " bytes to client.");
    return true;
  } catch (const boost::system::system_error &err) {
    LOG(ERROR, "Could not send message of length: ", data.size(), "!");
  }

  return false;
}

bool TcpServer::Client::isReceiving() const { return true; }

void TcpServer::Client::startReceiving() {
  using boost::endian::big_uint32_t;

  static constexpr const std::size_t headerSize = sizeof(big_uint32_t);
  auto recvHandler = [this](const boost::system::error_code &ec,
                            std::size_t bytes) {
    if (ec == 0) {
      TcpServerProtocol::Message msg;
      std::string msg_data{m_buffer.data(), bytes};
      if (msg.ParseFromString(msg_data)) {
        consumeMessage(msg);
      } else {
        LOG(ERROR, "Could not parse message!");
      }
      startReceiving();
    } else if (ec == boost::system::errc::operation_canceled) {
      stopReceiving();
    } else {
      LOG(ERROR, "Receiving error: ", ec);
    }
  };

  char buffer[headerSize];
  std::size_t bytes = asio::read(m_socket, asio::buffer(buffer, headerSize),
                                 asio::transfer_exactly(headerSize));

  if (bytes == headerSize) {
    big_uint32_t payloadSize = *reinterpret_cast<uint32_t *>(buffer);
    m_buffer.resize(payloadSize);
    asio::async_read(m_socket, asio::buffer(m_buffer),
                     asio::transfer_exactly(payloadSize),
                     m_strand.wrap(recvHandler));
  }
}

void TcpServer::Client::stopReceiving() {}

void TcpServer::Client::disconnect() {
  auto close = [this] { m_socket.close(); };
  try {
    m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
    m_socket.cancel();
    m_strand.post(close);
  } catch (const boost::system::system_error &err) {
    LOG(ERROR, "Could not disconnect socket! Error: ", err.what());
  }
}

void TcpServer::Client::consumeMessage(const TcpServerProtocol::Message &msg) {
  using namespace TcpServerProtocol;

  switch (msg.type()) {
    case Message::INITIALIZED:
      onInitialized();
      break;

    case Message::DISCONNECT:
      onDisconnect();
      break;

    case Message::PING:
      onPing();
      break;

    case Message::PONG:
      onPong();
      break;

    case Message::DATA:
      onData(msg.data());
      break;

    default:
      LOG(WARNING, "Received message of unsupported type!");
      break;
  }
}

void TcpServer::Client::onInitialized() {}

void TcpServer::Client::onDisconnect() {}

void TcpServer::Client::onPing() {}

void TcpServer::Client::onPong() {}

void TcpServer::Client::onData(const std::string &data) {}

void TcpServer::acceptClient(Client &&client) {
  client.start();
  m_clients.push_back(std::move(client));
}

}  // namespace ircbot
