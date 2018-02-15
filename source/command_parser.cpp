#include "ircbot/command_parser.hpp"

#include "ircbot/logger.hpp"
#include "ircbot/helpers.hpp"

CommandParser::CommandParser(ParserConfig config) :
    m_config{config},
    m_state{State::START},
    m_last_token{TokenType::END} {
}

CommandParser::Command CommandParser::parse(const std::string& command) {
  using helpers::isIn;

  const char space = 0x20;
  const char cr = 0xd;
  const char lf = 0xa;
  const char dquote = '"';
  const char quote = '\'';
  const char escape = '\\';

  int do_escape{0};
  bool is_quoted{false};
  bool is_dquote{false};

  State state = State::START;
  std::stringstream token;

  Command cmd;

  DEBUG("Parser initialized with START state");
  for (size_t i = 0; i < command.size(); ++i) {
    char x = command[i];

    if (state == State::START) {
      if (x == m_config.prefix) {
        DEBUG("Parser transforms to COMMAND state");
        state = State::COMMAND;
      } else {
        unexpectedCharacter(x, std::string{1, m_config.prefix});
      }
    } else if (state == State::COMMAND) {
      if (isCommandCharacter(x)) {
        token.put(x);
      } else if (x == space) {
        cmd.command = token.str();
        token.str(std::string{});
        state = State::ARGUMENT;
        DEBUG("Parser transforms to ARGUMENT state");
      } else {
        unexpectedCharacter(x, "a-zA-Z0-9-");
      }
    } else if (state == State::ARGUMENT) {
      if (do_escape > 0) {
        token.put(x);
      } else if (x == quote) {
        if (is_quoted and not is_dquote) {
          is_quoted = false;
        } else if (not is_quoted) {
          is_quoted = true;
          is_dquote = false;
        } else {
          token.put(x);
        }
      } else if (x == dquote) {
        if (is_quoted and is_dquote) {
          is_quoted = false;
        } else if (not is_quoted) {
          is_quoted = true;
          is_dquote = true;
        } else {
          token.put(x);
        }
      } else if (is_quoted) {
        token.put(x);
      } else if (x == escape) {
        do_escape = 2; // escape this character and next one
      } else if (x == space) {
        cmd.arguments.push_back(token.str());
        token.str(std::string{});
      } else {
        token.put(x);
      }
    }

    --do_escape;
  }

  if (not token.str().empty()) {
    if (state == State::COMMAND) {
      cmd.command = token.str();
    } else if (state == State::ARGUMENT) {
      cmd.arguments.push_back(token.str());
    }
  }

  return cmd;
}

size_t CommandParser::commandsCount() const {
  return m_commands.size();
}

bool CommandParser::commandsEmpty() const {
  return m_commands.empty();
}

CommandParser::Command CommandParser::getCommand() {
  Command cmd = m_commands.front();
  m_commands.pop();
  return cmd;
}

ParserConfig CommandParser::getConfig() const {
  return m_config;
}

bool CommandParser::isCommandCharacter(char x) {
  return ((x >= 'a' and x <= 'z') or
          (x >= 'A' and x <= 'Z') or
          (x >= '0' and x <= '9') or
          (x == '-'));
}

void CommandParser::putToken(TokenType type) {
  m_tokens.push(Token{type, {}});
}

void CommandParser::putToken(TokenType type, std::stringstream& token) {
  m_tokens.push(Token{type, token.str()});
  token.str(std::string());
}

void CommandParser::unexpectedCharacter(char x, const std::string& expected) {
  LOG(ERROR, "Unexpected character: ", x, ". Expected: ", expected);
  m_state = State::UNEXPECTED_CHARACTER;
  DEBUG("Lexer transforming state to UNEXPECTED_CHARACTER");
}
