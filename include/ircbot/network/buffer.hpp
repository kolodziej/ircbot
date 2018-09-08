#ifndef _IRCBOT_NETWORK_BUFFER_HPP
#define _IRCBOT_NETWORK_BUFFER_HPP

#include <boost/circular_buffer.hpp>
#include <mutex>

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE 4096
#endif

namespace ircbot {
namespace network {

/** \class Buffer
 *
 * \brief Circular buffer implementation
 *
 * Wrapper for boost::circular_buffer.
 */

class Buffer {
 public:
  /** default size of buffer */
  constexpr static const size_t default_buffer_size = DEFAULT_BUFFER_SIZE;

  /** default constructor */
  Buffer();

  /** Constructor with size argument
   *
   * \param size size of circular buffer
   */
  Buffer(size_t size);

  /** Get size of buffer
   *
   * \return size of buffer
   */
  size_t size() const;

  /** Is buffer full?
   *
   * \return bool indicating that buffer is full
   */
  bool full() const;

  /** Is buffer empty?
   *
   * \return bool indicating that buffer is empty
   */
  bool empty() const;

  /** Append data to buffer
   *
   * \param data pointer to data that will be appended
   * \param size lenght of data that will be appended
   */
  void append(const char* data, size_t size);

  /** Append data to buffer
   *
   * \param data string with data that will be appended
   */
  void append(const std::string& data);

  /** Get value of buffer
   *
   * \return string with contents of buffer
   */
  std::string get();

  /** Get given number of characters from buffer
   *
   * \param num number of characters that will be returned
   * \return min(num, size of buffer) first characters from buffer
   */
  std::string get(size_t num);

 private:
  /** underyling circular buffer */
  boost::circular_buffer<char> m_buffer;

  /** mutex protecting access access to circular buffer */
  std::mutex m_buffer_mtx;
};

}  // namespace network
}  // namespace ircbot

#endif
