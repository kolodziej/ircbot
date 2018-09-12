#include "ircbot/client.hpp"

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

Client::Client(Config cfg)
    : m_context_provider{ContextProvider::getInstance()},
      m_io_service{m_context_provider.getContext()},
      m_socket{m_io_service},
      m_cfg{cfg},
      m_admin_port{nullptr},
      m_tcp_plugin_server{nullptr},
      m_running{false},
      m_result{RunResult::OK},
      m_start_time{std::chrono::steady_clock::now()} {}

void Client::connect() {
  asio::ip::tcp::resolver resolver{m_io_service};
  boost::system::error_code ec;

  std::string host = m_cfg.tree().get("server", std::string{});
  uint16_t port = m_cfg.tree().get("port", 0u);

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
  uint32_t interval{m_cfg.tree().get("reconnect-interval",
                                     200u)};  // interval in milliseconds
  uint32_t maximum_interval{m_cfg.tree().get("max-reconnect-interval", 15000u)};
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

void Client::initializePlugins() {
  for (auto p : m_cfg.tree().get_child("plugins")) {
    const auto& pluginId = p.first;
    const auto& pluginConfig = p.second;

    loadPlugin(pluginId, pluginConfig);
  }
}

void Client::deinitializePlugins() {
  LOG(INFO, "Deinitializing all plugins");
  m_plugins.clear();
}

Client::PluginVectorIter Client::loadPlugin(const std::string& pluginId) {
  auto cfg = m_cfg.tree().get_child(std::string{"plugins."} + pluginId);
  return loadPlugin(pluginId, cfg);
}

Client::PluginVectorIter Client::loadPlugin(const std::string& pluginId,
                                            Config config) {
  std::string path = config.tree().get("path", std::string());
  if (path.empty()) {
    LOG(WARNING, "Plugin (id): ", pluginId,
        " has no path field in configuration! Omitting.");
    return m_plugins.end();
  }

  LOG(INFO, "Processing plugin: ", path);

  namespace fs = boost::filesystem;
  std::string ext = fs::path(path).extension().string();
  if (ext.empty()) {
    LOG(WARNING, "No extension in plugin's path: ", path, ". Omitting.");
    return m_plugins.end();
  }

  if (ext == ".so") {
    LOG(INFO, "Loading plugin from shared library: ", path, " with ID: '",
        pluginId, "'.");

    PluginConfig cfg{shared_from_this(), pluginId, config};
    auto plugin = loadSoPlugin(path, cfg);

    if (plugin == nullptr) {
      LOG(ERROR, "Could not load plugin from file: ", path);
    } else {
      LOG(INFO, "Plugin ", plugin->getName(), " loaded!");
      return addPlugin(std::move(plugin));
    }
  } else if (ext == ".builtin") {
    LOG(WARNING, "Support for builtin plugins is in development! Omitting.");
  } else {
    LOG(WARNING, "Unsupported plugin type: ", ext, ". Omitting!");
  }

  return m_plugins.end();
}

void Client::startAsyncReceive() {
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

void Client::stopAsyncReceive() {
  DEBUG("Cancelling asynchronous operations.");
  boost::system::error_code ec;
  m_socket.cancel(ec);
  if (ec != boost::system::errc::success) {
    LOG(ERROR, "Error canceling async operation on boost socket: ", ec);
  }
}

void Client::startAdminPort() {
  std::string socket_path{
      m_cfg.tree().get("admin_port.path", "/var/run/ircbot_admin.sock")};

  LOG(INFO, "Starting admin port on ", socket_path);

  if (m_admin_port != nullptr) {
    LOG(ERROR, "Cannot start another admin port!");
    return;
  }

  m_admin_port = std::make_unique<AdminPort>(shared_from_this(), socket_path);

  m_admin_port->acceptConnections();
}

void Client::stopAdminPort() {
  if (m_admin_port != nullptr) {
    m_admin_port->stop();
    m_admin_port = nullptr;
  } else {
    LOG(INFO, "There is no active admin port!");
  }
}

void Client::startTcpPluginServer() {
  std::string host{m_cfg.tree().get("tcp_plugin_server.host", "localhost")};
  uint16_t port{
      m_cfg.tree().get("tcp_plugin_server.port", static_cast<uint16_t>(5454))};

  LOG(INFO, "Starting tcp plugin server on ", host, ":", port);

  if (m_tcp_plugin_server != nullptr) {
    LOG(ERROR, "TcpPluginServer has already been initialized!");
    return;
  }

  m_tcp_plugin_server =
      std::make_unique<TcpPluginServer>(shared_from_this(), host, port);
  m_tcp_plugin_server->acceptConnections();
}

void Client::stopTcpPluginServer() {
  if (m_tcp_plugin_server == nullptr) {
    LOG(WARNING,
        "TcpPluginServer hasn't been initialized so cannot be stopped!");
    return;
  }

  m_tcp_plugin_server->stop();
}

void Client::disconnect() {
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

void Client::send(IRCMessage cmd) { send(cmd.toString()); }

void Client::send(std::string msg) {
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

void Client::run() {
  m_running = true;
  m_stop_promise = std::promise<RunResult>{};

  // connect to server
  connect();

  // initialize plugins
  initializePlugins();

  // start plugins
  startPlugins();

  startTcpPluginServer();
  startAdminPort();
}

void Client::stop() {
  LOG(INFO, "Stopping Client");

  stopAsyncReceive();
  disconnect();

  stopPlugins();

  LOG(INFO, "Stopping tcp plugin server (if exists)...");
  stopTcpPluginServer();

  deinitializePlugins();

  LOG(INFO, "Stopping admin port (if exists)...");
  stopAdminPort();

  LOG(INFO, "Client is stopped. Ready to exit.");

  m_stop_promise.set_value(m_result);
}

Client::RunResult Client::waitForStop() {
  std::future<RunResult> f{m_stop_promise.get_future()};
  f.wait();
  return f.get();
}

void Client::signal(int signum) {
  switch (signum) {
    case SIGTERM:
    case SIGINT:
      LOG(INFO, "Received signal: ", signum)
      stop();
      break;
  }
}

Client::PluginVectorIter Client::findPlugin(const std::string& pluginId) {
  auto pred = [pluginId](const std::unique_ptr<Plugin>& plugin) {
    return plugin->getId() == pluginId;
  };
  return std::find_if(m_plugins.begin(), m_plugins.end(), pred);
}

Client::PluginVectorIter Client::addPlugin(std::unique_ptr<Plugin>&& plugin) {
  LOG(INFO, "Adding plugin ", plugin->getName());
  return m_plugins.insert(m_plugins.end(), std::move(plugin));
}

void Client::removePlugin(PluginVectorIter it) {
  LOG(INFO, "Removing plugin ", (*it)->getId());
  if (it == m_plugins.end()) {
    throw std::runtime_error{"There is no such plugin!"};
  }
  m_plugins.erase(it);
}

void Client::removePlugin(const std::string& pluginId) {
  auto plugin = findPlugin(pluginId);
  if (plugin != m_plugins.end()) {
    removePlugin(plugin);
  } else {
    LOG(ERROR, "Could not found plugin with id ", pluginId);
  }
}

std::vector<std::string> Client::listPlugins() const {
  std::vector<std::string> names;
  for (const auto& plugin : m_plugins) {
    names.push_back(plugin->getName());
  }

  return names;
}

bool Client::authenticatePlugin(const std::string& token) {
  const std::string real_token =
      m_cfg.tree().get("plugin_token", std::string{});
  if (real_token.empty()) {
    LOG(ERROR, "Token for TCP plugins has not been set or is empty!");
    return false;
  }

  if (real_token == token) {
    return true;
  }

  return false;
}

std::unique_ptr<SoPlugin> Client::loadSoPlugin(const std::string& fname,
                                               PluginConfig config) {
  void* pluginLibrary = dlopen(fname.data(), RTLD_NOW);
  if (pluginLibrary == nullptr) {
    LOG(ERROR, "Could not load file ", fname, ": ", dlerror());
    return nullptr;
  }

  void* getPluginFunc = dlsym(pluginLibrary, "getPlugin");
  std::function<std::unique_ptr<SoPlugin>(PluginConfig)> func =
      reinterpret_cast<std::unique_ptr<SoPlugin> (*)(PluginConfig)>(
          getPluginFunc);

  if (func == nullptr) {
    LOG(ERROR, "Could not load plugin from ", fname, ": ", dlerror());
    return nullptr;
  }

  std::unique_ptr<SoPlugin> plugin = func(config);
  m_dl_plugins[plugin->getId()] = pluginLibrary;

  return plugin;
}

void Client::startPlugins() {
  for (auto& plugin : m_plugins) {
    LOG(INFO, "Starting plugin ", plugin->getName());
    plugin->spawn();
  }
}

void Client::stopPlugins() {
  for (auto& plugin : m_plugins) {
    LOG(INFO, "Stopping plugin ", plugin->getName());
    plugin->stop();
  }
}

void Client::restartPlugin(const std::string& pluginId) {
  auto it = findPlugin(pluginId);
  if (it == m_plugins.end()) {
    LOG(ERROR, "There is no plugin with ID: '", pluginId, "'.");
    return;
  }

  (*it)->stop();
  (*it)->spawn();
}

void Client::reloadPlugin(const std::string& pluginId) {
  auto it = findPlugin(pluginId);
  if (it == m_plugins.end()) {
    LOG(ERROR, "There is no plugin with ID: '", pluginId, "'.");
    return;
  }

  LOG(INFO, "Reloading plugin ", pluginId);
  (*it)->stop();
  void* so_lib = m_dl_plugins[pluginId];
  removePlugin(it);
  if (dlclose(so_lib) != 0) {
    LOG(ERROR, "Could not unload so library!");
  }

  auto plugin = loadPlugin(pluginId);
  (*plugin)->spawn();
}

asio::io_service& Client::getIoService() { return m_io_service; }

std::chrono::steady_clock::duration Client::getUptime() const {
  return std::chrono::steady_clock::now() - m_start_time;
}

}  // namespace ircbot
