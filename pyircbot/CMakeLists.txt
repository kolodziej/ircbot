set(SOURCES
  bot.cpp
  plugin.cpp
  pyircbot.cpp
)

pybind11_add_module(pyircbot ${SOURCES})
target_link_libraries(pyircbot PRIVATE
  ircbot
  ${Boost_LIBRARIES}
  ${Protobuf_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)
