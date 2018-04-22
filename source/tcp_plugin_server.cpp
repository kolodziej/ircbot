#include "ircbot/tcp_plugin_server.hpp"

#include <string>

TcpPluginServer::TcpPluginServer(std::shared_ptr<Client> client,
                                 const std::string& host,
                                 uint16_t port) :
    m_client{client},
    m_acceptor{client->getIoService()},
    m_socket{client->getIoService()} {
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

      // m_client->addTcpPlugin(m_socket);
      m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
      m_socket.close();
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

void TcpPluginServer::addPluginClient() {

}
