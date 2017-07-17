#include "ircbot/irc_interpreter_result.hpp"

std::ostream& operator<<(std::ostream& stream,
                         const IRCInterpreterResult& rhs) {
  stream << "{Command: " << rhs.command << "; "
         << "Params: ";

  for (const auto& x : rhs.params)
    stream << "\'" << x << "\' ";

  return stream << "}";
}
