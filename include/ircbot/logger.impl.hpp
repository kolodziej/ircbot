template <typename First, typename... Rest>
void Logger::operator()(std::stringstream& stream, First f, Rest... r) {
  std::lock_guard<std::mutex> lock(m_mtx);
  std::stringstream stream;
  log(stream, f, r);

  auto log_msg = stream.str();
  for (auto& log : m_outputs)
    log.log(log_msg);
}

template <typename First, typename... Rest>
void Logger::log(std::stringstream& stream, First f, Rest... r) {
  stream << f;
  log(stream, r...);
}

void Logger::log(std::stringstream& stream) {
  stream << std::endl;
}
