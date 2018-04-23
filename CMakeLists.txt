cmake_minimum_required(VERSION 3.0.0)

find_package(Threads REQUIRED)
find_package(Boost 1.53 REQUIRED COMPONENTS system program_options filesystem python)
find_package(Doxygen)
find_package(Protobuf REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)

# Workaround for different versions of CMake
# Some versions define PROTOBUF_* variables and some Protobuf_*
if(NOT DEFINED Protobuf_LIBRARIES)
    set(Protobuf_LIBRARIES ${PROTOBUF_LIBRARIES})
    set(Protobuf_INCLUDE_DIR ${PROTOBUF_INCLUDE_DIR})
endif()

include(GNUInstallDirs)

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_BINARY_DIR}/include")
include_directories("${CMAKE_BINARY_DIR}")
include_directories(${Boost_INCLUDE_DIR})
include_directories(${PYTHON_INCLUDE_DIR})
include_directories(${Protobuf_INCLUDE_DIR})

include_directories("${CMAKE_SOURCE_DIR}/gtest/include")

link_directories("${CMAKE_CURRENT_BINARY_DIR}/gtest/")
link_directories("${CMAKE_BINARY_DIR}/plugins/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DLOG_DEBUG -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

option(RESET_VERSION_INFO
    "Reset version information in version_configuration.hpp"
    OFF
)

include(${CMAKE_SOURCE_DIR}/cmake/version.cmake)

if (RESET_VERSION_INFO)
    file(REMOVE "${CMAKE_BINARY_DIR}/include/version_configuration.hpp")
endif()

configure_file("${CMAKE_SOURCE_DIR}/cmake/version_configuration.hpp.in"
               "${CMAKE_BINARY_DIR}/include/version_configuration.hpp")
configure_file("${CMAKE_SOURCE_DIR}/cmake/version-info.in"
               "${CMAKE_BINARY_DIR}/version-info")

option(BUILD_TESTS "Build tests" ON)
option(BUILD_PLUGINS "Build plugins from plugins directory" ON)

set(PROTOBUF_FILES
    protobuf/message.proto
    protobuf/init.proto
    protobuf/irc_message.proto
    protobuf/control.proto
)

set(SOURCES
    source/log_output.cpp
    source/log_level.cpp
    source/logger.cpp
    source/irc_parser.cpp
    source/irc_message.cpp
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
    source/so_plugin.cpp
    source/unexpected_character.cpp
    source/admin_port.cpp
    source/tcp_plugin.cpp
    source/tcp_plugin_server.cpp
)

set(HEADERS
    include/ircbot/log_level.hpp
    include/ircbot/log_output.hpp
    include/ircbot/logger.impl.hpp
    include/ircbot/logger.hpp
    include/ircbot/logger_macros.hpp
    include/ircbot/irc_parser.hpp
    include/ircbot/irc_message.hpp
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
    include/ircbot/so_plugin.hpp
    include/ircbot/unexpected_character.hpp
    include/ircbot/plugin_config.hpp
    include/ircbot/admin_port.hpp
    include/ircbot/tcp_plugin.hpp
    include/ircbot/tcp_plugin_server.hpp
)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTOBUF_FILES})
protobuf_generate_python(PROTO_PY ${PROTOBUF_FILES})

add_library("ircbot" SHARED ${HEADERS} ${SOURCES} ${PROTO_SRC} ${PROTO_HDR})
target_link_libraries("ircbot"
    ${Boost_LIBRARIES}
    ${Protobuf_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${BUILTIN_PLUGINS}
)

add_custom_target(PythonProtobuf ALL DEPENDS ${PROTO_PY})

if(${BUILD_PLUGINS})
    add_subdirectory(plugins)
endif()

add_subdirectory(program)

if(${BUILD_TESTS})
    enable_testing()
    add_subdirectory(gtest)
    add_subdirectory(tests)
endif()

if(DOXYGEN_FOUND)
    add_subdirectory(docs)
endif()

install(TARGETS ircbot
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
