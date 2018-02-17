#include "ircbot/unexpected_character.hpp"

UnexpectedCharacter::UnexpectedCharacter(const std::string& given,
                                         const std::string& expected) :
  std::logic_error{std::string{"Unexpected character: "} +
                   given +
                   std::string{". Expected: "} +
                   expected} {
}
