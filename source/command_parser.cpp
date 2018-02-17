#include "ircbot/command_parser.hpp"

#include "ircbot/logger.hpp"
#include "ircbot/helpers.hpp"
#include "ircbot/unexpected_character.hpp"

CommandParser::CommandParser(ParserConfig config) :
    m_config{config} {
}

CommandParser::Command CommandParser::parse(const std::string& command) {
  using helpers::isIn;

  const char space = 0x20;
  const char dquote = '"';
  const char quote = '\'';
  const char escape = '\\';
  const char cr = 0xd;
  const char lf = 0xa;

  int do_escape{0};
  bool is_quoted{false};
  bool is_dquote{false};

  State state = State::START;
  std::stringstream token;

  Command cmd;

  DEBUG("Parser initialized with START state");

  if (m_config.prefix == '\0') {
    DEBUG("Transforming parser to COMMAND state (without prefix)");
    state = State::COMMAND;
  }

  for (size_t i = 0; i < command.size(); ++i) {
    char x = command[i];

    if (state == State::START) {
      if (x == m_config.prefix) {
        DEBUG("Parser transforms to COMMAND state");
        state = State::COMMAND;
      } else {
        throw UnexpectedCharacter(std::string{1, x},
                                  std::string{1, m_config.prefix});
      }
    } else if (state == State::COMMAND) {
      if (isCommandCharacter(x)) {
        token.put(x);
      } else if (x == space) {
        cmd.command = token.str();
        token.str(std::string{});
        state = State::ARGUMENT;
        DEBUG("Parser transforms to ARGUMENT state");
      } else if (x == cr or x == lf) {
        DEBUG("Ignoring CR or LF character");
      } else {
        throw UnexpectedCharacter(std::string{1, x},
                                  "a-zA-Z0-9-");
      }
    } else if (state == State::ARGUMENT) {
      if (do_escape > 0) {
        token.put(x);
        DEBUG("Appending escaped character: ", x);
      } else if (x == escape) {
        do_escape = 2; // escape this character and next one
        DEBUG("Setting escape");
      } else if (x == quote) {
        if (is_quoted and not is_dquote) {
          is_quoted = false;
          DEBUG("Leaving quoted argument");
        } else if (not is_quoted) {
          is_quoted = true;
          is_dquote = false;
          DEBUG("Entering quoted argument");
        } else {
          token.put(x);
          DEBUG("Appending quote in quoted area");
        }
      } else if (x == dquote) {
        if (is_quoted and is_dquote) {
          is_quoted = false;
          DEBUG("Leaving double quoted area");
        } else if (not is_quoted) {
          is_quoted = true;
          is_dquote = true;
          DEBUG("Entering double quoted area");
        } else {
          token.put(x);
          DEBUG("Appending double quote in double quoted area");
        }
      } else if (is_quoted) {
        token.put(x);
        DEBUG("Appending character in quoted area: ", x);
      } else if (x == cr) {
        DEBUG("Ignoring CR character");
      } else if (x == space or x == lf) {
        cmd.arguments.push_back(token.str());
        DEBUG("Creating new argument: ", token.str());
        token.str(std::string{});
      } else {
        token.put(x);
        DEBUG("Appending character: ", x);
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

ParserConfig CommandParser::getConfig() const {
  return m_config;
}

bool CommandParser::isCommandCharacter(char x) {
  return ((x >= 'a' and x <= 'z') or
          (x >= 'A' and x <= 'Z') or
          (x >= '0' and x <= '9') or
          (x == '-'));
}
