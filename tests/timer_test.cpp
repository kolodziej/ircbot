#include "gtest/gtest.h"

#include "ircbot/cerr_log_output.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/network/context_provider.hpp"
#include "ircbot/network/timer.hpp"

#include <chrono>
#include <string>
#include <thread>

#include <boost/asio.hpp>

using namespace ircbot;

namespace asio = boost::asio;

class TimerTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    Logger& logger = Logger::getInstance();
    logger.addOutput(std::make_unique<CerrLogOutput>(LogLevel::DEBUG));
  }

  virtual void TearDown() { Logger::getInstance().clearOutputs(); }
};

void f(network::Timer::Result result) {
  switch (result) {
    case network::Timer::Result::OK:
      LOG(INFO, "Result is ok!");
      break;
    default:
      LOG(INFO, "Some error!");
      break;
  }
}

TEST_F(TimerTest, Sync) {
  auto& ctx = network::ContextProvider::getInstance();
  ctx.run();

  network::Timer timer(std::chrono::seconds(5), f);
  timer.wait();

  ctx.stop();
}

TEST_F(TimerTest, Async) {
  auto& ctx = network::ContextProvider::getInstance();
  ctx.run();

  network::Timer timer(std::chrono::seconds(5), f);
  timer.async_wait();
  std::this_thread::sleep_for(std::chrono::seconds(5));

  ctx.stop();
}
