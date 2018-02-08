#include "ircbot/command_parser.hpp"

#include "ircbot/logger.hpp"
#include "ircbot/helpers.hpp"

CommandParser::CommandParser(ParserConfig config) :
    m_config{config},
    m_state{State::START},
    m_last_token{TokenType::NONE} {
}

void CommandParser::parse(const std::string& command) {
  lexer(command);
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

void CommandParser::lexer(const std::string& message) {
  using helpers::isIn;

  std::stringstream token;
  const char space = 0x20;
  const char cr = 0xd;
  const char lf = 0xa;
  const char quote = '\'';
  const char dquote = '\"';
  const char escape = '\\';

  bool is_dquote{false};
  bool is_quoted{false};
  int do_escape{0};

  for (size_t index = 0; index < message.size(); ++index) {
    const char x = message[index];

    if (m_state == State::START) {
      if (x == m_config.prefix) {
        m_state = State::PREFIX;
        DEBUG("Lexer transforming state to PREFIX");
      } else {
        unexpectedCharacter(x, std::string{1, m_config.prefix});
      }
    } else if (m_state == State::PREFIX) {
      if (isCommandCharacter(x)) {
        m_state = State::COMMAND;
        DEBUG("Lexer transforming state to COMMAND");
        token.put(x);
      } else {
        unexpectedCharacter(x, "a-zA-Z0-9-");
      }
    } else if (m_state == State::COMMAND) {
      if (isCommandCharacter(x)) {
        token.put(x);
      } else if (x == space) {
        m_state = State::ARGUMENT;
        DEBUG("Lexer transforming state to ARGUMENT");
        putToken(TokenType::COMMAND, token);
      } else if (x == cr) {
        DEBUG("Ignoring CR character");
      } else if (x == lf) {
        m_state = State::START;
        putToken(TokenType::COMMAND, token);
      } else {
        unexpectedCharacter(x, "a-zA-Z0-9-");
      }
    } else if (m_state == State::ARGUMENT) {
      if (do_escape <= 0) {
        if (x == escape) {
          do_escape = 2;
          DEBUG("Escaping next character");
        } else if (x == dquote or x == quote) {
          if ((is_dquote and x == dquote) or
              (not is_dquote and x == quote)) {
            if (is_quoted) {
              DEBUG("Exiting quoted area");
            } else {
              DEBUG("Entering quoted area");
            }

            is_quoted = !is_quoted;
            is_dquote = (x == dquote);
          } else {
            token.put(x);
          }
        } else if (x == space and not is_quoted) {
          DEBUG("Adding token ARGUMENT: ", token.str());
          putToken(TokenType::ARGUMENT, token);
        } else if (x == cr) {
          DEBUG("Ignoring CR character");
        } else if (x == lf) {
          m_state = State::START;
          DEBUG("Adding token ARGUMENT: ", token.str());
          putToken(TokenType::ARGUMENT, token);
        } else {
          token.put(x);
        }
      } else {
        if (x != '\0') {
          token.put(x);
        } else {
          unexpectedCharacter('X', "any other");
        }
      }
    } else if (m_state == State::UNEXPECTED_CHARACTER) {
      if (x == cr) {
        DEBUG("Ignoring CR character");
      } else if (x == lf) {
        m_state = State::START; 
      }
    }

    --do_escape;
  }
}

void CommandParser::parser() {

}

bool CommandParser::isCommandCharacter(char x) {
  return ((x >= 'a' and x <= 'z') or
          (x >= 'A' and x <= 'Z') or
          (x >= '0' and x <= '9') or
          (x == '-'));
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
