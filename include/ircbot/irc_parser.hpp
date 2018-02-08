#ifndef _IRC_PARSER_HPP
#define _IRC_PARSER_HPP

#include <queue>
#include <sstream>

#include "irc_command.hpp"

class IRCParser {
 public:
  enum class TokenType {
    SERVERNAME,
    NICK,
    USER,
    HOST,
    COMMAND,
    PARAM,
    CR,
    LF
  };

  enum class State {
    NONE,
    SERVERNAME_NICK,
    USER,
    HOST,
    COMMAND,
    COMMAND_NUM,
    COMMAND_LETTER,
    PARAMS,
    TRAILING,
    CR,
    LF
  };

  struct Token {
    TokenType type;
    std::string value;

    Token(TokenType type) : type{type} {}
    Token(TokenType type, const std::string& value) : type{type}, value{value} {}
  };

  IRCParser();

  void parse(const std::string& command);

  size_t commandsCount() const;
  bool commandsEmpty() const;
  IRCCommand getCommand();

 private:
  void lexer(const std::string& command);
  void parser();

  void putToken(TokenType type, std::stringstream& token);
  void putToken(TokenType type);

  State m_state;
  TokenType m_last_token;

  std::queue<Token> m_tokens;

  IRCCommand m_command;
  std::queue<IRCCommand> m_commands;
};

#endif
