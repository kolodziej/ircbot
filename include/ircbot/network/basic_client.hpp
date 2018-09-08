#ifndef _IRCBOT_NETWORK_BASIC_CLIENT_HPP
#define _IRCBOT_NETWORK_BASIC_CLIENT_HPP

#include <string>

namespace ircbot {
namespace network {

/** \class BasicClient
 *
 * \brief Interface for all clients
 *
 * Client abstraction - interface for all clients' types
 */

class BasicClient {
 public:
  /** virtual destructor */
  virtual ~BasicClient() = default;

  /** Connect to server */
  virtual void connect() = 0;

  /** Send message to server
   *
   * \param buf data that should be sent
   */
  virtual void send(const std::string& buf) = 0;

  /** Start receiving messages
   *
   * Start accepting messages from socket. It may start asynchronous receiving
   * and return or receive and process next message from queue.
   */
  virtual void receive() = 0;
};

}  // namespace network
}  // namespace ircbot

#endif
