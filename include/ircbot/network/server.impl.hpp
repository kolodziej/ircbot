namespace ircbot {
namespace network {

template <typename Socket>
Server<Socket>::Server(const typename Socket::endpoint_type& endpoint)
    : m_endpoint{endpoint},
      m_acceptor{ContextProvider::getInstance().getContext()} {}

template <typename Socket>
void Server<Socket>::listen() {
  m_acceptor.bind(m_endpoint);
}

}  // namespace network
}  // namespace ircbot
