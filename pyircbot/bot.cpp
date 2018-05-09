#include "pyircbot/bot.hpp"

#include "ircbot/version.hpp"

Bot::Bot(const std::string& hostname, uint16_t port) :
  m_hostname{hostname},
  m_port{port},
  m_socket{m_io} {
}

std::string Bot::hostname() const {
  return m_hostname;
}

uint16_t Bot::port() const {
  return m_port;
}

void Bot::connect() {
  asio::ip::tcp::resolver resolver{m_io};
  auto endpoint_it = resolver.resolve({ m_hostname, std::to_string(m_port) });
  auto endpoint = endpoint_it->endpoint();

  m_socket.connect(endpoint);
}

void Bot::send(const std::string& data) {
  m_socket.send(asio::buffer(data.data(), data.size()));
}

void Bot::receive() {
  auto callback = [this](const boost::system::error_code& ec, std::size_t bytes) {
    if (ec == 0) {
      parse();
    } else {
      return;
    }

    receive();
  };

  m_socket.async_receive(
    asio::buffer(m_buffer.data(), m_buffer.size()),
    callback
  );
}

void Bot::parse() {

}
