cmake_minimum_required(VERSION 3.0.0)

find_package(Threads REQUIRED)
find_package(Boost 1.53 REQUIRED COMPONENTS system program_options)

include(GNUInstallDirs)

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_BINARY_DIR}/include")
include_directories(${Boost_INCLUDE_DIR})

include_directories("${CMAKE_SOURCE_DIR}/gtest/include")

link_directories("${CMAKE_CURRENT_BINARY_DIR}/gtest/")
link_directories("${CMAKE_BINARY_DIR}/plugins/")

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -pedantic -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-DLOG_DEBUG -g")

include(${CMAKE_SOURCE_DIR}/cmake/version.conf)

file(REMOVE "${CMAKE_BINARY_DIR}/include/version_configuration.hpp")
configure_file("${CMAKE_SOURCE_DIR}/cmake/version_configuration.hpp.in"
               "${CMAKE_BINARY_DIR}/include/version_configuration.hpp")

set(SOURCES
    source/log_output.cpp
    source/log_level.cpp
    source/logger.cpp
    source/irc_parser.cpp
    source/irc_command.cpp
    source/client.cpp
    source/plugin.cpp
    source/helpers.cpp
    source/config.cpp
    source/version.cpp
    source/cout_log_output.cpp
    source/clog_log_output.cpp
    source/cerr_log_output.cpp
    source/file_log_output.cpp
    source/command_parser.cpp
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
    include/ircbot/helpers.hpp
    include/ircbot/config.hpp
    include/ircbot/version.hpp
    include/ircbot/cout_log_output.hpp
    include/ircbot/clog_log_output.hpp
    include/ircbot/cerr_log_output.hpp
    include/ircbot/file_log_output.hpp
    include/ircbot/parser_config.hpp
    include/ircbot/command_parser.hpp
)

add_library("ircbot" SHARED ${HEADERS} ${SOURCES})
target_link_libraries("ircbot" ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    ${BUILTIN_PLUGINS})

add_subdirectory(plugins)
add_subdirectory(program)

if(${TESTS})
    enable_testing()
    add_subdirectory(gtest)
    add_subdirectory(tests)
endif()

install(TARGETS ircbot
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
