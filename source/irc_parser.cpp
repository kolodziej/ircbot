#include "ircbot/irc_parser.hpp"

#include "ircbot/helpers.hpp"
#include "ircbot/logger.hpp"

namespace ircbot {

IRCParser::IRCParser() : m_state{State::NONE}, m_last_token{TokenType::LF} {}

void IRCParser::parse(const std::string& message) { lexer(message); }

size_t IRCParser::messagesCount() const { return m_messages.size(); }

bool IRCParser::messagesEmpty() const { return m_messages.empty(); }

IRCMessage IRCParser::getMessage() {
  IRCMessage msg = m_messages.front();
  m_messages.pop();
  return msg;
}

void IRCParser::lexer(const std::string& message) {
  std::stringstream token;
  const char space = 0x20;
  const char cr = 0xd;
  const char lf = 0xa;
  using helpers::removeLineFeed;

  DEBUG("Running lexer for message: ", removeLineFeed(message));

  for (size_t index = 0; index < message.size(); ++index) {
    const char& x = message[index];

    if (m_state == State::NONE) {
      if (x == ':') {
        m_state = State::SERVERNAME_NICK;
        DEBUG("Lexer transforming state to SERVERNAME_NICK");
      } else {
        token.put(x);
        m_state = State::COMMAND;
        DEBUG("Lexer transforming state to COMMAND");
      }
    } else if (m_state == State::SERVERNAME_NICK) {
      if (x != space and x != '!' and x != '@') {
        token.put(x);
      } else if (x == space) {
        putToken(TokenType::SERVERNAME, token);
        m_state = State::COMMAND;
        DEBUG("Lexer transforming state to COMMAND");
      } else if (x == '!' or x == '@') {
        putToken(TokenType::NICK, token);
        if (x == '!') {
          m_state = State::USER;
          DEBUG("Lexer transforming state to USER");
        } else if (x == '@') {
          m_state = State::HOST;
          DEBUG("Lexer transforming state to HOST");
        } else if (x == space) {
          m_state = State::COMMAND;
          DEBUG("Lexer transforming state to COMMAND");
        }
      }
    } else if (m_state == State::USER) {
      if (x != space and x != '@') {
        token.put(x);
      } else {
        putToken(TokenType::USER, token);

        if (x == space) {
          m_state = State::COMMAND;
          DEBUG("Lexer transforming state to COMMAND");
        } else {
          m_state = State::HOST;
          DEBUG("Lexer transforming state to HOST");
        }
      }
    } else if (m_state == State::HOST) {
      if (x != space) {
        token.put(x);
      } else {
        putToken(TokenType::HOST, token);
        m_state = State::COMMAND;
        DEBUG("Lexer transforming state to COMMAND");
      }
    } else if (m_state == State::COMMAND) {
      if ('0' <= x and x <= '9') {
        token.put(x);
        m_state = State::COMMAND_NUM;
        DEBUG("Lexer transforming state to COMMAND_NUM");
      } else if (('a' <= x and x <= 'z') or ('A' <= x and x <= 'Z')) {
        token.put(x);
        m_state = State::COMMAND_LETTER;
        DEBUG("Lexer transforming state to COMMAND_LETTER");
      }
    } else if (m_state == State::COMMAND_NUM) {
      if ('0' <= x and x <= '9') {
        token.put(x);
      } else if (x == space) {
        putToken(TokenType::COMMAND, token);
        m_state = State::PARAMS;
        DEBUG("Lexer transforming state to PARAMS");
      } else {
        // error report!
      }
    } else if (m_state == State::COMMAND_LETTER) {
      if (('a' <= x and x <= 'z') or ('A' <= x and x <= 'Z')) {
        token.put(x);
      } else if (x == space) {
        putToken(TokenType::COMMAND, token);
        m_state = State::PARAMS;
        DEBUG("Lexer transforming state to PARAMS");
      } else {
        // error report!
      }
    } else if (m_state == State::PARAMS) {
      if (x != ':' and x != space and x != '\0' and x != cr and x != lf) {
        token.put(x);
      } else if (x == ':') {
        m_state = State::TRAILING;
        DEBUG("Lexer transforming state to TRAILING");
      } else if (x == space) {
        if (not token.str().empty()) putToken(TokenType::PARAM, token);
      } else if (x == cr) {
        if (not token.str().empty()) putToken(TokenType::PARAM, token);

        putToken(TokenType::CR);
        m_state = State::CR;
        DEBUG("Lexer transforming state to CR");
      } else if (x == lf) {
        if (not token.str().empty()) putToken(TokenType::PARAM, token);

        putToken(TokenType::LF);
        m_state = State::LF;
        DEBUG("Lexer transforming state to LF");
      }
    } else if (m_state == State::TRAILING) {
      if (x != '\0' and x != cr and x != lf) {
        token.put(x);
      } else if (x == cr or x == lf) {
        putToken(TokenType::PARAM, token);

        if (x == cr) {
          putToken(TokenType::CR);
          m_state = State::CR;
          DEBUG("Lexer transforming state to CR");
        } else {
          putToken(TokenType::LF);
          m_state = State::LF;
          DEBUG("Lexer transforming state to LF");
        }
      } else {
        // report error!
      }
    } else if (m_state == State::CR) {
      if (x == lf) {
        putToken(TokenType::LF);
        m_state = State::LF;
        DEBUG("Lexer transforming state to LF");
      } else {
        // report error!
      }
    }

    if (m_state == State::LF) {
      parser();
      m_state = State::NONE;
      DEBUG("Lexer transforming state to NONE");
    }
  }
}

void IRCParser::parser() {
  DEBUG("Running parser, tokens to process: ", m_tokens.size());

  while (not m_tokens.empty()) {
    Token token = m_tokens.front();
    TokenType type = token.type;
    m_tokens.pop();

    if (m_last_token == TokenType::LF) {
      if (type == TokenType::SERVERNAME) {
        m_message.servername = token.value;
        DEBUG("Parser is setting SERVERNAME");
      } else if (type == TokenType::NICK) {
        m_message.nick = token.value;
        DEBUG("Parser is setting NICK");
      } else if (type == TokenType::COMMAND) {
        m_message.command = token.value;
        DEBUG("Parser is setting COMMAND");
      } else {
        // report error!
      }
    } else if (m_last_token == TokenType::SERVERNAME) {
      if (type == TokenType::COMMAND) {
        m_message.command = token.value;
      } else {
        // report error!
      }
    } else if (m_last_token == TokenType::NICK) {
      if (type == TokenType::USER) {
        m_message.user = token.value;
        DEBUG("Parser is setting USER");
      } else if (type == TokenType::HOST) {
        m_message.host = token.value;
        DEBUG("Parser is setting HOST");
      } else if (type == TokenType::COMMAND) {
        m_message.command = token.value;
        DEBUG("Parser is setting COMMAND");
      } else {
        // report error!
      }
    } else if (m_last_token == TokenType::USER) {
      if (type == TokenType::HOST) {
        m_message.host = token.value;
      } else if (type == TokenType::COMMAND) {
        m_message.command = token.value;
      } else {
        // report error!
      }
    } else if (m_last_token == TokenType::HOST) {
      if (type == TokenType::COMMAND) {
        m_message.command = token.value;
      } else {
        // report error!
      }
    } else if (m_last_token == TokenType::COMMAND or
               m_last_token == TokenType::PARAM) {
      if (type == TokenType::PARAM) {
        m_message.params.push_back(token.value);
        DEBUG("Parser is adding new PARAM");
      } else if (type == TokenType::LF) {
        m_messages.push(m_message);
        DEBUG("Next command parsed");
        m_message = IRCMessage{};
      } else if (type != TokenType::CR) {
        // report error!
      }
    } else if (m_last_token == TokenType::CR) {
      if (type == TokenType::LF) {
        m_messages.push(m_message);
        DEBUG("Next command parsed");
        m_message = IRCMessage{};
      }
    }

    m_last_token = type;
  }
}

void IRCParser::putToken(TokenType type, std::stringstream& token) {
  m_tokens.emplace(type, token.str());
  token.str(std::string());
}

void IRCParser::putToken(TokenType type) { m_tokens.emplace(type); }

}  // namespace ircbot
