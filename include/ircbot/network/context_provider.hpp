#ifndef _IRCBOT_NETWORK_CONTEXT_PROVIDER_HPP
#define _IRCBOT_NETWORK_CONTEXT_PROVIDER_HPP

#include <memory>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

/** \class ContextProvider
 *
 * \brief Provides boost::asio::io_context instances.
 *
 * Singleton object providing access to boost::asio::io_context object.
 */

class ContextProvider {
 public:
  /** alias for boost::asio::io_context/boost::asio::io_service object */
  using Context = asio::io_service;

  /** destructor */
  ~ContextProvider();

  /** Get instance of ContextProvider
   *
   * \return reference to only instance of ContextProvider
   */
  static ContextProvider& getInstance();

  /** Get instance of Context
   *
   * \return reference to instance of Context
   */
  Context& getContext();

  /** run context
   *
   * Run context and idle work
   */
  void run();

  /** stop idle work */
  void stopWork();

  /** stop context */
  void stop();

 private:
  /** default constructor */
  ContextProvider() = default;

  /** Context instance */
  Context m_context;

  /** unique pointer to idle work */
  std::unique_ptr<Context::work> m_work;

  /** thread that is used to run Context */
  std::thread m_context_thread;

  /** instance of ContextProvider */
  static ContextProvider m_instance;
};

}  // namespace network
}  // namespace ircbot

#endif
