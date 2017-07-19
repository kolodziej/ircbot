#include "ircbot/irc_interpreter.hpp"

#include <string>
#include <vector>
#include <stdexcept>

#include "ircbot/logger.hpp"

IRCInterpreter::IRCInterpreter() :
    m_state{State::START}
{}

size_t IRCInterpreter::parse(std::string message) {
  Logger& logger = Logger::getInstance();
  logger(LogLevel::DEBUG, "Running IRC parser for message of size ", message.size());
  logger(LogLevel::DEBUG, "Message content: ", message);

  size_t added_results = 0;

  size_t index = 0;
  while (index < message.size()) {
    char x = message[index];

    switch (m_state) {
      case State::START:
        logger(LogLevel::DEBUG, "Interpreter state machine: START");
        if (x == ':') {
          m_state = State::PREFIX;
          logger(LogLevel::DEBUG, "Interpreter state machine: transpose to PREFIX");
          ++index;
        } else {
          m_state = State::COMMAND;
          logger(LogLevel::DEBUG, "Interpreter state machine: transpose to COMMAND");
        }

        break;

      case State::PREFIX:
        if (x == ' ') {
          m_state = State::COMMAND;
          logger(LogLevel::DEBUG, "Interpreter state machine: transpose to COMMAND");
        }

        ++index;
        break;

      case State::COMMAND:
        if ((x >= 'a' and x <= 'z') or (x >= 'A' and x <= 'Z')) {
          m_command.push_back(x);
        } else if (x >= '0' and x <= '9') {
          m_command.push_back(x);
        } else if (x == ' ') {
          m_state = State::PARAMS;
          logger(LogLevel::DEBUG, "Interpreter state machine: transpose to PARAMS");
        } else {
          throw std::logic_error{"could not parse COMMAND"};
        }

        ++index;
        break;

      case State::PARAMS:
        if (x == ':') {
          m_state = State::TRAILING;
          logger(LogLevel::DEBUG, "Interpreter state machine: transpose to TRAILING");
        } else if (x == '\r' or x == '\n') {
          m_params.push_back(m_param);
          if (x == '\r') {
            m_state = State::CR;
            logger(LogLevel::DEBUG, "Interpreter state machine: transpose to CR");
          } else if (x == '\n') {
            m_state = State::CRLF;
            logger(LogLevel::DEBUG, "Interpreter state machine: transpose to CRLF");
          }
        } else if (x == ' ') {
          m_params.push_back(m_param);
          m_param.clear();
        } else {
          m_param.push_back(x);
        }

        ++index;
        break;

      case State::TRAILING:
        if (x == '\0')
          throw std::logic_error{"unexpected character in TRAILING PARAM"};

        if (x == '\r' or x == '\n') {
          m_params.push_back(m_param);
          if (x == '\r') {
            m_state = State::CR;
            logger(LogLevel::DEBUG, "Interpreter state machine: transpose to CR");
          } else if (x == '\n') {
            m_state = State::CRLF;
            logger(LogLevel::DEBUG, "Interpreter state machine: transpose to CRLF");
          }
        } else {
          m_param.push_back(x);
        }

        ++index;
        break;

      case State::CR:
        if (x == '\n') {
          m_state = State::CRLF;
          logger(LogLevel::DEBUG, "Interpreter state machine: transpose to CRLF");
        } else {
          ++index;
        }

        break;

      case State::CRLF:
        ++index;
        m_results.emplace_back(m_command, m_params);
        ++added_results;

        m_params.clear();
        m_param.clear();
        m_command.clear();
        m_state = State::START;
        logger(LogLevel::DEBUG, "Interpreter state machine: transpose to START");
        break;
    }
  }

  return added_results;
}

IRCInterpreterResult IRCInterpreter::nextResult() {
  auto x = m_results.front();
  m_results.pop_front();
  return x;
}

size_t IRCInterpreter::resultsNumber() const {
  return m_results.size();
}
