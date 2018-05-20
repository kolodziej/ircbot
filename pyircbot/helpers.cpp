#include "pyircbot/helpers.hpp"

namespace pyircbot {
namespace helpers {

std::string responseDestination(const IRCMessage& msg, bool priv) {
  if (not priv and msg.command == "PRIVMSG" and (
      msg.params[0][0] == '#' or  // standard channels available accros whole network
      msg.params[0][0] == '&' or  // local channels
      msg.params[0][0] == '+')) { // modeless channels
    return msg.params[0];
  }

  return msg.nick;
}

} } // namespace pyircbot::helpers
