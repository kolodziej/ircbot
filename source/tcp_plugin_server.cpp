#include "ircbot/tcp_plugin_server.hpp"

#include <array>
#include <string>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "message.pb.h"

namespace asio = boost::asio;
namespace ptime = boost::posix_time;

namespace ircbot {

TcpPluginServer::TcpPluginServer(std::shared_ptr<Core> core,
                                 const std::string& host, uint16_t port)
    : m_core{core},
      m_acceptor{core->getIoService()},
      m_socket{core->getIoService()},
      m_deadline_timer{core->getIoService()} {
  asio::ip::tcp::resolver resolver{core->getIoService()};
  auto endpoints = resolver.resolve({host, std::to_string(port)});

  LOG(INFO, "Resolved ", host, ":", port, ".");

  m_endpoint = *endpoints;
  m_acceptor.open(m_endpoint.protocol());
  m_acceptor.bind(m_endpoint);
  m_acceptor.listen();
}

void TcpPluginServer::acceptConnections() {
  auto callback = [this](const boost::system::error_code& error) {
    if (error == boost::system::errc::success) {
      auto endpoint = m_socket.remote_endpoint();
      LOG(INFO, "Accepted new connection: ", endpoint.address().to_string(),
          ":", endpoint.port());
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
  std::string host = m_socket.remote_endpoint().address().to_string();
  uint16_t port = m_socket.remote_endpoint().port();

  auto plugin = std::make_unique<TcpPlugin>(m_core, std::move(m_socket));
  m_core->addPlugin(std::move(plugin));
}

}  // namespace ircbot
