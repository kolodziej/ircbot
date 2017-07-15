#include "ircbot/session.hpp"

#include <stdexcept>

Session::Session(boost::asio::io_service& io, std::string address, uint16_t port) :
  m_io{io},
  m_address{address},
  m_port{port},
  m_socket{m_io}
{}

void Session::connect() {
  boost::asio::ip::tcp::resolver resolver{m_io};
  boost::asio::ip::tcp::resolver::query query{m_address, std::to_string(m_port)};

  boost::system::error_code ec;
  auto endpoint_it = resolver.resolve(query, ec);

  if (ec != 0) { // change to boost ok code
    throw std::logic_error{"address resolving error occurred!"};
  }

  boost::asio::connect(m_socket, endpoint_it, ec);

  if (ec != 0) { // change to boost ok code
    throw std::logic_error{"could not connect to endpoint!"};
  }
}

std::string Session::receive() {
  static const size_t buffer_size = 1024;
  static char buf[buffer_size];

  boost::system::error_code ec;
  auto buffer = boost::asio::buffer(buf, buffer_size);
  size_t read_size = boost::asio::read(m_socket, buffer, ec);

  logger(LogLevel::INFO, "Receive error code: ", ec);
  if (ec != 0)
    return std::string();

  if (ec != 0) { // change to boost ok code
    throw std::logic_error{"an error occurred during reading from socket!"};
  }

  std::string message(buf, read_size);
  return message;
}

void Session::send(const std::string& message) {
  boost::system::error_code ec;
  auto buffer = boost::asio::buffer(message.data(), message.size());
  boost::asio::write(m_socket, buffer, ec);

  if (ec != 0) { // change to boost ok code
    throw std::logic_error{"an error occurred during writing to socket!"};
  }
}
