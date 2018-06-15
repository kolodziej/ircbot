#ifndef _UNEXPECTED_CHARACTER_HPP
#define _UNEXPECTED_CHARACTER_HPP

#include <stdexcept>
#include <string>

namespace ircbot {

/** \class UnexpectedCharacter
 *
 * \brief Exception thrown on unexpected character
 */

struct UnexpectedCharacter : std::logic_error {
  /** Constructor
   *
   * \param given given character (may be descriptive, eg. CR, LF)
   * \param expected character (may be descriptive, range, list of possible
   * characters)
   */
  UnexpectedCharacter(const std::string& given,
                      const std::string& excpected);
};

} // namespace ircbot

#endif
