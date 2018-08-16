#ifndef _IRCBOT_NETWORK_ENDPOINT_HPP
#define _IRCBOT_NETWORK_ENDPOINT_HPP

namespace ircbot {
namespace network {

template <typename Protocol>
class Endpoint {
 public:
  Endpoint() = default;
  Endpoint(const typename Protocol::endpoint& endpoint);

  void setEndpoint(const typename Protocol::endpoint& endpoint);
  typename Protocol::endpoint endpoint() const;

 private:
  typename Protocol::endpoint m_endpoint;
};

}  // namespace network
}  // namespace ircbot

#include "ircbot/network/endpoint.impl.hpp"

#endif
