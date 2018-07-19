#ifndef _IRCBOT_NETWORK_BUFFER_HPP
#define _IRCBOT_NETWORK_BUFFER_HPP

#include <boost/circular_buffer.hpp>
#include <mutex>

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE 4096
#endif

namespace ircbot {
namespace network {

class Buffer {
 public:
  constexpr static const size_t default_buffer_size = DEFAULT_BUFFER_SIZE;
  Buffer() = default;

  size_t size() const;

  void append(const char *data, size_t size);
  void append(const std::string& data);

  std::string get(size_t num);

 private:
  boost::circular_buffer<char> m_buffer;
  std::mutex m_buffer_mtx;
};

} // namespace network
} // namespace ircbot

#endif
