#ifndef _COMMAND_PARSER_HPP
#define _COMMAND_PARSER_HPP

#include <string>
#include <vector>
#include <queue>

#include "ircbot/parser_config.hpp"
#include "ircbot/irc_message.hpp"

/** \class CommandParser
 *
 * \brief Parses commands from users
 *
 * Takes whole command as a string and splits it into command and arguments. It
 * uses optional prefix character which will indicate that given message is a
 * command. It supports multi-word arguments, quotes, double quotes and escape
 * character \.
 */

class CommandParser {
 public:
  /** Possible token types */
  enum class TokenType {
    COMMAND, /**< command */
    ARGUMENT, /**< single argument */
    END, /**< end of command */
    IGNORE /**< indicates that whole command should be ignored */
  };

  /** All possible states during parsing phase */
  enum class State {
    START = 0, /**< initial state */
    PREFIX, /**< prefix character */
    COMMAND, /**< command part */
    ARGUMENT, /**< arguments */
  };

  /** \class Token
   *
   * \brief Struct keeping token information
   */
  struct Token {
    TokenType type;
    std::string value;
  };

  /** \class Command
   *
   * \brief Struct keeping parsed command and arguments. It is possible to add
   * additional arguments.
   */
  struct Command {
    /** command */
    std::string command;
    /** parsed arguments */
    std::vector<std::string> arguments;
    /** parsed irc message */
    IRCMessage irc_message;
  };

  /** Default constructor
   *
   * \param config ParserConfig object which keeps information about prefix and
   * other necessary parameters of CommandParser
   */
  CommandParser(ParserConfig config);

  /** Takes a command as a string and returns parsed command
   *
   * \param command string with command from user
   * 
   * \return parsed command (Command object)
   */
  Command parse(const std::string& command);

  /** Returns copy of parser configuration */
  ParserConfig getConfig() const;

 private:
  /** Checks if character is from a-zA-Z0-9- set
   *
   * \param x character
   * 
   * \return true if character is from given set
   */
  bool isCommandCharacter(char x);

  /** parser configuration */
  ParserConfig m_config;

  /** current state */
  State m_state;
};

#endif
