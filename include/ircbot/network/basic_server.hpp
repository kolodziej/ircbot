#ifndef _IRCBOT_NETWORK_BASIC_SERVER_HPP
#define _IRCBOT_NETWORK_BASIC_SERVER_HPP

namespace ircbot {
namespace network {

class BasicServer {
 public:
  virtual void listen() = 0;
  virtual void accept() = 0;

 protected:
  virtual void startAsyncAccepting() = 0;
};

}  // namespace network
}  // namespace ircbot

#endif
