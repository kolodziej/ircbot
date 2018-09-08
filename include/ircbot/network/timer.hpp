#ifndef _IRCBOT_NETWORK_TIMER_HPP
#define _IRCBOT_NETWORK_TIMER_HPP

#include <chrono>
#include <functional>

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

/** \class Timer
 *
 * \brief Simple timer implementation
 *
 * Timer let user to call handler on given timepoint. User can wait for timeout
 * synchronously or asynchronously. This implementation uses timers from
 * boost::asio library.
 */

class Timer {
 public:
  /** Possible results of timer wait functions */
  enum class Result : uint8_t {
    OK = 0,      /**< timeout expired */
    CANCEL,      /**< waiting has been cancelled */
    ERROR = 0xff /**< other error */
  };

  /** default constructor */
  Timer() = default;

  /** constructor
   *
   * \param duration how long timer should wait before handler call
   * \param f function called when timer will expire
   */
  Timer(std::chrono::steady_clock::duration duration,
        std::function<void(Result)> f);

  /** constructor
   *
   * \param point in time when timer will expire
   * \param f function called when timer will expire
   */
  Timer(std::chrono::steady_clock::time_point time_point,
        std::function<void(Result)> f);

  /** default move constructor */
  Timer(Timer&&) = default;

  /** default move assignment operator */
  Timer& operator=(Timer&&) = default;

  /** removed copy constructor */
  Timer(const Timer&) = delete;

  /** removed copy assignment operator */
  Timer operator=(const Timer&) = delete;

  /** synchronously wait for timer expiration */
  void wait();

  /** asynchronously wait for timer expiration */
  void async_wait();

  /** cancel waiting for asynchronous expiration */
  void cancel();

  /** set callback function
   *
   * \param f function that will be called when timer will expire
   */
  void setFunction(std::function<void(Result)> f);

 private:
  /** timer from boost::asio */
  asio::basic_waitable_timer<std::chrono::steady_clock> m_timer;

  /** function that will be called when timer will expire */
  std::function<void(Result)> m_f;

  /** handler for asynchronous function from boost::asio
   *
   * called by asio::basic_waitable_timer<>
   */
  void waitHandler(const boost::system::error_code& ec);
};

}  // namespace network
}  // namespace ircbot

#endif
