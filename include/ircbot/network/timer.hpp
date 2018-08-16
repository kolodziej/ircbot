#ifndef _IRCBOT_NETWORK_TIMER_HPP
#define _IRCBOT_NETWORK_TIMER_HPP

#include <chrono>
#include <functional>

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

class Timer {
 public:
  enum class Result : uint8_t { OK = 0, CANCEL, ERROR = 0xff };

  Timer() = default;
  Timer(std::chrono::steady_clock::duration duration,
        std::function<void(Result)> f);
  Timer(std::chrono::steady_clock::time_point time_point,
        std::function<void(Result)>);

  Timer(Timer&&) = default;
  Timer& operator=(Timer&&) = default;
  Timer(const Timer&) = delete;
  Timer operator=(const Timer&) = delete;

  void wait();
  void async_wait();
  void cancel();
  void setFunction(std::function<void(Result)> f);

 private:
  asio::basic_waitable_timer<std::chrono::steady_clock> m_timer;
  std::function<void(Result)> m_f;

  void waitHandler(const boost::system::error_code& ec);
};

}  // namespace network
}  // namespace ircbot

#endif
