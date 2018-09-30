set(SOURCES
  bot.cpp
  plugin.cpp
  helpers.cpp
  pyircbot.cpp
)

set(HEADERS
  pyircbot/bot.hpp
  pyircbot/plugin.hpp
  pyircbot/helpers.hpp
  pyircbot/pyircbot.hpp
)

pybind11_add_module(pyircbot ${SOURCES})
target_link_libraries(pyircbot PRIVATE
  ircbot yaml-cpp
  ${Boost_LIBRARIES}
  ${Protobuf_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)

message(STATUS "Python Version: ${PYTHON_VERSION_STRING}")
if(NOT DEFINED PYTHON_INSTALL_PATH)
    execute_process(COMMAND /usr/bin/env python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}
        ${CMAKE_CURRENT_SOURCE_DIR}/pythonpath.py ${PYTHON_VERSION_STRING}
        OUTPUT_VARIABLE PYTHON_INSTALL_PATH)

    string(STRIP "${PYTHON_INSTALL_PATH}" PYTHON_INSTALL_PATH)
    message(STATUS "Found pyircbot module installation path: ${PYTHON_INSTALL_PATH}")
else()
    message(STATUS "pyircbot module installation path: ${PYTHON_INSTALL_PATH}")
endif()


install(TARGETS pyircbot
        LIBRARY DESTINATION ${PYTHON_INSTALL_PATH})
