#include "ircbot/core.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <utility>

#include <boost/filesystem.hpp>

#include <dlfcn.h>

#include "ircbot/helpers.hpp"
#include "ircbot/plugin.hpp"
#include "ircbot/so_plugin.hpp"
#include "ircbot/tcp_plugin_server.hpp"

namespace ircbot {

using network::ContextProvider;

Core::Core(Config cfg)
    : m_context_provider{ContextProvider::getInstance()},
      m_socket{m_context_provider.getContext()},
      m_cfg{cfg},
      m_admin_port{nullptr},
      m_tcp_plugin_server{nullptr},
      m_running{false},
      m_result{RunResult::OK},
      m_start_time{std::chrono::steady_clock::now()} {}

void Core::connect() {
  asio::ip::tcp::resolver resolver{m_context_provider.getContext()};
  boost::system::error_code ec;

  std::string host = m_cfg["server"].as<std::string>();
  uint16_t port = m_cfg["port"].as<uint16_t>();

  if (port == 0u or host.empty()) {
    throw std::runtime_error{"Incorrect host or port!"};
  }

  LOG(INFO, "Trying to resolve ", host, ':', port);
  auto endp = resolver.resolve({host, std::to_string(port)}, ec);

  if (ec) {
    LOG(ERROR, "Could not resolve address; ec = ", ec);
    throw std::runtime_error{"Could not resolve address!"};
  }

  uint32_t trials{};
  uint32_t interval{m_cfg["reconnect-interval"].as<uint32_t>(
      200u)};  // interval in milliseconds
  uint32_t maximum_interval{
      m_cfg["max-reconnect-interval"].as<uint32_t>(15000u)};
  while (true) {
    LOG(INFO, "(", trials, ") Trying to connect...");
    asio::connect(m_socket, endp, ec);
    if (ec != boost::system::errc::success) {
      LOG(ERROR, "Could not connect to host!");
      LOG(INFO, "Reconnecting in ", interval, " milliseconds.");
      auto interval_time = std::chrono::milliseconds(interval);
      std::this_thread::sleep_for(interval_time);
      interval *= 2;
      interval = (interval < maximum_interval) ? interval : maximum_interval;
      ++trials;
    } else {
      LOG(INFO, "Connected successfully!");
      break;
    }
  }

  startAsyncReceive();
}

void Core::initializePlugins() {
  // @TODO: initializePlugins
}

void Core::deinitializePlugins() {
  // @TODO: deinitializePlugins
}

void Core::startAsyncReceive() {
  using asio::mutable_buffers_1;

  auto handler = [this](const boost::system::error_code& ec, size_t bytes) {
    if (ec and ec != asio::error::operation_aborted) {
      LOG(ERROR, "An error occurred during asynchronous receiving: ", ec);
      m_result = RunResult::CONNECTION_ERROR;
      stop();
      return;
    } else if (ec == asio::error::operation_aborted) {
      LOG(INFO, "Asynchronous receiving cancelled!");
      return;
    }

    m_parser.parse(std::string(m_buffer.data(), bytes));
    LOG(INFO, "Parsed commands: ", m_parser.messagesCount());
    while (m_parser.messagesCount() > 0) {
      auto msg = m_parser.getMessage();
      LOG(INFO, "Passing message to plugins: ", msg.toString());
      for (auto& plugin : m_plugins) {
        if (not plugin->preFilter(msg)) {
          DEBUG("Plugin ", plugin->getName(), " pre-filtering not passed",
                " for message: ", msg.toString(true));
          continue;
        }

        if (plugin->filter(msg)) {
          DEBUG("Plugin ", plugin->getName(),
                " filtering passed for message: ", msg.toString(true));
          plugin->receive(msg);
        }
      }
    }

    startAsyncReceive();
  };

  m_socket.async_receive(mutable_buffers_1(m_buffer.data(), m_buffer.size()),
                         handler);
}

void Core::stopAsyncReceive() {
  DEBUG("Cancelling asynchronous operations.");
  boost::system::error_code ec;
  m_socket.cancel(ec);
  if (ec != boost::system::errc::success) {
    LOG(ERROR, "Error canceling async operation on boost socket: ", ec);
  }
}

void Core::startAdminPort() {
  std::string socket_path{m_cfg["admin_port"]["path"].as<std::string>(
      "/var/run/ircbot_admin.sock")};

  LOG(INFO, "Starting admin port on ", socket_path);

  if (m_admin_port != nullptr) {
    LOG(ERROR, "Cannot start another admin port!");
    return;
  }

  m_admin_port = std::make_unique<AdminPort>(shared_from_this(), socket_path);

  m_admin_port->acceptConnections();
}

void Core::stopAdminPort() {
  if (m_admin_port != nullptr) {
    m_admin_port->stop();
    m_admin_port = nullptr;
  } else {
    LOG(INFO, "There is no active admin port!");
  }
}

void Core::startTcpPluginServer() {
  std::string host{
      m_cfg["tcp_plugin_server"]["host"].as<std::string>("localhost")};
  uint16_t port{m_cfg["tcp_plugin_server"]["port"].as<uint16_t>(5454u)};

  LOG(INFO, "Starting tcp plugin server on ", host, ":", port);

  if (m_tcp_plugin_server != nullptr) {
    LOG(ERROR, "TcpPluginServer has already been initialized!");
    return;
  }

  m_tcp_plugin_server =
      std::make_unique<TcpPluginServer>(shared_from_this(), host, port);
  m_tcp_plugin_server->acceptConnections();
}

void Core::stopTcpPluginServer() {
  if (m_tcp_plugin_server == nullptr) {
    LOG(WARNING,
        "TcpPluginServer hasn't been initialized so cannot be stopped!");
    return;
  }

  m_tcp_plugin_server->stop();
}

void Core::disconnect() {
  m_running = false;
  boost::system::error_code ec;

  LOG(INFO, "Shutting down socket");
  m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
  if (ec) {
    LOG(ERROR, "An error occurred during socket shutdown: ", ec);
  }

  LOG(INFO, "Closing socket");
  m_socket.close(ec);
  if (ec) {
    LOG(ERROR, "An error occurred during socket close operation: ", ec);
  }
}

void Core::send(IRCMessage cmd) { send(cmd.toString()); }

void Core::send(std::string msg) {
  auto write_handler = [this](boost::system::error_code ec,
                              size_t transferred) {
    if (not ec) {
      LOG(INFO, "Sent message of size ", transferred);
    } else {
      LOG(ERROR, "Could not send message! Asio returned ", ec);
    }
  };
  auto const_buf = asio::const_buffers_1(msg.data(), msg.size());
  DEBUG("Trying to send message: ", std::string{msg.data(), msg.size()});
  m_socket.async_send(const_buf, write_handler);
}

void Core::run() {
  m_running = true;
  m_stop_promise = std::promise<RunResult>{};

  // connect to server
  connect();

  // initialize plugins
  initializePlugins();

  // start plugins
  // startPlugins();

  startTcpPluginServer();
  startAdminPort();
}

void Core::stop() {
  LOG(INFO, "Stopping Client");

  stopAsyncReceive();
  disconnect();

  // stopPlugins();

  LOG(INFO, "Stopping tcp plugin server (if exists)...");
  stopTcpPluginServer();

  deinitializePlugins();

  LOG(INFO, "Stopping admin port (if exists)...");
  stopAdminPort();

  LOG(INFO, "Client is stopped. Ready to exit.");

  m_stop_promise.set_value(m_result);
}

Core::RunResult Core::waitForStop() {
  std::future<RunResult> f{m_stop_promise.get_future()};
  f.wait();
  return f.get();
}

void Core::signal(int signum) {
  switch (signum) {
    case SIGTERM:
    case SIGINT:
      LOG(INFO, "Received signal: ", signum)
      stop();
      break;
  }
}

bool Core::authenticatePlugin(const std::string& token) {
  const std::string real_token = m_cfg["plugin_token"].as<std::string>();
  if (real_token.empty()) {
    LOG(ERROR, "Token for TCP plugins has not been set or is empty!");
    return false;
  }

  if (real_token == token) {
    return true;
  }

  return false;
}

asio::io_service& Core::getIoService() {
  return m_context_provider.getContext();
}

std::chrono::steady_clock::duration Core::getUptime() const {
  return std::chrono::steady_clock::now() - m_start_time;
}

}  // namespace ircbot
