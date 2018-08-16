#ifndef _IRCBOT_NETWORK_BASIC_CLIENT_HPP
#define _IRCBOT_NETWORK_BASIC_CLIENT_HPP

#include <string>

namespace ircbot {
namespace network {

class BasicClient {
 public:
  virtual ~BasicClient() = default;

  virtual void connect() = 0;
  virtual void send(const std::string& buf) = 0;
  virtual void receive() = 0;
};

}  // namespace network
}  // namespace ircbot

#endif
