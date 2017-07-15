CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

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
    source/main.cpp
)

SET(HEADERS
    include/ircbot/bot_config.hpp
    include/ircbot/log_level.hpp
    include/ircbot/log_output.hpp
    include/ircbot/logger.impl.hpp
    include/ircbot/logger.hpp
)

ADD_EXECUTABLE("ircbot" ${SOURCES})
TARGET_LINK_LIBRARIES("ircbot" "${Boost_LIBRARIES}")
