#include "ircbot/tcp_plugin.hpp"

TcpPlugin::TcpPlugin(PluginConfig config, asio::ip::tcp::socket&& socket) :
    Plugin{config},
    m_socket{std::move(socket)} {
  startReceiving();
}

void TcpPlugin::startReceiving() {
  auto callback = [this](const boost::system::error_code& error,
                         std::size_t bytes) {
    if (error == 0) {
      std::string data{m_buffer.data(), bytes};
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
