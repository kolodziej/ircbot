#include "ircbot/tcp_plugin.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "ircbot/plugin_graph.hpp"

#include "control.pb.h"
#include "message.pb.h"

namespace ircbot {

TcpPlugin::TcpPlugin(std::shared_ptr<PluginGraph> plugin_graph,
                     asio::ip::tcp::socket&& socket)
    : Plugin{plugin_graph->core()},
      m_socket{std::move(socket)},
      m_init_timer{m_socket.get_io_service()},
      m_name{TcpPlugin::defaultName(m_socket)} {
  startReceiving();
  startInitTimer();
}

std::string TcpPlugin::getName() const { return m_name; }

void TcpPlugin::startReceiving() {
  auto callback = [this](const boost::system::error_code& error,
                         std::size_t bytes) {
    if (error == boost::system::errc::success) {
      std::string data{m_buffer.data(), bytes};
      parseMessage(data);
      startReceiving();
    } else {
      if (error == asio::error::operation_aborted) {
        LOG(INFO, "Asynchronous receiving messages cancelled for plugin: ",
            getName());
      } else {
        LOG(ERROR, "TcpPlugin ", getName(),
            ": error during receiving message! Stopping!");
      }

      m_ready_for_shutdown.set_value();
      stop();
    }
  };

  m_socket.async_receive(asio::buffer(m_buffer.data(), m_buffer.size()),
                         callback);
}

void TcpPlugin::startInitTimer() {
  auto callback = [this](const boost::system::error_code& ec) {
    if (ec == asio::error::operation_aborted) return;

    LOG(ERROR, "TcpPlugin ", getName(),
        ": initialization timer expired! Stopping.");
    stop();
  };

  m_init_timer.expires_from_now(boost::posix_time::seconds(5));
  m_init_timer.async_wait(callback);
}

void TcpPlugin::onInit() {
  PluginProtocol::ControlRequest ctrl_req;
  ctrl_req.set_type(PluginProtocol::ControlRequest::INIT);

  std::string ctrl_req_buf;
  ctrl_req.SerializeToString(&ctrl_req_buf);
  const size_t buf_size = ctrl_req_buf.size();
  size_t sent = m_socket.send(asio::buffer(ctrl_req_buf.data(), buf_size));

  if (sent < buf_size) {
    LOG(ERROR, "Sent ", sent, " out of ", buf_size,
        " bytes of ControlInitRequest to plugin ", getName());
  }
}

void TcpPlugin::onMessage(IRCMessage received_msg) {
  PluginProtocol::Message msg;
  msg.set_type(PluginProtocol::Message::IRC_MESSAGE);

  PluginProtocol::IrcMessage* irc_msg = msg.mutable_irc_msg();
  irc_msg->set_servername(received_msg.servername);
  irc_msg->set_user(received_msg.user);
  irc_msg->set_nick(received_msg.nick);
  irc_msg->set_host(received_msg.host);
  irc_msg->set_command(received_msg.command);

  for (const auto& param : received_msg.params) {
    irc_msg->add_params(param);
  }

  DEBUG("Serializing IRCMessage to PluginProtocol::IrcMessage");
  std::string serialized;
  msg.SerializeToString(&serialized);

  LOG(INFO, "Sending PluginProtocol::IRCMessage to TcpPlugin ", getName());
  sendToPlugin(serialized);
}

void TcpPlugin::onNewConfiguration() {
  LOG(WARNING, "Setting configuration in TcpPlugins is not supported yet!");
}

bool TcpPlugin::filter(const IRCMessage& /*msg*/) { return true; }

void TcpPlugin::onShutdown() {
  m_init_timer.cancel();

  if (not m_socket.is_open()) {
    LOG(INFO, "TcpPlugin ", getName(),
        ": socket is not open. Closing without shutdown sequence!");
    return;
  }

  DEBUG("Sending SHUTDOWN message to TcpPlugin ", getName());
  sendControlRequest(PluginProtocol::ControlRequest::SHUTDOWN);

  LOG(INFO, "Waiting until TcpPlugin ", getName(), " is ready for shutdown...");
  m_ready_for_shutdown.get_future().get();

  try {
    DEBUG("Shutting down socket for Tcp Plugin ", getName());
    m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
  } catch (const boost::system::system_error& error) {
    LOG(ERROR, "Could not shutdown connection to Tcp Plugin ", getName());
  }

  DEBUG("Canceling all asynchronous operations for Tcp Plugin ", getName());
  m_socket.cancel();
  DEBUG("Closing socket for Tcp Plugin ", getName());
  m_socket.close();

  LOG(INFO, "Removing plugin from Client");
  // core()->removePlugin(getName());
}

std::string TcpPlugin::defaultName(asio::ip::tcp::socket& socket) {
  std::string name{
      "tcp_plugin:" + socket.remote_endpoint().address().to_string() + ":" +
      std::to_string(socket.remote_endpoint().port())};

  return name;
}

void TcpPlugin::parseMessage(const std::string& data) {
  LOG(INFO, "TcpPlugin ", getName(), " received a message. Trying to parse.");
  PluginProtocol::Message msg;
  if (msg.ParseFromString(data)) {
    LOG(INFO, "TcpPlugin ", getName(), ": Message parsed!")
    switch (msg.type()) {
      case PluginProtocol::Message::INIT_REQUEST:
        DEBUG("TcpPlugin ", getName(), ": Received INIT_REQUEST");
        if (msg.has_init_req()) {
          DEBUG("TcpPlugin ", getName(), ": Has init_req");
          processInitRequest(msg.init_req());
        }
        break;
      case PluginProtocol::Message::IRC_MESSAGE:
        DEBUG("TcpPlugin ", getName(), ": Received IRC_MESSAGE");
        if (msg.has_irc_msg()) {
          DEBUG("TcpPlugin ", getName(), ": Has irc_msg");
          processIrcMessage(msg.irc_msg());
        }
        break;
      default:
        LOG(WARNING, "Received unsupported type of message!");
        break;
    }
  } else {
    LOG(ERROR, "Received message has incorrect format!");
  }
}

void TcpPlugin::sendToPlugin(const std::string& msg) {
  try {
    LOG(INFO, "TcpPlugin ", getName(), ": Sending ", msg.size(),
        " bytes to tcp plugin");
    m_socket.send(asio::buffer(msg.data(), msg.size()));
  } catch (const boost::system::system_error& error) {
    LOG(ERROR, "Could not send data to TcpPlugin ", getName(), ": ",
        error.what());
  }
}

void TcpPlugin::processInitRequest(const PluginProtocol::InitRequest& req) {
  LOG(INFO, "Processing initializatino request from ", getName());
  const auto& name = req.name();
  const auto& token = req.token();

  LOG(INFO, "Trying to authenticate plugin ", getName(), " as: ", name,
      " with token: ", token);
  if (core()->authenticatePlugin(token)) {
    LOG(INFO, "TcpPlugin ", getName(), " has been successfully authenticated!");
    m_name = name;
    m_init_timer.cancel();

    sendInitResponse(PluginProtocol::InitResponse::OK);
    spawn();
  } else {
    LOG(ERROR, "TcpPlugin ", getName(), " failed to authenticate! Stopping.");
    sendInitResponse(PluginProtocol::InitResponse::ERROR);
    stop();
  }
}

void TcpPlugin::processIrcMessage(const PluginProtocol::IrcMessage& pb_msg) {
  IRCMessage msg = IRCMessage::fromProtobuf(pb_msg);
  send(msg);
}

void TcpPlugin::sendInitResponse(
    const PluginProtocol::InitResponse::Status& status) {
  PluginProtocol::Message msg;
  msg.set_type(PluginProtocol::Message::INIT_RESPONSE);

  PluginProtocol::InitResponse* resp = msg.mutable_init_resp();
  resp->set_status(status);

  std::string serialized;
  msg.SerializeToString(&serialized);
  sendToPlugin(serialized);
}

void TcpPlugin::sendControlRequest(
    const PluginProtocol::ControlRequest::Type& type,
    const std::string& req_msg, uint32_t code) {
  PluginProtocol::Message msg;
  msg.set_type(PluginProtocol::Message::CONTROL_REQUEST);

  PluginProtocol::ControlRequest* req = msg.mutable_ctrl_req();
  req->set_type(type);
  if (not req_msg.empty()) req->set_msg(req_msg);
  if (code > 0) req->set_code(code);

  std::string serialized;
  msg.SerializeToString(&serialized);
  sendToPlugin(serialized);
}

}  // namespace ircbot
