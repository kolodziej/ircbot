#ifndef _ADMIN_PORT_HPP
#define _ADMIN_PORT_HPP

#include "ircbot/command_parser.hpp"

#include <memory>
#include <string>
#include <array>

#include <boost/asio.hpp>

namespace asio = boost::asio;

class Client;

class AdminPort {
 public:
  AdminPort(std::shared_ptr<Client> client, const std::string& socket_path);
  ~AdminPort();

  void acceptConnections();
  void stop();

  void addClient();
  void processCommand(const std::string& command);

 private:
  struct AdminPortClient {
    AdminPort* adminPort;
    asio::local::stream_protocol::socket socket;
    std::array<char, 8192> buffer;

    void startReceiving();
  };

  void restartPlugin(const std::string& pluginId);

  std::shared_ptr<Client> m_client;
  asio::local::stream_protocol::endpoint m_endpoint;
  asio::local::stream_protocol::acceptor m_acceptor;
  asio::local::stream_protocol::socket m_socket;

  CommandParser m_command_parser;
  std::vector<AdminPortClient> m_clients;
};

#endif
