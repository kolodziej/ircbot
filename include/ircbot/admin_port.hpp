#ifndef _ADMIN_PORT_HPP
#define _ADMIN_PORT_HPP

#include "ircbot/command_parser.hpp"

#include <array>
#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "admin.pb.h"
#include "ircbot/network/tcp_client.hpp"
#include "ircbot/network/tcp_server.hpp"

namespace asio = boost::asio;

namespace ircbot {

class Client;

/** \class AdminPort
 *
 * \brief Provides access to administration utilities
 *
 * AdminPort object initializes unix socket which provides external
 * users/applications interface for ircbot management.
 */

class AdminPort : public network::TcpServer,
                  public std::enable_shared_from_this<AdminPort> {
 public:
  /** Default constructor
   *
   * \param client shared pointer to Client instance which will be managed with
   * this AdminPort
   * \param endpoint endpoint on which AdminPort should listen
   */
  AdminPort(std::shared_ptr<Client> client,
            const asio::ip::tcp::endpoint& endpoint);

  /** Destructor
   *
   * Removes created socket
   */
  virtual ~AdminPort() {}

  /** process ADD_PLUGIN request
   *
   *  \param req request received by AdminPort
   */
  void addPlugin(const AdminPortProtocol::Request& req);

  /** process REMOVE_PLUGIN request
   *
   *  \param req request received by AdminPort
   */
  void removePlugin(const AdminPortProtocol::Request& req);

  /** process START_PLUGIN request
   *
   *  \param req request received by AdminPort
   */
  void startPlugin(const AdminPortProtocol::Request& req);

  /** process STOP_PLUGIN request
   *
   *  \param req request received by AdminPort
   */
  void stopPlugin(const AdminPortProtocol::Request& req);

  /** process RESTART_PLUGIN request
   *
   *  \param req request received by AdminPort
   */
  void restartPlugin(const AdminPortProtocol::Request& req);

  /** process RELOAD_PLUGIN request
   *
   *  \param req request received by AdminPort
   */
  void reloadPlugin(const AdminPortProtocol::Request& req);

  /** process SHUTDOWN request
   *
   *  \param req request received by AdminPort
   */
  void shutdown(const AdminPortProtocol::Request& req);

 private:
  /** \class AdminPortClient
   *
   * \brief Client connecting to AdminPort
   *
   * Representation of client connecting to AdminPort. Provides socket and
   * buffer for received messages.
   */
  struct AdminPortClient : public network::TcpClient {
    /** default constructor */
    AdminPortClient(asio::ip::tcp::socket&& socket)
        : network::TcpClient{std::move(socket)} {}

    /** pointer for parent admin port */
    std::shared_ptr<AdminPort> m_admin_port;

    /** callback for incoming data */
    virtual void onRead(const std::string& data);
  };

  /** Pointer to client which is managed with this AdminPort */
  std::shared_ptr<Client> m_client;

  /** creates client of AdminPortClient type */
  virtual std::unique_ptr<network::TcpClient> createClient(
      asio::ip::tcp::socket&& socket);
};

}  // namespace ircbot

#endif
