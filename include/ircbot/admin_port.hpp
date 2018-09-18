#ifndef _ADMIN_PORT_HPP
#define _ADMIN_PORT_HPP

#include "ircbot/command_parser.hpp"

#include <array>
#include <memory>
#include <string>

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ircbot {

class Core;

/** \class AdminPort
 *
 * \brief Provides access to administration utilities
 *
 * AdminPort object initializes unix socket which provides external
 * users/applications interface for ircbot management.
 */

class AdminPort {
 public:
  /** Default constructor
   *
   * \param client shared pointer to Client instance which will be managed with
   * this AdminPort
   * \param socket_path path to unix socket file which will be used to manage
   * client instance
   */
  AdminPort(std::shared_ptr<Core> client, const std::string& socket_path);
  /** Destructor
   *
   * Removes created socket
   */
  ~AdminPort();

  /** Start accepting connections to unix socket */
  void acceptConnections();

  /** Stops listening for connections */
  void stop();

  /** Adds AdminPortClient instance
   *
   * m_socket represents remote socket for client connected to admin port
   */
  void addClient();

  /** Process given command
   *
   * \param command command received from administration socket
   */
  void processCommand(const std::string& command);

 private:
  /** \class AdminPortClient
   *
   * \brief Client connecting to AdminPort
   *
   * Representation of client connecting to AdminPort. Provides socket and
   * buffer for received messages.
   */
  struct AdminPortClient {
    AdminPort* adminPort;
    asio::local::stream_protocol::socket socket;
    std::array<char, 8192> buffer;

    /** Start asynchronous receiving data from administration port */
    void startReceiving();
  };

  /** Pointer to client which is managed with this AdminPort */
  std::shared_ptr<Core> m_client;

  /** Endpoint - unix socket file */
  asio::local::stream_protocol::endpoint m_endpoint;

  /** Acceptor - listening unix socket */
  asio::local::stream_protocol::acceptor m_acceptor;

  /** Temporary socket for incoming connections */
  asio::local::stream_protocol::socket m_socket;

  CommandParser m_command_parser;

  /** Connected clients */
  std::vector<AdminPortClient> m_clients;
};

}  // namespace ircbot

#endif
