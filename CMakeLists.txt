CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(Boost 1.62 REQUIRED COMPONENTS system program_options)

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/gtest/include")
LINK_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}/gtest/")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

SET(SOURCES
    source/log_output.cpp
    source/log_level.cpp
    source/logger.cpp
    source/irc_parser.cpp
    source/irc_command.cpp
    source/client.cpp
)

SET(HEADERS
    include/ircbot/log_level.hpp
    include/ircbot/log_output.hpp
    include/ircbot/logger.impl.hpp
    include/ircbot/logger.hpp
    include/ircbot/irc_parser.hpp
    include/ircbot/irc_command.hpp
    include/ircbot/client.hpp
)

ADD_LIBRARY("ircbot" SHARED ${HEADERS} ${SOURCES})
TARGET_LINK_LIBRARIES("ircbot" ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})


IF(${TESTS})
    ADD_SUBDIRECTORY(gtest)
    ADD_SUBDIRECTORY(tests)
ENDIF()
