#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "ircbot/client.hpp"
#include "ircbot/logger.hpp"

namespace asio = boost::asio;

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " server port\n";
    return 1;
  }

  Logger& logger = Logger::getInstance();
  logger.addOutput(LogOutput{std::clog, LogLevel::DEBUG});

  std::string server = argv[1], port = argv[2];

  asio::io_service io;

  Client client(io, server, std::stoi(port));

  std::thread io_thread([&io] { io.run(); });
  client.spawn();

  io_thread.join();
  return 0;
}
