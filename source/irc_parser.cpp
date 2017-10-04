#include "ircbot/irc_parser.hpp"

void IRCParser::parse(const std::string& message) {
  std::stringstream token;
  const char space = 0x20;
  const char cr = 0xd;
  const char lf = 0xa;

  for (size_t index = 0; index < message.size(); ++index) {
    const char& x = message[index];

    if (state == State::NONE) {
      if (x == ':') {
        state = State::SERVERNAME_NICK;
      }
    } else if (state == State::SERVERNAME_NICK) {
      if (x != space and x != '!' and x != '@') {
        token.put(x);
      } else if (x == space) {
        tokens.emplace_back(TokenType::SERVERNAME, token.str());
        token.str(std::string());
        state = State::COMMAND;
      } else if (x == '!' or x == '@') {
        tokens.emplace_back(TokenType::NICK, token.str());
        token.str(std::string());
        if (x == '!')
          state = State::USER;
        else if (x == '@')
          state = State::HOST;
        else if (x == space) 
          state = State::COMMAND;
      }
    } else if (state == State::USER) {
      if (x != space and x != '@') {
        token.put(x);
      } else {
        putToken(TokenType::USER, token);
        
        if (x == space) {
          state = State::COMMAND;
        else
          state = State::HOST;
      }
    } else if (state == State::HOST) {
      if (x != space) {
        token.put(x);
      } else {
        putToken(TokenType::HOST, token);
        state = State::COMMAND;
      }
    } else if (state == State::COMMAND) {
      if ('0' <= x and x <= '9') {
        token.put(x);
        state = State::COMMAND_NUM;
      } else if (('a' <= x and x <= 'z') or ('A' <= x and x <= 'Z')) {
        token.put(x);
        state = State::COMMAND_LETTER;
      }
    } else if (state == State::COMMAND_NUM) {
      if ('0' <= x and x <= '9') {
        token.put(x);
      } else if (x == space) {
        putToken(TokenType::COMMAND, token);
        state = State::PARAMS;
      } else {
        // error report!
      }
    } else if (state == State::COMMAND_LETTER) {
      if (('a' <= x and x <= 'z') or ('A' <= x and x <= 'Z')) {
        token.put(x);
      } else if (x == space) {
        putToken(TokenType::COMMAND, token);
        state = State::PARAMS;
      } else {
        // error report!
      }
    } else if (state == State::PARAMS) {
      if (x != ':' and x != space and x != '\0' and x != cr and x != lf) {
        token.put(x);
      } else if (x == ':') {
        state = State::TRAILING;
      } else if (x == space) {
        putToken(TokenType::PARAM, token);
      } else if (x == cr) {
        putToken(TokenType::CR);
      } else if (x == lf) {
        putToken(TokenType::LF);
      }
    } else if (state == State::TRAILING) {
      if (x != '\0' and x != cr and x != lf) {
        token.put(x);
      } else if (x == cr) {
        putToken(TokenType::CR);
      } else if (x == lf) {
        putToken(TokenType::LF);
      } else {
        // report error!
      }
    }
  }
}

void IRCParser::putToken(TokenType type, std::stringstream& token) {
  tokens.emplace_back(type, token.str());
  token.str(std::string());
}

void IRCParser::putToken(TokenType type) {
  tokens.emplace_back(type);
}
