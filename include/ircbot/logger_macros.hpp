#ifndef _LOGGER_MACROS_HPP
#define _LOGGER_MACROS_HPP

#ifdef LOG_DEBUG

#define DEBUG(...)                                           \
  ircbot::Logger::m_logger_instance(ircbot::LogLevel::DEBUG, \
                                    __PRETTY_FUNCTION__, ": ", __VA_ARGS__);

#else

#define DEBUG(...)

#endif

#define LOG(level, ...)                                      \
  ircbot::Logger::m_logger_instance(ircbot::LogLevel::level, \
                                    __PRETTY_FUNCTION__, ": ", __VA_ARGS__);

#endif
