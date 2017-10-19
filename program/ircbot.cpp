#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "ircbot/client.hpp"
#include "ircbot/plugin.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/version.hpp"
#include "ircbot/helpers.hpp"

namespace asio = boost::asio;
namespace opt = boost::program_options;

int main(int argc, char **argv) {
  std::string config_fname;
  
  opt::options_description cmd_opts("IRCBot client");
  cmd_opts.add_options()
    ("help,h", "show help message")
    ("version,v", "show version information")
    ("config,c",
       opt::value<std::string>(&config_fname)->required(),
       "configuration file")
  ;

  opt::variables_map var_map;

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

  } catch (std::exception& exc) {
    std::clog << "Terminating application due to unhandled exception. "
        << "Check logs for details.\n";
    LOG(ERROR, "Unhandled exception: ", exc.what(), ". Terminating.");
    return 2;
  }

  return 0;
}
