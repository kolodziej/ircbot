#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <memory>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "ircbot/client.hpp"
#include "ircbot/plugin.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/clog_log_output.hpp"
#include "ircbot/helpers.hpp"

namespace asio = boost::asio;

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " config_file\n";
    return 1;
  }

  Logger& logger = Logger::getInstance();
  logger.addOutput(std::make_unique<ClogLogOutput>(LogLevel::DEBUG));

  asio::io_service io;

  Config cfg(argv[1]);
  Client client(io, cfg);

  PluginManager& plugins = client.pluginManager();
  plugins.startPlugins();

  std::thread io_thread([&io] { io.run(); });
  helpers::setThreadName(io_thread, "asio io service");
  client.spawn();

  io_thread.join();
  return 0;
}
