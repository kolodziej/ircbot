#include <iostream>
#include <string>
#include <thread>
#include <array>

#include <cstdlib>
#include <cstdio>

#include <readline/readline.h>
#include <readline/history.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#ifndef SOCKET_PATH
#define SOCKET_PATH "/var/run/ircbot.admin"
#endif

namespace asio = boost::asio;
namespace opt = boost::program_options;

int main(int argc, char** argv) {
  std::string socket_fname;

  opt::options_description opts("IRCBot CTL");
  opts.add_options()
    ("help,h", "show help message")
    ("version,v", "show version information")
    ("socket,s",
     opt::value<std::string>(&socket_fname)->default_value(SOCKET_PATH),
     "host on which ircbot admin port is listening")
  ;
  
  opt::variables_map var;
  try {
    opt::store(opt::parse_command_line(argc, argv, opts), var);

    if (var.count("help")) {
      std::cout << opts;
      return 1;
    }

    if (var.count("version")) {
      std::cout << "ircbotctl\n";
      return 0;
    }

    opt::notify(var);
  } catch (opt::error& erc) {
    std::cerr << "Problem during parsing arguments!\n";
  }

  std::cout << "Chosen admin port: " << socket_fname << '\n';

  asio::io_service io;

  asio::local::stream_protocol::endpoint ep(socket_fname);
  asio::local::stream_protocol::socket socket(io);
  boost::system::error_code ec;
  socket.connect(ep, ec);
  if (ec) {
    std::cerr << "Could not connect to admin port unix socket!\n";
    return 2;
  }

  std::cout << "Probably connected.. :-)\n";

  bool quit = false;
  const char* prompt = "ircbotctl> ";
  std::array<char, 4096> read_buf;
  while (not quit) {
    char* line = readline(prompt);
    size_t size = strlen(line);
    if (size > 0) {
      add_history(line);

      if (strcmp(line, "quit") == 0) {
        quit = true;
      } else {
        socket.send(asio::buffer(line, size));
        
        if (strcmp(line, "shutdown") == 0) {
          std::cout << "Command sent! Quitting...\n";
          quit = true;
          break;
        }

        size_t read = socket.read_some(
          asio::buffer(read_buf.data(), read_buf.size())
        );

        if (read > 0) {
          std::cout << std::string(read_buf.data(), read) << '\n';
        }
      }
    }
  }

  socket.shutdown(
    asio::local::stream_protocol::socket::shutdown_both
  );
  socket.close();

  return 0;
}
