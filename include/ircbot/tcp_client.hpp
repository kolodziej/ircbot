#ifndef _TCP_CLIENT_HPP
#define _TCP_CLIENT_HPP

#include <boost/asio.hpp>

#include "tcp_server.pb.h"

namespace asio = boost::asio;

namespace ircbot {

class TcpClient {
 public:
  TcpClient(asio::io_service &io, const std::string &host, uint16_t port);

  void start();
  void stop();

  bool send(const TcpServerProtocol::Message &msg);
  bool send(const std::string &data);

 private:
  asio::io_service &m_io;

  asio::ip::tcp::socket m_socket;

  const std::string m_host;

  const m_port;
};

}  // namespace ircbot

#endif
