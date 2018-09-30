#include "ircbot/tcp_plugin_server.hpp"

#include <array>
#include <string>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "ircbot/network/context_provider.hpp"

#include "message.pb.h"

namespace asio = boost::asio;
namespace ptime = boost::posix_time;

namespace ircbot {

TcpPluginServer::TcpPluginServer(std::shared_ptr<PluginGraph> plugin_graph,
                                 const std::string& host, uint16_t port)
    : m_plugin_graph{plugin_graph},
      m_acceptor{network::ContextProvider::getInstance().getContext()},
      m_socket{network::ContextProvider::getInstance().getContext()},
      m_deadline_timer{network::ContextProvider::getInstance().getContext()} {
  asio::ip::tcp::resolver resolver{
      network::ContextProvider::getInstance().getContext()};
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

  auto plugin =
      std::make_unique<TcpPlugin>(m_plugin_graph, std::move(m_socket));
}

}  // namespace ircbot
