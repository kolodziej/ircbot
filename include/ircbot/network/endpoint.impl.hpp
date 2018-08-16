namespace ircbot {
namespace network {

template <typename Protocol>
Endpoint<Protocol>::Endpoint(const typename Protocol::endpoint& endpoint)
    : m_endpoint{endpoint} {}

template <typename Protocol>
void Endpoint<Protocol>::setEndpoint(
    const typename Protocol::endpoint& endpoint) {
  m_endpoint = endpoint;
}

template <typename Protocol>
typename Protocol::endpoint Endpoint<Protocol>::endpoint() const {
  return m_endpoint;
}

}  // namespace network
}  // namespace ircbot
