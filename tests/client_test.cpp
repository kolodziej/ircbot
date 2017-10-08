#include "gtest/gtest.h"

#include <iostream>
#include <stdexcept>
#include <thread>
#include <array>
#include <boost/asio.hpp>

#include "ircbot/client.hpp"
#include "ircbot/logger.hpp"

namespace asio = boost::asio;

class TestServer {
 public:
  TestServer();
  ~TestServer();

 private:
  asio::io_service m_io_service;
  asio::ip::tcp::socket m_socket;
  asio::ip::tcp::acceptor m_acceptor;

  std::thread m_io_thread;
  std::array<char, 1024> m_recv_buf;
};

TestServer::TestServer() :
    m_socket{m_io_service},
    m_acceptor{m_io_service, asio::ip::tcp::endpoint{asio::ip::tcp::v4(), 15001}} {
  auto handler = [this](const boost::system::error_code& ec) {
    if (ec) {
      throw std::runtime_error{"could not accept connection!"};
    }

    IRCCommand cmd;
    cmd.command = "PING";
    cmd.params.push_back("testserver.com"); 

    std::string msg{static_cast<std::string>(cmd)};
    auto const_buf = asio::const_buffers_1(msg.data(), msg.size());
    auto mut_buf = asio::mutable_buffers_1(m_recv_buf.data(), m_recv_buf.size());
    for (int i = 0; i < 5; ++i) {
      m_socket.send(const_buf);
    }
  };

  m_acceptor.async_accept(m_socket, handler);
  m_io_thread = std::move(std::thread{[this] { m_io_service.run(); }});

}

TestServer::~TestServer() {
  m_io_thread.join();
}

TEST(ClientTest, Basic) {
  Logger& logger = Logger::getInstance();
  logger.addOutput(LogOutput{std::cerr, LogLevel::DEBUG});
  // TestServer server;

  asio::io_service io_service;
  Client client{io_service};
  client.connect("127.0.0.1", 15001);
  client.spawn();
  std::thread io_thread{[&io_service] { io_service.run(); }};

  io_thread.join();
  client.disconnect();
}
