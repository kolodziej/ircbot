cmake_minimum_required(VERSION 3.0.0)

find_package(Threads REQUIRED)
find_package(Boost 1.62 REQUIRED COMPONENTS system program_options)

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories(${Boost_INCLUDE_DIR})

include_directories("${CMAKE_SOURCE_DIR}/gtest/include")
link_directories("${CMAKE_CURRENT_BINARY_DIR}/gtest/")

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -pedantic -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-DLOG_DEBUG -g")

set(SOURCES
    source/log_output.cpp
    source/log_level.cpp
    source/logger.cpp
    source/irc_parser.cpp
    source/irc_command.cpp
    source/client.cpp
    source/plugin.cpp
    source/init_plugin.cpp
    source/ping_plugin.cpp
    source/plugin_manager.cpp
    source/helpers.cpp
)

set(HEADERS
    include/ircbot/log_level.hpp
    include/ircbot/log_output.hpp
    include/ircbot/logger.impl.hpp
    include/ircbot/logger.hpp
    include/ircbot/logger_macros.hpp
    include/ircbot/irc_parser.hpp
    include/ircbot/irc_command.hpp
    include/ircbot/client.hpp
    include/ircbot/plugin.hpp
    include/ircbot/init_plugin.hpp
    include/ircbot/ping_plugin.hpp
    include/ircbot/plugin_manager.hpp
    include/ircbot/helpers.hpp
)

add_library("ircbot" SHARED ${HEADERS} ${SOURCES})
target_link_libraries("ircbot" ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(program)
add_subdirectory(plugins)

if(${TESTS})
    add_subdirectory(gtest)
    add_subdirectory(tests)
endif()
