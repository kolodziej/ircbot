#include "ircbot/tcp_plugin_server.hpp"

#include <string>
#include <array>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "message.pb.h"

namespace asio = boost::asio;
namespace ptime = boost::posix_time;

TcpPluginServer::TcpPluginServer(std::shared_ptr<Client> client,
                                 const std::string& host,
                                 uint16_t port) :
    m_client{client},
    m_acceptor{client->getIoService()},
    m_socket{client->getIoService()},
    m_deadline_timer{client->getIoService()} {
  asio::ip::tcp::resolver resolver{client->getIoService()};
  auto endpoints = resolver.resolve({ host, std::to_string(port) });
  
  LOG(INFO, "Resolved ", host, ":", port, ".");
  
  m_endpoint = *endpoints;
  m_acceptor.open(m_endpoint.protocol());
  m_acceptor.bind(m_endpoint);
  m_acceptor.listen();
}

void TcpPluginServer::acceptConnections() {
  auto callback = [this](const boost::system::error_code& error) {
    if (error == 0) {
      auto endpoint = m_socket.remote_endpoint();
      LOG(INFO, "Accepted new connection: ", endpoint.address().to_string(), ":", endpoint.port());

      initializePlugin();
   } else if (error == asio::error::operation_aborted) {
      LOG(INFO, "Canceling asynchronous connection accepting!");
      return;
   }

    LOG(INFO, "Waiting for new connection...");
    acceptConnections();
  };

  m_acceptor.async_accept(m_socket, m_endpoint, callback);
}

void TcpPluginServer::stop() {
  LOG(INFO, "Canceling all asynchronous operations on acceptor!");
  m_acceptor.cancel();
  LOG(INFO, "Closing acceptor's socket!");
  m_acceptor.close();
}

void TcpPluginServer::initializePlugin() {
  auto recv_callback = [this](const boost::system::error_code& error, std::size_t bytes) {
    if (error == 0) {
      m_deadline_timer.cancel();

      std::string data{m_buffer.data(), bytes};
      bool init_result;
      std::string plugin_id, token;
      std::tie(init_result, plugin_id, token) = parseInitRequest(data);

      if (not init_result) {
        deinitializePlugin(ircbot::InitResponse::ERROR);
      } else {
        if (not authenticatePlugin(token)) {
          LOG(ERROR, "Plugin has incorrect token!");
          deinitializePlugin(ircbot::InitResponse::ERROR);
          return;
        }

        auto plugin = createPlugin(plugin_id, token);
        if (plugin == nullptr) {
          LOG(ERROR, "Plugin has not been created!");
        } else {
          LOG(INFO, "Adding plugin to Client!");
          m_client->addPlugin(std::move(plugin));
        }
      }
    }
  };

  m_socket.async_receive(asio::buffer(m_buffer.data(), m_buffer.size()), recv_callback);
  m_deadline_timer.expires_from_now(ptime::seconds(5));

  boost::system::error_code timer_error;
  m_deadline_timer.wait(timer_error);

  if (timer_error == 0) {
    LOG(ERROR, "Plugin connected to server timed out! Disconnecting!");
    deinitializePlugin(ircbot::InitResponse::TIMEOUT);
  }
}

std::tuple<bool, std::string, std::string>
TcpPluginServer::parseInitRequest(const std::string& req) {
  ircbot::Message msg;
  msg.ParseFromString(req);

  if (msg.type() == ircbot::Message::INIT_REQUEST) {
    LOG(INFO, "Received INIT_REQUEST from TcpPlugin!");

    if (not msg.has_init_req()) {
      LOG(ERROR, "Badly formed INIT_REQUEST!");     
      return std::make_tuple(false, std::string{}, std::string{});
    }

    std::string plugin_id = msg.init_req().id(),
                token = msg.init_req().token();
    return std::make_tuple(true, plugin_id, token);
  }


  LOG(ERROR, "Expected INIT_REQUEST hasn't been received!");
  return std::make_tuple(false, std::string{}, std::string{});
}

std::string TcpPluginServer::serializeInitResponse(ircbot::InitResponse::Status status) {
  ircbot::Message msg;
  msg.set_type(ircbot::Message::INIT_RESPONSE);
  msg.mutable_init_resp()->set_status(status);

  std::string resp;
  if (not msg.SerializeToString(&resp)) {
    LOG(ERROR, "Could not serialize InitResponse!");
    return std::string{};
  }

  return resp;
}

bool TcpPluginServer::authenticatePlugin(const std::string& token) {
  return m_client->authenticatePlugin(token);
}

std::unique_ptr<TcpPlugin>
TcpPluginServer::createPlugin(const std::string& plugin_id,
                              const std::string& token) {
  PluginConfig config{
    m_client,
    plugin_id,
    Config{} // TODO: fetch configuration from client
  };
  return std::make_unique<TcpPlugin>(config, std::move(m_socket));
}

void TcpPluginServer::deinitializePlugin(ircbot::InitResponse::Status status) {
  std::string resp = serializeInitResponse(status);
  if (not resp.empty()) {
    try {
      size_t sent = m_socket.send(asio::buffer(resp));
    } catch (const boost::system::system_error& error) {
      LOG(ERROR, "Could not send InitResponse: ", error.what());
    }
  }

  LOG(INFO, "Closing connection to TcpPlugin!");
  m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
  m_socket.cancel();
  m_socket.close();
}
