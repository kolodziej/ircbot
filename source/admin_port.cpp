#include "ircbot/admin_port.hpp"

#include "ircbot/client.hpp"
#include "ircbot/config.hpp"
#include "ircbot/logger.hpp"

#include <unistd.h>

namespace ircbot {

AdminPort::AdminPort(std::shared_ptr<Client> client,
                     const asio::ip::tcp::endpoint& endpoint)
    : network::TcpServer{endpoint}, m_client{client} {
  LOG(INFO, "Started admin port at: ");
}

void AdminPort::addPlugin(const AdminPortProtocol::Request& req) {
  if (not req.has_add_plugin_params()) {
    // TODO: error
  }

  const std::string id{req.add_plugin_params().id()};
  const std::string path{req.add_plugin_params().path()};

  if (req.add_plugin_params().config_size() > 0)
    LOG(WARNING, "Plugin configuration from AdminPort is not supported yet!");

  Config cfg;
  cfg.tree().put(id + std::string{".path"}, path);

  auto plugin_iter = m_client->loadPlugin(id, cfg);
  LOG(WARNING,
      "Error support is not fully implemented yet! Make sure that plugin has "
      "been loaded!");
  // TODO: finish: add to client and check if plugin could be added
}

void AdminPort::removePlugin(const AdminPortProtocol::Request& req) {
  if (not req.has_plugin_params()) {
    // TODO: error
  }

  for (int i = 0; i < req.plugin_params().plugins_size(); ++i) {
    const std::string id{req.plugin_params().plugins(i)};
    try {
      m_client->removePlugin(id);
      LOG(INFO, "Plugin ", id, " has been removed!");
    } catch (const std::runtime_error& err) {
      LOG(ERROR, "Could not remove plugin ", id, ": ", err.what());
      // TODO: Response
    }
  }
}

void AdminPort::startPlugin(const AdminPortProtocol::Request& req) {
  if (not req.has_plugin_params()) {
    // TODO: error
  }

  for (int i = 0; i < req.plugin_params().plugins_size(); ++i) {
    const std::string id{req.plugin_params().plugins(i)};
    m_client->startPlugin(id);
  }
}

void AdminPort::stopPlugin(const AdminPortProtocol::Request& req) {
  if (not req.has_plugin_params()) {
    // TODO: error
  }

  for (int i = 0; i < req.plugin_params().plugins_size(); ++i) {
    const std::string id{req.plugin_params().plugins(i)};
    m_client->stopPlugin(id);
  }
}

void AdminPort::restartPlugin(const AdminPortProtocol::Request& req) {
  if (not req.has_plugin_params()) {
    // TODO: error
  }

  for (int i = 0; i < req.plugin_params().plugins_size(); ++i) {
    const std::string id{req.plugin_params().plugins(i)};
    m_client->restartPlugin(id);
  }
}

void AdminPort::reloadPlugin(const AdminPortProtocol::Request& req) {
  if (not req.has_plugin_params()) {
    // TODO: error
  }

  for (int i = 0; i < req.plugin_params().plugins_size(); ++i) {
    const std::string id{req.plugin_params().plugins(i)};
    m_client->reloadPlugin(id);
  }
}

void AdminPort::shutdown(const AdminPortProtocol::Request& req) {
  m_client->stop();
}

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

void AdminPort::AdminPortClient::respond(
    AdminPortProtocol::Response::ResponseType type, bool end, int32_t code,
    const std::string& msg) {
  using AdminPortProtocol::Response;
  Response resp;
  resp.set_type(type);
  resp.set_end(end);
  resp.set_code(code);
  if (not msg.empty()) resp.set_msg(msg);

  // TODO: send message
}

std::unique_ptr<network::TcpClient> AdminPort::createClient(
    asio::ip::tcp::socket&& socket) {
  auto client = std::make_unique<AdminPortClient>(std::move(socket));
  client->m_admin_port = shared_from_this();
  return client;
}

}  // namespace ircbot
