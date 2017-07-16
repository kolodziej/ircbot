#ifndef _INTERPRETER_HPP
#define _INTERPRETER_HPP

#include "ircbot/interpreter_result.hpp"

class Interpreter {
 public:
  Interpreter() = default;

  InterpreterResult run(std::string message);

 private:

};

#endif
