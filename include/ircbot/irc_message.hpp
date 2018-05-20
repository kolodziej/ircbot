#ifndef _IRC_MESSAGE_HPP
#define _IRC_MESSAGE_HPP

#include <string>
#include <vector>
#include <ostream>
#include <initializer_list>

#include "irc_message.pb.h"

/** \class IRCMessage
 *
 * \brief Representation of parsed IRC message (compliant with RFC 1459)
 *
 * All parsed irc messages are at some step represented by object of this
 * structure. IRCParser returns correctly parsed messages as object of
 * IRCMessage. IRCMessage::toString function provides possibility to create
 * outgoing messages easily.
 */

struct IRCMessage {
  /** Defualt constructor */
  IRCMessage() = default;

  /** Constructor that takes command and params
   *
   * \param command IRC command (e.g. PRIVMSG, PONG)
   * \param params list of string parameters represented by
   * std::initializer_list<std::string>
   */
  IRCMessage(const std::string& command,
             std::initializer_list<std::string> params);

  /** servername part of message */
  std::string servername;
  
  /** user part of message (may be empty) */
  std::string user;

  /** nick part of message (may be empty) */
  std::string nick;

  /** host part of message */
  std::string host;

  /** command part of message */
  std::string command;

  /** vector of params */
  std::vector<std::string> params;

  /** conversion to std::string operator (uses toString function) */
  operator std::string() const;

  /** Conversion to string
   *
   * \param stripCRLF if true, \r\n sequence at the end is not added (useful
   * for generating logs)
   */
  std::string toString(bool stripCRLF = false) const;

  /** Conversion from Protobuf IrcMessage to IRCMessage
   *
   * \param pb_msg object of protobuf IrcMessage
   * \return IRCMessage object
   */
  static IRCMessage fromProtobuf(const PluginProtocol::IrcMessage&);
};

#endif
