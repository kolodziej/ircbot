#include "ircbot/irc_client.hpp"

#include <sstream>
#include <stdexcept>

#include "ircbot/logger.hpp"

std::string IRCClient::initialize(std::string name,
                                  std::string server,
                                  std::string realname) {
  
  std::stringstream stream;
  stream << "NICK " << name << "\r\n";
  stream << "USER " << name << " " << server
      << " " << server << " :" << realname << "\r\n";

  return stream.str();
}

std::string IRCClient::getResponse(const IRCInterpreterResult& r) {
  Logger& logger = Logger::getInstance();

  if (r.command == "PING") {
    logger(LogLevel::DEBUG, "Matched PONG response to: ", r);
    return ping(r);
  } else {
    logger(LogLevel::DEBUG, "Could not match IRC response to: ", r);
  }

  return std::string();
}

std::string IRCClient::ping(const IRCInterpreterResult& r) {
  if (r.params.size() != 1)
    throw std::logic_error{"UNKNOWN PARAMS IN PING COMMAND"};

  std::stringstream stream;
  stream << "PONG :" << r.params[0] << "\r\n";
  return stream.str();
}
