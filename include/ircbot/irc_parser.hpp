#ifndef _IRC_PARSER_HPP
#define _IRC_PARSER_HPP

#include <queue>
#include <sstream>

#include "irc_message.hpp"

/** \class IRCParser
 *
 * \brief State machine for parsing IRC messages (RFC 1459 compliant)
 *
 * Object of IRCParser allows to parse IRC messages. They can be delivered in
 * parts. State is kept between IRCParser::parse calls. Many messages can also
 * be delivered in one buffer.
 */

class IRCParser {
 public:
  /** Tokens' types used to identify parts returned by lexer */
  enum class TokenType {
    SERVERNAME, /**< servername part of message */
    NICK, /**< nick part of message */
    USER, /**< user part of message */
    HOST, /**< host part of message */
    COMMAND, /**< command part of message */
    PARAM, /**< single parameter */
    CR, /**< carriage return character */
    LF /**< line feed character */
  };

  enum class State {
    NONE, /**< Initial state */
    SERVERNAME_NICK, /**< servername or nick part of message */
    USER, /**< user part of message */
    HOST, /**< host part of message */
    COMMAND, /**< first character of command */
    COMMAND_NUM, /**< numeric command */
    COMMAND_LETTER, /**< alphanumeric command */
    PARAMS, /**< parameters */
    TRAILING, /**< trailing parameter */
    CR, /**< carriage return character */
    LF /**< line feed character */
  };

  /** \class Token
   *
   * \brief Representation of single token in IRCParser */
  struct Token {
    /** token type */
    TokenType type;
    /** characters which this token consist of */
    std::string value;

    /** Constructor with type only
     *
     * \brief Constructor taking only type
     *
     * Constructs Token without content (e.g. LF, CR)
     * 
     * \param type type of token
     */
    Token(TokenType type) : type{type} {}

    /** Constructor 
     *
     * \brief Constructor taking type and value of token
     * 
     * \param type type of token
     * \param value characters which belong to token
     */
    Token(TokenType type, const std::string& value) : type{type}, value{value} {}
  };

  /** Default constructor 
   * 
   * Initializes parser with state State::NONE and last token TokenType::LF
   */
  IRCParser();

  /** Parse message
   *
   * String passed to this function can carry one IRC message, part of IRC
   * message, multiple IRC messages, continuation of previously parsed part of
   * IRC message.
   *
   * \param command IRC message (part, one, multiple, continuation of message)
   */
  void parse(const std::string& command);

  /** Returns number of parsed messages waiting in queue to be extracted from
   * parser
   *
   * \return number of messages that already has been parsed and not extracted
   */
  size_t messagesCount() const;

  /** Checks if parsed message queue is empty
   *
   * \return true if there are no messages waiting to be extracted
   */
  bool messagesEmpty() const;

  /** Get next parsed message
   *
   * Message is removed from queue, so returned object is now the only copy of
   * this message.
   *
   * \return IRCMessage object representing parsed message
   */
  IRCMessage getMessage();

 private:
  /** Lexer and tokenizer part
   *
   * \param command command passed to IRCParser::parse
   */
  void lexer(const std::string& command);

  /** Parser part
   *
   * Takes tokens from queue and parses them.
   */
  void parser();

  /** Create and add to queue token
   *
   * Takes std::stringstream reference from which extracts value of token. After
   * all it sets stringstream content to empty string ""
   *
   * \param type token type
   * \param token std::stringstream containing token's value
   */
  void putToken(TokenType type, std::stringstream& token);

  /** Create and add to queue token without value
   *
   * \param type token type
   */
  void putToken(TokenType type);

  /** Current state of parsing state machine */
  State m_state;
  /** Previously added token */
  TokenType m_last_token;

  /** Queue of tokens */
  std::queue<Token> m_tokens;

  /** Message which is being parsed */
  IRCMessage m_message;

  /** Parsed messages waiting to be extracted with getMessage function */
  std::queue<IRCMessage> m_messages;
};

#endif
