#include <iomanip>

template <typename... Args>
void Logger::operator()(LogLevel level, Args... args) {
  std::lock_guard<std::mutex> lock(m_mtx);
  m_stream = std::stringstream();

  // time
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);

  m_stream << std::put_time(std::localtime(&time), "%Y-%m-%d %X") << " ";

  m_stream << LogLevelDesc(level) << ": ";

  log(args...);

  auto log_msg = m_stream.str();
  for (auto& log : m_outputs)
    log.log(level, log_msg);
}

template <typename First, typename... Rest>
void Logger::log(First f, Rest... r) {
  m_stream << f;
  log(r...);
}
