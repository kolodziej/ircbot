#ifndef _SESSION_HPP
#define _SESSION_HPP

#include <string>

#include <boost/asio.hpp>

class Session {
 public:
  Session(boost::asio::io_service& io, std::string address, uint16_t port);

  void connect();

 private:
  boost::asio::io_service& m_io;
  std::string m_address;
  uint16_t m_port;

  boost::asio::ip::tcp::socket m_socket;
};

#endif
