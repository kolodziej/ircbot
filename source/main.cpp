#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <stdexcept>

#include "ircbot/bot_config.hpp"
#include "ircbot/session.hpp"
#include "ircbot/bot.hpp"
#include "ircbot/logger.hpp"

#include <boost/program_options.hpp>

namespace opt = boost::program_options;

int main(int argc, char** argv) {
  BotConfig cfg;
  Logger& logger = Logger::getInstance();

  std::fstream error_log{"/var/log/ircbot/error.log",
    std::ios::out | std::ios::app};
  std::fstream debug_log{"/var/log/ircbot/debug.log",
    std::ios::out | std::ios::app};

  logger.addOutput(LogOutput{std::clog, LogLevel::INFO});
  logger.addOutput(LogOutput{error_log, LogLevel::ERROR});
  logger.addOutput(LogOutput{debug_log, LogLevel::DEBUG});

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
    logger(LogLevel::ERROR, "Startup error: ", exc.what());
    return 2;
  }

  boost::asio::io_service io_service;
  std::thread io_thread{[&io_service]() { io_service.run(); }};

  Session sess{io_service, cfg.irc_server, cfg.irc_port};
  Bot bot{sess};

  try {
    logger(LogLevel::INFO, "Connecting to ", cfg.irc_server, ":", cfg.irc_port);
    sess.connect();
    logger(LogLevel::INFO, "Connected to ", cfg.irc_server, ":", cfg.irc_port);
  } catch (std::logic_error& exc) {
    logger(LogLevel::ERROR, "Connection error: ", exc.what());
    io_thread.join();
    return 3;
  }

  std::thread recv_thread(&Bot::run_receiver, &bot);
  std::thread interpreter_thread(&Bot::run_interpreter, &bot);
  std::thread send_thread(&Bot::run_sender, &bot);

  recv_thread.join();
  interpreter_thread.join();
  send_thread.join();
  io_thread.join();

  return 0;
}
