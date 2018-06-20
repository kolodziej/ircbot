#include "gtest/gtest.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "ircbot/cerr_log_output.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/tcp_server.hpp"

#include <boost/asio.hpp>

using namespace ircbot;

TEST(TcpServer, TestPing) {
  Logger& logger = Logger::getInstance();
  logger.addOutput(std::make_unique<CerrLogOutput>(LogLevel::DEBUG));

  const uint16_t port = 9090;
  boost::asio::io_service io;

  TcpServer server{io, port};
  server.start();

  std::thread io_thread([&io] { io.run(); });

  using namespace std::literals::chrono_literals;
  std::this_thread::sleep_for(5s);

  server.stop();
  io_thread.join();
}
