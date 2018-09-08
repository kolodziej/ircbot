#include "ircbot/network/timer.hpp"

#include "ircbot/logger.hpp"
#include "ircbot/network/context_provider.hpp"

namespace ircbot {
namespace network {

Timer::Timer(std::chrono::steady_clock::duration duration,
             std::function<void(Result)> f)
    : m_timer{ContextProvider::getInstance().getContext(), duration}, m_f{f} {}

Timer::Timer(std::chrono::steady_clock::time_point time_point,
             std::function<void(Result)> f)
    : m_timer{ContextProvider::getInstance().getContext(), time_point},
      m_f{f} {}

void Timer::wait() {
  DEBUG("Waiting...");
  boost::system::error_code ec;
  m_timer.wait(ec);
  waitHandler(ec);
}

void Timer::async_wait() {
  DEBUG("Asynchronous waiting...");
  auto handler = [this](const boost::system::error_code& ec) {
    waitHandler(ec);
  };
  m_timer.async_wait(handler);
}

void Timer::cancel() { m_timer.cancel(); }

void Timer::setFunction(std::function<void(Result)> f) { m_f = f; }

void Timer::waitHandler(const boost::system::error_code& ec) {
  LOG(INFO, "Timer error code: ", ec);
  if (ec == boost::system::errc::success) {
    m_f(Result::OK);
  } else if (ec == asio::error::operation_aborted) {
    m_f(Result::CANCEL);
  } else {
    m_f(Result::ERROR);
  }
}

}  // namespace network
}  // namespace ircbot
