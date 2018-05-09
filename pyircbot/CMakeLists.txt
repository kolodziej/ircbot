set(SOURCES
  bot.cpp
  plugin.cpp
  pyircbot.cpp
)

pybind11_add_module(pyircbot ${SOURCES})
target_link_libraries(pyircbot PRIVATE ${Boost_LIBRARIES})
