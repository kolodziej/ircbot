#ifndef _PYIRCBOT_HELPERS_HPP
#define _PYIRCBOT_HELPERS_HPP

#include <string>

#include "ircbot/irc_message.hpp"

namespace pyircbot {
namespace helpers {

/** Generate destination for PRIVMSG
 *
 * If priv mode is set, response will always be for user who send that message.
 * Otherwise it will be directed to the channel on which msg was sent or to user
 * if it was private message
 *
 * \param msg IRCMessage to which you want to respond
 * \param priv is this message private?
 * \return destination for response
 */
std::string responseDestination(const ircbot::IRCMessage& msg, bool priv = false);

} } // namespace pyircbot::helpers

#endif
