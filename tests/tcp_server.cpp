#include "gtest/gtest.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "ircbot/tcp_server.hpp"
#include "ircbot/cerr_log_output.hpp"
#include "ircbot/logger.hpp"

#include <boost/asio.hpp>

using namespace ircbot;

TEST(TcpServer, TestPing) {
  Logger& logger = Logger::getInstance();
  logger.addOutput(std::make_unique<CerrLogOutput>(LogLevel::DEBUG));

  const uint16_t port = 9090;
  boost::asio::io_service io;

  TcpServer server{io, port};
  server.start();

  using namespace std::literals::chrono_literals;
  std::this_thread::sleep_for(5s);

  server.stop();
}
