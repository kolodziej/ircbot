CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

INCLUDE_DIRECTORIES(
    "${CMAKE_SOURCE_DIR}/include"
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

SET(SOURCES
    source/log_output.cpp
    source/log_level.cpp
    source/logger.cpp
    source/main.cpp
)

SET(HEADERS
    include/ircbot/log_level.hpp
    include/ircbot/log_output.hpp
    include/ircbot/logger.impl.hpp
    include/ircbot/logger.hpp
)

ADD_EXECUTABLE("ircbot" ${SOURCES})
