#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <signal.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "ircbot/core.hpp"
#include "ircbot/helpers.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/network/context_provider.hpp"
#include "ircbot/plugin.hpp"
#include "ircbot/plugin_graph.hpp"
#include "ircbot/tcp_plugin_server.hpp"
#include "ircbot/version.hpp"

#include "ircbot/clog_log_output.hpp"
#include "ircbot/cout_log_output.hpp"
#include "ircbot/file_log_output.hpp"

namespace asio = boost::asio;
namespace opt = boost::program_options;

using namespace ircbot;

namespace signal_handling {

static Core* client = nullptr;

void signal_handler(int signal) {
  if (client != nullptr) client->signal(signal);
}

}  // namespace signal_handling

int main(int argc, char** argv) {
  std::string config_fname;
  bool daemon;

  opt::options_description cmd_opts("IRCBot client");
  cmd_opts.add_options()("help,h", "show help message")(
      "version,v", "show version information")(
      "daemon,d", opt::bool_switch(&daemon)->default_value(false),
      "run as a daemon")("config,c",
                         opt::value<std::string>(&config_fname)->required(),
                         "configuration file");

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

    Config config{config_fname};

    using network::ContextProvider;
    ContextProvider& ctx = ContextProvider::getInstance();
    ctx.run();

    bool stdout_logging = false;
    // add logger outputs
    Logger& logger = Logger::getInstance();
    for (auto log : config["log"]) {
      std::string level_str = log["level"].as<std::string>();
      std::string log_fname = log["file"].as<std::string>();
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

    std::shared_ptr<Core> client = std::make_shared<Core>(config);
    signal_handling::client = client.get();

    // setting signal handlers
    signal(SIGINT, signal_handling::signal_handler);
    signal(SIGTERM, signal_handling::signal_handler);

    Core::RunResult result{};
    do {
      client->run();
      result = client->waitForStop();

      switch (result) {
        case Core::RunResult::OK:
          LOG(INFO,
              "Client returned from run() successfully. Leaving application.");
          break;
        case Core::RunResult::CONNECTION_ERROR:
          LOG(ERROR, "Connection error. Restarting client...");
          break;
        case Core::RunResult::ERROR:
          LOG(CRITICAL, "Unknown error. Restarting client...");
          break;
        default:
          LOG(CRITICAL, "Unsupported result code!!!");
          break;
      }
    } while (result != Core::RunResult::OK);

    ctx.stop();
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
