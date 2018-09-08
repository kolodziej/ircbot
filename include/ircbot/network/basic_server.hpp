#ifndef _IRCBOT_NETWORK_BASIC_SERVER_HPP
#define _IRCBOT_NETWORK_BASIC_SERVER_HPP

namespace ircbot {
namespace network {

/** \class BasicServer
 *
 * \brief Interface for all servers
 *
 * Server abstraction - interface for all servers' types
 */

class BasicServer {
 public:
  /** virtual destructor */
  virtual ~BasicServer() = default;

  /** start listening */
  virtual void start() = 0;

  /** stop listening */
  virtual void stop() = 0;

 protected:
  /** start asynchronous accepting connections */
  virtual void startAsyncAccepting() = 0;
};

}  // namespace network
}  // namespace ircbot

#endif
