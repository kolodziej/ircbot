#ifndef _IRCBOT_NETWORK_CONTEXT_PROVIDER_HPP
#define _IRCBOT_NETWORK_CONTEXT_PROVIDER_HPP

#include <memory>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

class ContextProvider {
 public:
  using Context = asio::io_service;

  ~ContextProvider();

  static ContextProvider& getInstance();

  Context& getContext();
  void run();
  void stopWork();
  void stop();

 private:
  ContextProvider() = default;

  Context m_context;
  std::unique_ptr<Context::work> m_work;
  std::thread m_context_thread;
  static ContextProvider m_instance;
};

}  // namespace network
}  // namespace ircbot

#endif
