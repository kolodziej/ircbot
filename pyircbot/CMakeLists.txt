set(SOURCES
  bot.cpp
)

pybind11_add_module(pyircbot ${SOURCES})
target_link_libraries(pyircbot PRIVATE ${Boost_LIBRARIES})
