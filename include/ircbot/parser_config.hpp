#ifndef _PARSER_CONFIG_HPP
#define _PARSER_CONFIG_HPP

namespace ircbot {

/** \class ParserConfig
 *
 * Carries configuration for CommandParser
 */

struct ParserConfig {
  /** prefix for command (first character in line) */
  char prefix;

  /** unused */
  bool parseOneCommand;
};

}  // namespace ircbot

#endif
