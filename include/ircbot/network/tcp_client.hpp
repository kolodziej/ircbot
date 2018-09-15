#ifndef _IRCBOT_NETWORK_TCP_CLIENT_HPP
#define _IRCBOT_NETWORK_TCP_CLIENT_HPP

#include "ircbot/logger.hpp"
#include "ircbot/network/basic_client.hpp"
#include "ircbot/network/buffer.hpp"
#include "ircbot/network/context_provider.hpp"
#include "ircbot/network/endpoint.hpp"

#include <array>
#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {
namespace network {

/** \class TcpClient
 *
 * \brief Tcp client implementation
 *
 * TcpClient can be use as standalone client or as a componenent used by
 * TcpServer to represent connected clients.
 */

class TcpClient : public Endpoint<asio::ip::tcp>, public BasicClient {
 public:
  /** default buffer size */
  constexpr static const size_t default_buffer_size = 4096;

  /** constructor
   *
   * used by standalone client
   *
   * \param endpoint endpoint to which socket will be connected
   */
  TcpClient(const asio::ip::tcp::endpoint& endpoint);

  /** constructor
   *
   * used by server
   *
   * \param socket socket to which client is connected
   */
  TcpClient(asio::ip::tcp::socket&& socket);

  /** Move constructor */
  TcpClient(TcpClient&&) = default;

  /** virtual destructor */
  virtual ~TcpClient();

  virtual void connect();
  virtual void send(const std::string& data);
  virtual void receive();

  /** Disconnect from server */
  virtual void disconnect();

 protected:
  /** Handler called when data is sent
   *
   * \param bytes_transferred number of bytes written to m_socket
   */
  virtual void onWrite(const size_t /* bytes_transferred */) {}

  /** Handler called when data is received
   *
   * \param data that was received
   */
  virtual void onRead(const std::string& /* data */) {}

 private:
  /** socket for this client */
  asio::ip::tcp::socket m_socket;

  /** buffer for received data for this client */
  std::array<char, default_buffer_size> m_receive_buffer;

  /** write handler used by boost::asio asynchronous functions */
  void writeHandler(const boost::system::error_code& ec,
                    std::size_t bytes_transferred);

  /** read handler used by boost::asio asynchronous functions */
  void readHandler(const boost::system::error_code& ec,
                   std::size_t bytes_transferred);
};

}  // namespace network
}  // namespace ircbot

#endif
