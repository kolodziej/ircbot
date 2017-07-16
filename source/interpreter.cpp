#include "ircbot/interpreter.hpp"

#include <string>
#include <vector>
#include <stdexcept>

InterpreterResult Interpreter::run(std::string message) {
  enum class State {
    START,
    PREFIX,
    COMMAND,
    PARAMS,
    TRAILING,
    CR,
    CRLF,
  };

  State state = State::START;

  std::string command, param;
  std::vector<std::string> params;

  bool crlf = false;
  for (size_t i = 0; i < message.size() and not crlf;) {
    char x = message[i];

    switch (state) {
      case State::START:
        if (x == ':') {
          state = State::PREFIX;
          ++i;
        } else {
          state = State::COMMAND;
        }

        break;

      case State::PREFIX:
        if (x == ' ')
          state = State::COMMAND;

        ++i;
        break;

      case State::COMMAND:
        if ((x >= 'a' and x <= 'z') or (x >= 'A' and x <= 'Z'))
          command.push_back(x);
        else if (x >= '0' and x <= '9')
          command.push_back(x);
        else if (x == ' ')
          state = State::PARAMS;
        else
          throw std::logic_error{"could not parse COMMAND"};

        ++i;
        break;

      case State::PARAMS:
        if (x == ':') {
          state = State::TRAILING;
        } else if (x == '\r' or x == '\n') {
          params.push_back(param);
          if (x == '\r')
            state = State::CR;
          else if (x == '\n')
            state = State::CRLF;
        } else if (x == ' ') {
          params.push_back(param);
          param.clear();
        } else {
          param.push_back(x);
        }

        ++i;
        break;

      case State::TRAILING:
        if (x == '\0')
          throw std::logic_error{"unexpected character in TRAILING PARAM"};

        if (x == '\r' or x == '\n') {
          params.push_back(param);
          if (x == '\r')
            state = State::CR;
          else if (x == '\n')
            state = State::CRLF;
        } else {
          param.push_back(x);
        }

        ++i;
        break;

      case State::CR:
        if (x == '\n')
          state = State::CRLF;

        break;

      case State::CRLF:
        crlf = true;
        break;
    }
  }

  return InterpreterResult{command, params};
}
