#include "ircbot/client.hpp"

#include <stdexcept>
#include <thread>
#include <chrono>

Client::Client(asio::io_service& io_service) :
    m_io_service{io_service},
    m_socket{io_service},
    m_running{false},
    m_logger{Logger::getInstance()}
{}

Client::Client(asio::io_service& io_service, std::string host, uint16_t port) :
    Client{io_service} {
  connect(host, port);
}

void Client::connect(std::string host, uint16_t port) {
  asio::ip::tcp::resolver resolver{m_io_service};

  boost::system::error_code ec;
  m_logger(LogLevel::INFO, "Trying to resolve ", host, ':', port);
  auto endp = resolver.resolve({host, std::to_string(port)}, ec);

  if (ec) {
    m_logger(LogLevel::ERROR, "Could not resolve address; ec = ", ec);
    throw std::runtime_error{"Could not resolve address!"};
  }

  m_logger(LogLevel::INFO, "Trying to connect...");
  asio::connect(m_socket, endp, ec);

  if (ec) {
    throw std::runtime_error{"Could not connect to host!"};
  }

  startAsyncReceive();
}

void Client::startAsyncReceive() {
  using asio::mutable_buffers_1;
  
  auto handler = [this](const boost::system::error_code& ec, size_t bytes) {
    m_parser.parse(std::string(m_buffer.data(), bytes));
    startAsyncReceive();
  };

  m_socket.async_receive(
      mutable_buffers_1(m_buffer.data(), m_buffer.size()), handler);
}

void Client::stopAsyncReceive() {
  m_socket.cancel();
}

void Client::disconnect() {
  m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
  m_socket.close();
}

void Client::send(IRCCommand cmd) {
  send(static_cast<std::string>(cmd));
}

void Client::send(std::string msg) {
  auto write_handler = [this](boost::system::error_code ec,
                              size_t transferred) {
    if (not ec) {
      m_logger(LogLevel::INFO, "Sent message of size ", transferred);
    } else {
      m_logger(LogLevel::ERROR, "Could not send message! Asio returned ", ec);
    }
  };
  auto const_buf = asio::const_buffers_1(msg.data(), msg.size());
  m_socket.async_send(const_buf, write_handler);
}

void Client::spawn() {
  m_plugin_thread = std::move(std::thread{[this] { pluginLoop(); }});
  m_parser_thread = std::move(std::thread{[this] { parserLoop(); }});
}

void Client::pluginLoop() {
  using namespace std::literals::chrono_literals;

  while (m_running) {
    std::this_thread::sleep_for(5ms);
  }
  // for plugin, check if it has message to send
}

void Client::parserLoop() {
  using namespace std::literals::chrono_literals;

  while (m_running) {
    if (not m_parser.commandsCount()) {
      std::this_thread::sleep_for(50us);
      continue;
    }

    while (m_parser.commandsCount()) {
      IRCCommand cmd = m_parser.getCommand(); 
      // push to plugins
    }
  }
}

