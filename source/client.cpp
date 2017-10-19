#include "ircbot/client.hpp"

#include <stdexcept>
#include <thread>
#include <chrono>
#include <stdexcept>

#include "ircbot/plugin.hpp"
#include "ircbot/helpers.hpp"

Client::Client(asio::io_service& io_service, Config cfg) :
    m_io_service{io_service},
    m_socket{io_service},
    m_cfg{cfg},
    m_running{false}
{
  init();
}

void Client::init() {
  try {
    std::string server = m_cfg.tree().get<std::string>("server");
    uint16_t port = m_cfg.tree().get<uint16_t>("port");

    connect(server, port);

    m_plugins.initializePlugins(m_cfg);

  } catch (pt::ptree_bad_path& exc) {
    LOG(ERROR, "Could not find path in configuration!");
    LOG(ERROR, exc.what());
    throw std::runtime_error{"Client initialization failed!"};
  } catch (pt::ptree_bad_data& exc) {
    LOG(ERROR, "Bad data format in Client's configuration!");
    LOG(ERROR, exc.what());
    throw std::runtime_error{"Client initialization failed!"};
  }
}

void Client::connect(std::string host, uint16_t port) {
  asio::ip::tcp::resolver resolver{m_io_service};

  boost::system::error_code ec;

  LOG(INFO, "Trying to resolve ", host, ':', port);
  auto endp = resolver.resolve({host, std::to_string(port)}, ec);

  if (ec) {
    LOG(ERROR, "Could not resolve address; ec = ", ec);
    throw std::runtime_error{"Could not resolve address!"};
  }

  LOG(INFO, "Trying to connect...");
  asio::connect(m_socket, endp, ec);

  if (ec) {
    throw std::runtime_error{"Could not connect to host!"};
  }

  startAsyncReceive();
}

void Client::startAsyncReceive() {
  using asio::mutable_buffers_1;
  
  auto handler = [this](const boost::system::error_code& ec, size_t bytes) {
    if (ec) {
      stopAsyncReceive();
      return;
    }
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
  m_running = false;
  m_plugin_thread.join();
  m_parser_thread.join();

  stopAsyncReceive();

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
      LOG(INFO, "Sent message of size ", transferred);
    } else {
      LOG(ERROR, "Could not send message! Asio returned ", ec);
    }
  };
  auto const_buf = asio::const_buffers_1(msg.data(), msg.size());
  m_socket.async_send(const_buf, write_handler);
}

void Client::spawn() {
  m_running = true;
  m_plugin_thread = std::move(std::thread{[this] { sendLoop(); }});
  m_parser_thread = std::move(std::thread{[this] { parserLoop(); }});

  using helpers::setThreadName;
  setThreadName(m_plugin_thread, "plugin loop");
  setThreadName(m_parser_thread, "parser loop");
}

PluginManager& Client::pluginManager() {
  return m_plugins;
}

void Client::sendLoop() {
  LOG(INFO, "Starting plugin loop");

  while (m_running) {
    IRCCommand cmd = m_plugins.getOutgoing();
    DEBUG("Sending command: ", cmd.toString(true));
    send(cmd);
  }
}

void Client::parserLoop() {
  LOG(INFO, "Starting parser loop");

  while (m_running) {
    IRCCommand cmd = m_parser.getCommand(); 
    DEBUG("Pushing command to plugins: ", cmd.toString(true));
    m_plugins.putIncoming(cmd);
  }
}
