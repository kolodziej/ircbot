#include "ircbot/logger.hpp"

namespace ircbot {
namespace network {

template <typename Client>
Server<Client>::Server(const typename Client::ProtocolType::endpoint& endpoint)
    : m_endpoint{endpoint},
      m_acceptor{ContextProvider::getInstance().getContext(), endpoint},
      m_socket{ContextProvider::getInstance().getContext()} {}

template <typename Client>
Server<Client>::~Server() {
  stop();
}

template <typename Client>
void Server<Client>::start() {
  DEBUG("Starting server on ", m_endpoint);
  startAsyncAccepting();
}

template <typename Client>
void Server<Client>::stop() {
  if (m_acceptor.is_open()) {
    m_acceptor.cancel();
    m_acceptor.close();
  }
}

template <typename Client>
void Server<Client>::startAsyncAccepting() {
  auto handler = [this](const boost::system::error_code& ec) {
    DEBUG("Async accept handler. Error code: ", ec);
    if (ec == boost::system::errc::success) {
      auto remote_endpoint = m_socket.remote_endpoint();
      LOG(INFO, "Creating new client: ", remote_endpoint);
      m_clients.emplace_back(std::move(m_socket));
      m_clients.back().receive();
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

}  // namespace network
}  // namespace ircbot
