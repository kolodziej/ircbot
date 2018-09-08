#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <signal.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "ircbot/client.hpp"
#include "ircbot/helpers.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/plugin.hpp"
#include "ircbot/tcp_plugin_server.hpp"
#include "ircbot/version.hpp"

#include "ircbot/clog_log_output.hpp"
#include "ircbot/cout_log_output.hpp"
#include "ircbot/file_log_output.hpp"

namespace asio = boost::asio;
namespace opt = boost::program_options;

using namespace ircbot;

namespace signal_handling {

static Client* client = nullptr;

void signal_handler(int signal) {
  if (client != nullptr) client->signal(signal);
}

}  // namespace signal_handling

int main(int argc, char** argv) {
  std::string config_fname, admin_port_socket, tcp_server_host;
  bool daemon;
  uint16_t tcp_server_port;

  opt::options_description cmd_opts("IRCBot client");
  cmd_opts.add_options()("help,h", "show help message")(
      "version,v", "show version information")(
      "daemon,d", opt::bool_switch(&daemon)->default_value(false),
      "run as a daemon")("config,c",
                         opt::value<std::string>(&config_fname)->required(),
                         "configuration file")(
      "admin-port,a", opt::value<std::string>(&admin_port_socket),
      "path to socket for admin port")(
      "tcp-server-host",
      opt::value<std::string>(&tcp_server_host)->default_value("127.0.0.1"),
      "host on which tcp plugin server should listen")(
      "tcp-server-port",
      opt::value<uint16_t>(&tcp_server_port)->default_value(5454),
      "port on which tcp plugin server should listen");

  opt::variables_map var_map;

  std::vector<std::ofstream> log_files;
  try {
    try {
      opt::store(opt::parse_command_line(argc, argv, cmd_opts), var_map);

      if (var_map.count("help")) {
        std::clog << cmd_opts;
        return 0;
      }

      if (var_map.count("version")) {
        std::cout << "IRCBot " << version::str() << '\n';
        return 0;
      }

      opt::notify(var_map);
    } catch (opt::error& exc) {
      std::clog << exc.what() << '\n';
      std::clog << cmd_opts;
      return 1;
    }

    asio::io_service io;
    Config config{config_fname};

    bool stdout_logging = false;
    // add logger outputs
    Logger& logger = Logger::getInstance();
    for (auto log : config.tree().get_child("log")) {
      std::string level_str = log.second.get<std::string>("level");
      std::string log_fname = log.second.get<std::string>("file");
      LogLevel level = GetLogLevel(level_str);
#ifndef LOG_DEBUG
      if (level == LogLevel::DEBUG) {
        std::cerr << "DEBUG log has been chosen for output " << log_fname
                  << ", but DEBUG logs are not available in this compilation!";
        return 2;
      }
#endif
      if (log_fname == "-" or log_fname == "-stderr") {
        logger.addOutput(std::make_unique<ClogLogOutput>(level));
        stdout_logging = true;
      } else if (log_fname == "-stdout") {
        logger.addOutput(std::make_unique<CoutLogOutput>(level));
        stdout_logging = true;
      } else {
        logger.addOutput(std::make_unique<FileLogOutput>(log_fname, level));
      }
    }

    if (stdout_logging and daemon) {
      std::clog
          << "You have chosen logging to stdout/stderr and daemon mode. "
          << "It can can cause printing text in terminal which is not running "
          << "ircbot in foreground!\n";
    }

    if (daemon) {
      pid_t id = fork();
      if (id == -1) {
        LOG(ERROR,
            "Error occurred during forking daemon process. Continuing in "
            "foreground!");
      } else if (id > 0) {
        LOG(INFO, "Succesfully forked daemon process (PID ", id,
            ") to background!");
        return 0;
      }
    }

    std::shared_ptr<Client> client = std::make_shared<Client>(io, config);
    signal_handling::client = client.get();

    do {
      client->connect();

      if (not admin_port_socket.empty()) {
        LOG(INFO, "Trying to initialize admin port at: ", admin_port_socket);
        client->startAdminPort(admin_port_socket);
      }

      client->initializePlugins();

      signal(SIGINT, signal_handling::signal_handler);
      signal(SIGTERM, signal_handling::signal_handler);

      client->startPlugins();

      // start tcp plugin server
      client->startTcpPluginServer(tcp_server_host, tcp_server_port);

      LOG(INFO, "Spawning io_thread");
      std::thread io_thread([&io] { io.run(); });

      LOG(INFO, "Waiting for io_thread...");
      io_thread.join();
    } while (client->shouldReconnect());

  } catch (std::runtime_error& exc) {
    std::cerr << "Runtime error: " << exc.what() << '\n';
    return 2;
  } catch (std::exception& exc) {
    std::cerr << "Terminating application due to unhandled exception!\n";
    std::cerr << "Unhandled exception: " << exc.what() << ". Terminating.\n";
    return 3;
  }

  return 0;
}
