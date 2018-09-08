#include "ircbot/network/context_provider.hpp"

#include "ircbot/logger.hpp"

namespace ircbot {
namespace network {

ContextProvider::~ContextProvider() { stop(); }

ContextProvider& ContextProvider::getInstance() { return m_instance; }

ContextProvider::Context& ContextProvider::getContext() { return m_context; }

void ContextProvider::run() {
  DEBUG("Running io_context thread and work");
  m_context.reset();
  auto ctx_run = [this] { m_context.run(); };
  m_work = std::make_unique<Context::work>(m_context);
  m_context_thread = std::move(std::thread{ctx_run});
}

void ContextProvider::stopWork() {
  DEBUG("Stopping work");
  m_work.reset(nullptr);
}

void ContextProvider::stop() {
  stopWork();
  if (m_context_thread.joinable()) {
    DEBUG("Joining context thread");
    m_context.stop();
    m_context_thread.join();
    DEBUG("Context thread terminated!");
  }
}

ContextProvider ContextProvider::m_instance;

}  // namespace network
}  // namespace ircbot
