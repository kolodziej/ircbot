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
  uint16_t port = m_socket.local_endpoint().port();
  std::string plugin_id{"tcp_plugin_" + std::to_string(port)};

  PluginConfig config{
    m_client,
    plugin_id,
    Config{}
  };

  auto plugin = std::make_unique<TcpPlugin>(config, std::move(m_socket));
  m_client->addPlugin(std::move(plugin));
}
