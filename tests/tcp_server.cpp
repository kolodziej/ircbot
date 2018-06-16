#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "ircbot/tcp_server.hpp"

#include <boost/asio.hpp>

using namespace ircbot;

TEST(TcpServer, TestPing) {
  boost::asio::io_service io;
  uint32_t data;
  TcpServer<uint32_t> server(io, 1234);
}
