set(SOURCES
  bot.cpp
)

pybind11_add_module(pyircbot ${SOURCES})
