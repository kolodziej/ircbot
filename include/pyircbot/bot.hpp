#ifndef _PYIRCBOT_BOT_HPP
#define _PYIRCBOT_BOT_HPP

#include <pybind11/pybind11.h>
#include <boost/asio.hpp>

#include <array>

namespace py = pybind11;
namespace asio = boost::asio;

class Bot {
 public:
  Bot(const std::string& hostname, uint16_t port);

  std::string hostname() const;
  uint16_t port() const;

  void connect();
  void send(const std::string& data);

 private:
  const std::string m_hostname;
  const uint16_t m_port;

  asio::io_service m_io;
  asio::ip::tcp::socket m_socket;

  std::array<char, 8192> m_buffer;

  void receive();
  void parse();
};

#endif
