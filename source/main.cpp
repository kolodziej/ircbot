#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <stdexcept>

#include "ircbot/bot_config.hpp"
#include "ircbot/session.hpp"

#include <boost/program_options.hpp>

namespace opt = boost::program_options;

#include "ircbot/logger.hpp"

int main(int argc, char** argv) {
  BotConfig cfg;

  opt::options_description opts("Options");
  opts.add_options()
    (
      "irc-server", 
      opt::value<std::string>(&cfg.irc_server)->required(),
      "address of IRC server"
    )
    (
      "irc-port",
      opt::value<uint16_t>(&cfg.irc_port)->required(),
      "port of IRC server"
    )
    (
      "cli-address",
      opt::value<std::string>(&cfg.cli_address)->default_value("127.0.0.1"),
      "address for cli server to listen on"
    )
    (
      "cli-port",
      opt::value<uint16_t>(&cfg.cli_port)->default_value(5001),
      "port for cli server to listen on"
    )
  ;

  opt::positional_options_description positional;
  positional.add("plugins", -1);

  opt::variables_map vm;
  opt::store(opt::command_line_parser(argc, argv)
              .options(opts)
              .positional(positional)
              .run(),
            vm);

  if (vm.count("help")) {
    std::cout << opts << "\n";
    return 1;
  }

  try {
    opt::notify(vm);
  } catch (opt::error& exc) {
    std::cerr << "Error: " << exc.what() << "\n";
    return 2;
  }

  boost::asio::io_service io_service;
  Session sess{io_service, cfg.irc_server, cfg.irc_port};

  std::thread io_thread{[&io_service]() { io_service.run(); }};

  try {
    sess.connect();
  } catch (std::logic_error& exc) {
    std::cerr << "Connection error: " << exc.what() << "\n";
    io_thread.join();
    return 3;
  }

  io_thread.join();

  return 0;
}
