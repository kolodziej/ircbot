#include "ircbot/tcp_plugin.hpp"

#include "message.pb.h"
#include "control.pb.h"

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
    } else if (error == asio::error::operation_aborted) {
      LOG(INFO, "Asynchronous receiving messages cancelled for plugin: ", getId());
      return;
    }

    startReceiving();
  };

  m_socket.async_receive(
      asio::buffer(m_buffer.data(), m_buffer.size()),
      callback
  );
}

void TcpPlugin::onInit() {
  ircbot::ControlRequest ctrl_req;
  ctrl_req.set_type(ircbot::ControlRequest::INIT);

  std::string ctrl_req_buf;
  ctrl_req.SerializeToString(&ctrl_req_buf);
  const size_t buf_size = ctrl_req_buf.size();
  size_t sent = m_socket.send(
    asio::buffer(ctrl_req_buf.data(), buf_size)
  );

  if (sent < buf_size) {
    LOG(ERROR, "Sent ", sent, " out of ", buf_size, " bytes of ControlInitRequest to plugin ", getName());
  }
}

void TcpPlugin::onMessage(IRCMessage /*msg*/) {

}

void TcpPlugin::onNewConfiguration() {

}

bool TcpPlugin::filter(const IRCMessage& /*msg*/) {
  return true;
}

void TcpPlugin::onShutdown() {
  DEBUG("Shutting down socket for Tcp Plugin ", getId());
  m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
  DEBUG("Canceling all asynchronous operations for Tcp Plugin ", getId());
  m_socket.cancel();
  DEBUG("Closing socket for Tcp Plugin ", getId());
  m_socket.close();
}

void TcpPlugin::parseMessage(const std::string& data) {
  ircbot::Message msg;
  msg.ParseFromString(data);
}
