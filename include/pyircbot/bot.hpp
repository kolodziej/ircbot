#ifndef _PYIRCBOT_BOT_HPP
#define _PYIRCBOT_BOT_HPP

#include <thread>
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

  void start();
  void stop();

  void send(const std::string& data);
  void receive();

 private:
  const std::string m_hostname;
  const uint16_t m_port;

  asio::io_service m_io;
  asio::ip::tcp::socket m_socket;

  std::thread m_io_thread;

  std::array<char, 8192> m_buffer;

  void parse(size_t bytes);
};

#endif
