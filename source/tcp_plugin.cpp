#include "ircbot/tcp_plugin.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "message.pb.h"
#include "control.pb.h"

TcpPlugin::TcpPlugin(PluginConfig config, asio::ip::tcp::socket&& socket) :
    Plugin{config},
    m_socket{std::move(socket)},
    m_init_timer{m_socket.get_io_service()},
    m_name{TcpPlugin::defaultName(m_socket)} {
  startReceiving();
  startInitTimer();
}

std::string TcpPlugin::getName() const {
  return m_name;
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
    } else {
      LOG(ERROR, "TcpPlugin ", getName(), ": error during receiving message! Stopping!");
      return;
    }

    startReceiving();
  };

  m_socket.async_receive(
      asio::buffer(m_buffer.data(), m_buffer.size()),
      callback
  );
}

void TcpPlugin::startInitTimer() {
  auto callback = [this](const boost::system::error_code& ec) {
    if (ec == asio::error::operation_aborted)
      return;

    LOG(ERROR, "TcpPlugin ", getName(), ": initialization timer expired! Stopping.");
    stop();
  };

  m_init_timer.expires_from_now(boost::posix_time::seconds(5));
  m_init_timer.async_wait(callback);
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
  m_init_timer.cancel();

  DEBUG("Shutting down socket for Tcp Plugin ", getId());
  m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
  DEBUG("Canceling all asynchronous operations for Tcp Plugin ", getId());
  m_socket.cancel();
  DEBUG("Closing socket for Tcp Plugin ", getId());
  m_socket.close();
}

std::string TcpPlugin::defaultName(asio::ip::tcp::socket& socket) {
  std::string name{
    "tcp_plugin:" +
    socket.remote_endpoint().address().to_string() + ":" +
    std::to_string(socket.remote_endpoint().port())
  };

  return name;
}

void TcpPlugin::parseMessage(const std::string& data) {
  LOG(INFO, "TcpPlugin ", getName(), " received a message. Trying to parse.");
  ircbot::Message msg;
  if (msg.ParseFromString(data)) {
    LOG(INFO, "TcpPlugin ", getName(), ": Message parsed!")
    if (msg.type() == ircbot::Message::INIT_REQUEST) {
      DEBUG("TcpPlugin ", getName(), ": Received INIT_REQUEST");
      if (msg.has_init_req()) {
        DEBUG("TcpPlugin ", getName(), ": Has init_req");
        processInitRequest(msg.init_req());
      }
    }
  } else {
    LOG(ERROR, "Received message has incorrect format!");
  }
}

void TcpPlugin::send(const std::string& msg) {
  LOG(INFO, "TcpPlugin ", getName(), ": Sending ", msg.size(), " bytes to tcp plugin");
  m_socket.send(asio::buffer(msg.data(), msg.size()));
}

void TcpPlugin::processInitRequest(const ircbot::InitRequest& req) {
  LOG(INFO, "Processing initializatino request from ", getName()); 
  const auto& name = req.name();
  const auto& token = req.token();

  LOG(INFO, "Trying to authenticate plugin ", getName(), " as: ", name, " with token: ", token);
  if (client()->authenticatePlugin(token)) {
    LOG(INFO, "TcpPlugin ", getName(), " has been successfully authenticated!");
    m_name = name;
    m_init_timer.cancel();

    sendInitResponse(ircbot::InitResponse::OK);
  } else {
    LOG(ERROR, "TcpPlugin ", getName(), " failed to authenticate! Stopping.");
    sendInitResponse(ircbot::InitResponse::ERROR);
    stop();
  }
}

void TcpPlugin::sendInitResponse(const ircbot::InitResponse::Status& status) {
  ircbot::Message msg;
  msg.set_type(ircbot::Message::INIT_RESPONSE);

  ircbot::InitResponse* resp = msg.mutable_init_resp();
  resp->set_status(status);

  std::string msg_str;
  msg.SerializeToString(&msg_str);
  send(msg_str);
}
