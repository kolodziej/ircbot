#include "ircbot/logger.hpp"

namespace ircbot {
namespace network {

template <typename Protocol>
Server<Protocol>::Server(const typename Protocol::endpoint& endpoint)
    : m_endpoint{endpoint},
      m_acceptor{ContextProvider::getInstance().getContext(), endpoint},
      m_socket{ContextProvider::getInstance().getContext()} {}

template <typename Protocol>
void Server<Protocol>::start() {
  startAsyncAccepting();
}

template <typename Protocol>
void Server<Protocol>::stop() {
  m_acceptor.cancel();
  m_acceptor.close();
}

template <typename Protocol>
void Server<Protocol>::startAsyncAccepting() {
  auto handler = [this](const boost::system::error_code& ec) {
    if (ec == boost::system::errc::success) {
      auto remote_endpoint = m_socket.remote_endpoint();
      LOG(INFO, "Creating new client: ", remote_endpoint);
      m_clients.emplace_back(std::move(m_socket));
      m_clients.back().receive();
      startAsyncAccepting();
    } else if (ec == asio::error::operation_aborted) {
      LOG(INFO, "Asynchronous accepting aborted!");
    } else {
      LOG(ERROR, "Could not accept connection: (error code) ", ec);
    }
  };

  m_acceptor.async_accept(m_socket, handler);
  LOG(INFO, "Waiting for next client...");
}

}  // namespace network
}  // namespace ircbot
