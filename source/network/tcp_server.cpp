#include "ircbot/network/tcp_server.hpp"

namespace ircbot {
namespace network {

TcpServer::TcpServer(const asio::ip::tcp::endpoint& endpoint)
    : Endpoint<asio::ip::tcp>{endpoint},
      m_acceptor{ContextProvider::getInstance().getContext(), endpoint},
      m_socket{ContextProvider::getInstance().getContext()} {}

TcpServer::~TcpServer() { stop(); }

void TcpServer::start() {
  DEBUG("Starting server on ", endpoint());
  startAsyncAccepting();
}

void TcpServer::stop() {
  if (m_acceptor.is_open()) {
    m_acceptor.cancel();
    m_acceptor.close();
  }
}

void TcpServer::startAsyncAccepting() {
  auto handler = [this](const boost::system::error_code& ec) {
    DEBUG("Async accept handler. Error code: ", ec);
    if (ec == boost::system::errc::success) {
      auto remote_endpoint = m_socket.remote_endpoint();
      LOG(INFO, "Creating new client: ", remote_endpoint);
      auto client = createClient(std::move(m_socket));
      m_clients.push_back(std::move(client));
      startAsyncAccepting();
    } else if (ec == asio::error::operation_aborted) {
      LOG(INFO, "Asynchronous accepting aborted!");
    } else if (ec == asio::error::bad_descriptor) {
      LOG(INFO, "Bad descriptor!");
    } else {
      LOG(ERROR, "Could not accept connection: (error code) ", ec);
    }
  };

  LOG(INFO, "Waiting for next client...");
  m_acceptor.async_accept(m_socket, handler);
  DEBUG("Ascyn accepting started!");
}

std::unique_ptr<TcpClient> TcpServer::createClient(
    asio::ip::tcp::socket&& socket) {
  return std::make_unique<TcpClient>(std::move(socket));
}

}  // namespace network
}  // namespace ircbot
