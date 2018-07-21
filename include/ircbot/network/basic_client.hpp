#ifndef _IRCBOT_NETWORK_BASIC_CLIENT_HPP
#define _IRCBOT_NETWORK_BASIC_CLIENT_HPP

#include "ircbot/network/buffer.hpp"

namespace ircbot {
namespace network {

class BasicClient {
 public:
  virtual void connect() = 0;
  virtual void send(const Buffer& buf) = 0;
  virtual void receive() = 0;
};

}  // namespace network
}  // namespace ircbot

#endif
