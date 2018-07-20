#include "ircbot/network/buffer.hpp"

namespace ircbot {
namespace network {

Buffer::Buffer() : Buffer{Buffer::default_buffer_size} {}

Buffer::Buffer(size_t size) : m_buffer{size} {}

size_t Buffer::size() const { return m_buffer.size(); }

bool Buffer::full() const { return m_buffer.full(); }

bool Buffer::empty() const { return m_buffer.empty(); }

void Buffer::append(const char* data, size_t size) {
  std::string t{data, size};
  append(t);
}

void Buffer::append(const std::string& data) {
  std::lock_guard<std::mutex> guard{m_buffer_mtx};
  m_buffer.insert(m_buffer.end(), data.begin(), data.end());
}

std::string Buffer::get() { return get(size()); }

std::string Buffer::get(size_t num) {
  std::lock_guard<std::mutex> guard{m_buffer_mtx};
  num = std::min(num, size());

  auto it = m_buffer.begin();
  std::advance(it, num);
  std::string ret{m_buffer.begin(), it};
  m_buffer.erase(m_buffer.begin(), it);

  return ret;
}

}  // namespace ircbot
}  // namespace network
