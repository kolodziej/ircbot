#ifndef _IRCBOT_NETWORK_ENDPOINT_HPP
#define _IRCBOT_NETWORK_ENDPOINT_HPP

namespace ircbot {
namespace network {

/** \class Endpoint
 *
 * \brief Template of class providing endpoint support for any boost tcp/udp
 * protocols
 *
 * Endpoint provides basic support of endpoints provided by boost::asio library.
 */

template <typename Protocol>
class Endpoint {
 public:
  /** default constructor */
  Endpoint() = default;

  /** copying constructor */
  Endpoint(const typename Protocol::endpoint& endpoint);

  /** Set endpoint
   *
   * \param boost::asio endpoint endpoint that should be represented by object
   */
  void setEndpoint(const typename Protocol::endpoint& endpoint);

  /** Get endpoint
   *
   * \return boost::asio endpoint represented by this object
   */
  typename Protocol::endpoint endpoint() const;

 private:
  /** boost::asio endpoint */
  typename Protocol::endpoint m_endpoint;
};

}  // namespace network
}  // namespace ircbot

#include "ircbot/network/endpoint.impl.hpp"

#endif
