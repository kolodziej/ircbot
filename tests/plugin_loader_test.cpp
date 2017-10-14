#include "gtest/gtest.h"

#include <iostream>
#include <stdexcept>

#include "ircbot/client.hpp"
#include "ircbot/logger.hpp"

#include "ircbot/plugin_loader.hpp"

TEST(PluginLoader, Basic) {
  Logger& logger = Logger::getInstance();
  logger.addOutput(LogOutput{std::cerr, LogLevel::ERROR});

  PluginLoader loader{"./plugins"};
  auto ptr = loader.loadPlugin("libhelloworld.so");
  ASSERT_NE(ptr, nullptr);
}
