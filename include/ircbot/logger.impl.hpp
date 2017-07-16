#include <iomanip>
#include <string>

template <typename... Args>
void Logger::operator()(LogLevel level, Args... args) {
  std::lock_guard<std::mutex> lock(m_mtx);
  m_stream.str(std::string());

  // time
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);

  char t[64];
  std::strftime(t, 64, "%Y-%m-%d %X", std::localtime(&time));
  m_stream << t;

  m_stream << " " << LogLevelDesc(level) << ": ";

  log(args...);

  std::string log_msg = m_stream.str();
  for (auto& log : m_outputs)
    log.log(level, log_msg);
}

template <typename First, typename... Rest>
void Logger::log(First f, Rest... r) {
  m_stream << f;
  log(r...);
}

template <typename... Rest>
void Logger::log(std::string s, Rest... r) {
  m_stream << filterString(s);
  log(r...);
}
