#include "pyircbot/bot.hpp"

#include <iostream>

#include "message.pb.h"
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

void Bot::start() {
  m_io_thread = std::move(std::thread{[this] { m_io.run(); }});
}

void Bot::stop() {
  m_socket.cancel();
  m_socket.close();
  m_io_thread.join();
}

void Bot::send(const std::string& data) {
  m_socket.send(asio::buffer(data.data(), data.size()));
}

void Bot::receive() {
  auto callback = [this](const boost::system::error_code& ec, std::size_t bytes) {
    if (ec == 0) {
      parse(bytes);
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

void Bot::parse(size_t bytes) {
  ircbot::Message msg; 
  auto msg_str = std::string{m_buffer.data(), bytes};
  msg.ParseFromString(msg_str);
  std::cout << "Received " << bytes << " bytes: " << msg_str << "\n";
}

void Bot::initialize(const std::string& name, const std::string& token) {
  ircbot::Message msg;
  msg.set_type(ircbot::Message::INIT_REQUEST);
  
  ircbot::InitRequest* req = msg.mutable_init_req();
  req->set_name(name);
  req->set_token(token);

  std::string serialized;
  msg.SerializeToString(&serialized);

  send(serialized);
}
