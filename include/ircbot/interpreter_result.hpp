#ifndef _INTERPRETER_RESULT_HPP
#define _INTERPRETER_RESULT_HPP

#include <string>
#include <vector>

struct InterpreterResult {
  std::string command;
  std::vector<std::string> params;
};

#endif
