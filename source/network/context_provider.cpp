#include "ircbot/network/context_provider.hpp"

namespace ircbot {
  namespace network {

ContextProvider::~ContextProvider() { stop(); }

ContextProvider& ContextProvider::getInstance() { return m_instance; }

ContextProvider::Context& ContextProvider::getContext() { return m_context; }

void ContextProvider::run() {
  auto ctx_run = [this] { m_context.run(); };
  m_work = std::make_unique<Context::work>(m_context);
  m_context_thread = std::move(std::thread{ctx_run});
}

void ContextProvider::stop() {
  if (m_work != nullptr) m_work.reset(nullptr);

  if (m_context_thread.joinable()) m_context_thread.join();
}

ContextProvider ContextProvider::m_instance;

}  // namespace network
}  // namespace ircbot
