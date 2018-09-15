#include "ircbot/admin_port.hpp"

#include "ircbot/client.hpp"
#include "ircbot/logger.hpp"

#include <unistd.h>

namespace ircbot {

AdminPort::AdminPort(std::shared_ptr<Client> client,
                     const asio::ip::tcp::endpoint& endpoint)
    : network::TcpServer{endpoint}, m_client{client} {
  LOG(INFO, "Started admin port at: ");
}

void AdminPort::addPlugin(const AdminPortProtocol::Request& /* req */) {}

void AdminPort::removePlugin(const AdminPortProtocol::Request& /* req */) {}

void AdminPort::startPlugin(const AdminPortProtocol::Request& /* req */) {}

void AdminPort::stopPlugin(const AdminPortProtocol::Request& /* req */) {}

void AdminPort::restartPlugin(const AdminPortProtocol::Request& /* req */) {}

void AdminPort::reloadPlugin(const AdminPortProtocol::Request& /* req */) {}

void AdminPort::shutdown(const AdminPortProtocol::Request& /* req */) {}

void AdminPort::AdminPortClient::onRead(const std::string& data) {
  using namespace AdminPortProtocol;

  Request req;
  if (not req.ParseFromString(data)) {
    LOG(ERROR, "Could not parse received Request!");
    return;
  }

  switch (req.type()) {
    case Request::ADD_PLUGIN:
      m_admin_port->addPlugin(req);
      break;

    case Request::REMOVE_PLUGIN:
      m_admin_port->removePlugin(req);
      break;

    case Request::START_PLUGIN:
      m_admin_port->startPlugin(req);
      break;

    case Request::STOP_PLUGIN:
      m_admin_port->stopPlugin(req);
      break;

    case Request::RESTART_PLUGIN:
      m_admin_port->restartPlugin(req);
      break;

    case Request::RELOAD_PLUGIN:
      m_admin_port->reloadPlugin(req);
      break;

    case Request::SHUTDOWN:
      m_admin_port->shutdown(req);
      break;

    default:
      LOG(WARNING, "Unsupported action: ", static_cast<int>(req.type()));
      break;
  }
}

std::unique_ptr<network::TcpClient> AdminPort::createClient(
    asio::ip::tcp::socket&& socket) {
  auto client = std::make_unique<AdminPortClient>(std::move(socket));
  client->m_admin_port = shared_from_this();
  return client;
}

}  // namespace ircbot
