#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "ircbot/client.hpp"
#include "ircbot/plugin.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/helpers.hpp"

namespace asio = boost::asio;
namespace opt = boost::program_options;

int main(int argc, char **argv) {
  
  opt::options_description opts("IRCBot client");
  opts.add_options()
    ("h,help", "show help message")
  ;

  return 0;
}
