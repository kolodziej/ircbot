#ifndef _HELPERS_HPP
#define _HELPERS_HPP

#include <string>
#include <thread>

#include "ircbot/irc_message.hpp"

namespace ircbot {
namespace helpers {

std::string removeLineFeed(const std::string& str);

void setThreadName(std::thread& thread, const std::string& name);

bool startsWith(const std::string& str, const std::string& start);

bool isIn(char c, const std::string& s);

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
std::string responseDestination(const ircbot::IRCMessage& msg,
                                bool priv = false);

}  // namespace helpers
}  // namespace ircbot

#endif
