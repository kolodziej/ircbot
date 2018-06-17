#include "ircbot/tcp_server.hpp"

#include <boost/endian/arithmetic.hpp>

#include "ircbot/logger.hpp"
#include "tcp_server.pb.h"

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


void TcpServer::Client::start() {
  using namespace TcpServerProtocol;
  // init message
  Message init_msg;
  init_msg.set_type(Message::INIT);

  auto send_init = [this] -> bool { return send(init_msg); };
  if (not helpers::retry(send_init, 500, 5, 1.2)) {
    throw std::runtime_error{"could not send init message!"};
  }

  startReceiving();
}

void TcpServer::Client::stop() {

}

bool TcpServer::Client::send(const TcpServerProtocol::Message &msg) {
  std::string msg_data;
  msg.SerializeToString(&msg_data);

  return send(msg_data);
}

bool TcpServer::Client::send(const std::string &data) {
  try {
    asio::write(m_socket, asio::buffer(data));
    return true;
  } catch (const boost::system::system_error &err) {
    LOG(ERROR, "Could not send message of length: ", data.size(), "!");
  }

  return false;
}

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

void TcpServer::Client::consumeMessage(const TcpServerProtocol::Message &msg) {
  using namespace TcpServerProtocol;

  switch (msg.type()) {
  case Message::INITIALIZED:
    initialized();
    break;

  case Message::DISCONNECT:
    disconnect();
    break;

  case Message::PING:
    ping();
    break;

  case Message::PONG:
    pong();
    break;

  case Message::DATA:
    if (msg.has_data()) {
      data(msg.data());
    } else {
      LOG(WARNING, "DATA message hasn't data!");
    }
    break;

  default:
    LOG(WARNING, "Received message of unsupported type!");
    break;
  }
}

void TcpServer::Client::initialized() {

}

void TcpServer::Client::disconnect() {

}

void TcpServer::Client::ping() {

}

void TcpServer::Client::pong() {

}

void TcpServer::Client::data(const std::string &data) {
  
}

void TcpServer::acceptClient(Client&& client) {
  m_clients.push_back(std::move(client));
}

} // namespace ircbot
