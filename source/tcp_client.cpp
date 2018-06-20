#include "ircbot/tcp_client.hpp"

namespace ircbot {

TcpClient::TcpClient(asio::io_service &io, const std::string &host,
                     uint16_t port)
    : m_io{io}, m_socket{io}, m_host{host}, m_port{port} {}

void TcpClient::start() {}

void TcpClient::stop() {}

bool TcpClient::send(const TcpServerProtocol::Message &msg) {
  std::string msg_data;
  msg.SerializeToString(&msg_data);

  return send(msg_data);
}

bool TcpClient::send(const std::string &data) {
  try {
    DEBUG("Trying to send ", data.size(), " bytes.");
    asio::write(m_socket, asio::buffer(data));
  } catch (const boost::system::system_error &err) {
  }
}

}  // namespace ircbot
