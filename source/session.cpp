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
