#include "ircbot/tcp_plugin.hpp"

#include "message.pb.h"

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

  if (msg.type() == ircbot::Message::INIT_REQUEST) {
    LOG(INFO, "Received Init Request from Tcp plugin");

    const std::string plugin_id = msg.init_req().id();
    const std::string token = msg.init_req().token();

    LOG(INFO, "Trying to authenticate plugin ", plugin_id, " with token ", token);
    if (client()->authenticatePlugin(plugin_id, token)) {
      LOG(INFO, "Plugin ", plugin_id, " authenticated!");
      sendInitResponse(true);
    } else {
      LOG(WARNING, "Could not authenticate plugin ", plugin_id, " with token ", token);
      sendInitResponse(false);
    }
  }
}

void TcpPlugin::sendInitResponse(bool status) {

}
