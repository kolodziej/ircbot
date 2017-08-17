CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

FIND_PACKAGE(Threads)
FIND_PACKAGE(Boost 1.62 COMPONENTS system program_options REQUIRED)

INCLUDE_DIRECTORIES(
    "${CMAKE_SOURCE_DIR}/include"
    "${Boost_INCLUDE_DIR}"
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -Wextra")

SET(SOURCES
    source/log_output.cpp
    source/log_level.cpp
    source/logger.cpp
    source/session.cpp
    source/bot.cpp
    source/irc_interpreter.cpp
    source/irc_command.cpp
    source/plugin_manager.cpp
    source/plugin.cpp
    source/main.cpp
)

SET(HEADERS
    include/ircbot/log_level.hpp
    include/ircbot/log_output.hpp
    include/ircbot/logger.impl.hpp
    include/ircbot/logger.hpp
    include/ircbot/session.hpp
    include/ircbot/bot_config.hpp
    include/ircbot/bot.hpp
    include/ircbot/irc_interpreter.hpp
    include/ircbot/irc_command.hpp
    include/ircbot/plugin.hpp
    include/ircbot/plugin_manager.hpp
)

ADD_EXECUTABLE("ircbot" ${SOURCES})
TARGET_LINK_LIBRARIES("ircbot" "${Boost_LIBRARIES}" "${CMAKE_THREAD_LIBS_INIT}")
