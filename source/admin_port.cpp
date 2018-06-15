#include "ircbot/admin_port.hpp"

#include "ircbot/client.hpp"
#include "ircbot/logger.hpp"

#include <unistd.h>

namespace ircbot {

AdminPort::AdminPort(std::shared_ptr<Client> client,
                     const std::string& socket_path)
    : m_client{client},
      m_endpoint{socket_path},
      m_acceptor{client->getIoService(), m_endpoint},
      m_socket{client->getIoService()},
      m_command_parser{ParserConfig{'\0', true}} {
  LOG(INFO, "Started admin port at: ", socket_path);
}

AdminPort::~AdminPort() { ::unlink(m_endpoint.path().data()); }

void AdminPort::acceptConnections() {
  using boost::system::error_code;

  auto handler = [this](const error_code& ec) {
    if (ec) {
      if (ec == asio::error::operation_aborted) {
        LOG(INFO, "Canceling asynchronous connection accepting.");
        return;
      }
      LOG(ERROR, "Could not accept connection to admin port! Error: ", ec);
    } else {
      addClient();
    }

    acceptConnections();
  };

  m_acceptor.async_accept(m_socket, handler);
}

void AdminPort::stop() {
  m_acceptor.cancel();
  m_acceptor.close();
}

void AdminPort::addClient() {
  AdminPortClient client{this, std::move(m_socket), {}};
  m_clients.push_back(std::move(client));
  m_clients.back().startReceiving();
}

void AdminPort::processCommand(const std::string& command) {
  LOG(INFO, "Processing command in AdminPort: ", command);

  CommandParser::Command cmd = m_command_parser.parse(command);
  LOG(INFO, "Command: ", cmd.command);

  if (cmd.command == "restartPlugin") {
    if (cmd.arguments.size() < 1) {
      LOG(ERROR, "You have to give at least one plugin id!");
    }
    for (const auto& plugin : cmd.arguments) {
      m_client->restartPlugin(plugin);
    }
  } else if (cmd.command == "reloadPlugin") {
    if (cmd.arguments.size() < 1) {
      LOG(ERROR, "You have to give at least one plugin id!");
    }
    for (const auto& plugin : cmd.arguments) {
      m_client->reloadPlugin(plugin);
    }
  } else if (cmd.command == "shutdown") {
    m_client->stop();
  }
}

void AdminPort::AdminPortClient::startReceiving() {
  using boost::system::error_code;

  auto handler = [this](const error_code& ec, std::size_t bytes) {
    if (ec) {
      if (ec == boost::asio::error::operation_aborted) {
        LOG(INFO, "Canceling asynchronous receiving.");
        return;
      }
      LOG(ERROR, "Error occurred: ", ec);
      return;
    }

    std::string msg{buffer.data(), bytes};
    adminPort->processCommand(msg);
    startReceiving();
  };

  using asio::mutable_buffers_1;
  socket.async_receive(mutable_buffers_1(buffer.data(), buffer.size()),
                       handler);
}

}  // namespace ircbot
