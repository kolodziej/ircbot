#include "ircbot/tcp_plugin.hpp"

#include "message.pb.h"

TcpPlugin::TcpPlugin(PluginConfig config, asio::ip::tcp::socket&& socket) :
    Plugin{config},
    m_socket{std::move(socket)} {
  startReceiving();
}

std::string TcpPlugin::getName() const {
  return std::string{"TcpPlugin: "} + getId();
}

void TcpPlugin::startReceiving() {
  auto callback = [this](const boost::system::error_code& error,
                         std::size_t bytes) {
    if (error == 0) {
      std::string data{m_buffer.data(), bytes};
      parseMessage(data);
    }

    startReceiving();
  };

  m_socket.async_receive(
      asio::buffer(m_buffer.data(), m_buffer.size()),
      callback
  );
}

void TcpPlugin::onInit() {
}

void TcpPlugin::onMessage(IRCMessage /*msg*/) {

}

void TcpPlugin::onNewConfiguration() {

}

bool TcpPlugin::filter(const IRCMessage& /*msg*/) {
  return true;
}

void TcpPlugin::onShutdown() {

}

void TcpPlugin::parseMessage(const std::string& data) {
  ircbot::Message msg;
  msg.ParseFromString(data);
}
