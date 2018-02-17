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
  };

  struct Token {
    TokenType type;
    std::string value;
  };

  struct Command {
    std::string command;
    std::vector<std::string> arguments;
    std::vector<std::string> additional_arguments;
  };

  CommandParser(ParserConfig config);

  Command parse(const std::string& command);
  ParserConfig getConfig() const;

 private:
  bool isCommandCharacter(char x);

  ParserConfig m_config;
  State m_state;
};

#endif
