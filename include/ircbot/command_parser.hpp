#ifndef _COMMAND_PARSER_HPP
#define _COMMAND_PARSER_HPP

#include <string>
#include <vector>
#include <queue>

#include "ircbot/parser_config.hpp"

class CommandParser {
 public:
  enum class TokenType {
    COMMAND,
    ARGUMENT,
    END,
    IGNORE
  };

  enum class State {
    START = 0,
    PREFIX,
    COMMAND,
    ARGUMENT,
    UNEXPECTED_CHARACTER
  };

  struct Token {
    TokenType type;
    std::string value;
  };

  struct Command {
    std::string command;
    std::vector<std::string> arguments;
  };

  CommandParser(ParserConfig config);

  void parse(const std::string& command);

  size_t commandsCount() const;
  bool commandsEmpty() const;
  Command getCommand();

 private:
  void lexer(const std::string& command);
  void parser();

  bool isCommandCharacter(char x);

  void putToken(TokenType type);
  void putToken(TokenType type, std::stringstream& token);
  void unexpectedCharacter(char x, const std::string& expected);

  ParserConfig m_config;

  State m_state;
  TokenType m_last_token;

  std::queue<Token> m_tokens;

  Command m_command;
  std::queue<Command> m_commands;
};

#endif
