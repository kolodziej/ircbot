#ifndef _UNEXPECTED_CHARACTER_HPP
#define _UNEXPECTED_CHARACTER_HPP

#include <stdexcept>
#include <string>

struct UnexpectedCharacter : std::logic_error {
  UnexpectedCharacter(const std::string& given,
                      const std::string& excpected);
};

#endif
