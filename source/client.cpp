#include "ircbot/client.hpp"

#include <stdexcept>
#include <thread>
#include <chrono>
#include <utility>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include <dlfcn.h>

#include "ircbot/plugin.hpp"
#include "ircbot/so_plugin.hpp"
#include "ircbot/lua_plugin.hpp"
#include "ircbot/helpers.hpp"

Client::Client(asio::io_service& io_service, Config cfg) :
    m_io_service{io_service},
    m_socket{io_service},
    m_cfg{cfg},
    m_admin_port{nullptr},
    m_running{false}
{}

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

  LOG(INFO, "Trying to connect...");
  asio::connect(m_socket, endp, ec);

  if (ec) {
    throw std::runtime_error{"Could not connect to host!"};
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

Client::PluginVectorIter Client::loadPlugin(const std::string& pluginId) {
  auto cfg = m_cfg.tree().get_child(std::string{"plugins."} + pluginId);
  return loadPlugin(pluginId, cfg);
}

Client::PluginVectorIter Client::loadPlugin(const std::string& pluginId,
                                            Config config) {
  std::string path = config.tree().get("path", std::string());
  if (path.empty()) {
    LOG(WARNING, "Some plugin has no path field in configuration! Omitting.");
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
    LOG(INFO, "Loading plugin from shared library: ", path,
        " with ID: '", pluginId, "'.");

    PluginConfig cfg{
      shared_from_this(),
      pluginId,
      config
    };
    auto plugin = loadSoPlugin(path, cfg);

    if (plugin == nullptr) {
      LOG(ERROR, "Could not load plugin from file: ", path);
    } else {
      LOG(INFO, "Plugin ", plugin->getName(), " loaded!");
      return addPlugin(std::move(plugin));
    }
  } else if (ext == ".lua") {
    LOG(INFO, "Loading Lua Plugin from file: ", path, " with ID: '", pluginId, "'.");

    PluginConfig cfg{
      shared_from_this(),
      pluginId,
      config
    };
    auto plugin = loadLuaPlugin(path, cfg);

    if (plugin == nullptr) {
      LOG(ERROR, "Could not load Lua Plugin from file: ", path);
    } else {
      LOG(INFO, "Plugin ", plugin->getName(), " loaded!");
      return addPlugin(std::move(plugin));
    }
  } else if (ext == ".builtin") {
    LOG(WARNING, "Support for builtin plugins is in development! Omitting.");
  } else {
    LOG(WARNING, "Unsupported plugin type: ", ext, ". Omitting!");
  }
}

void Client::startAsyncReceive() {
  using asio::mutable_buffers_1;
  
  auto handler = [this](const boost::system::error_code& ec, size_t bytes) {
    if (ec) {
      stopAsyncReceive();
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

  m_socket.async_receive(
      mutable_buffers_1(m_buffer.data(), m_buffer.size()), handler);
}

void Client::stopAsyncReceive() {
  boost::system::error_code ec;
  m_socket.cancel(ec);
  if (ec != 0) {
    LOG(ERROR, "Error canceling async operation on boost socket: ", ec);
  }
}

void Client::startAdminPort(const std::string& socket_path) {
  if (m_admin_port != nullptr) {
    LOG(ERROR, "Cannot start another admin port!");
    return;
  }

  m_admin_port = std::make_unique<AdminPort>(
    shared_from_this(),
    socket_path
  );

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

void Client::disconnect() {
  m_running = false;
  m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
  m_socket.close();
}

void Client::send(IRCMessage cmd) {
  send(static_cast<std::string>(cmd));
}

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
  LOG(DEBUG, "Trying to send message: ", msg.data());
  m_socket.async_send(const_buf, write_handler);
}

void Client::run() {
  LOG(INFO, "Spawning Client threads...");
  m_running = true;
}

void Client::stop() {
  LOG(INFO, "Stopping Client...");
  stopAsyncReceive();
  disconnect();
  for (auto& plugin : m_plugins) {
    plugin->stop();
  }
  stopAdminPort();
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
  auto pred = [pluginId] (const std::unique_ptr<Plugin>& plugin) {
    return plugin->getId() == pluginId;
  };
  return std::find_if(m_plugins.begin(), m_plugins.end(), pred);
}

Client::PluginVectorIter Client::addPlugin(std::unique_ptr<Plugin>&& plugin) {
  LOG(INFO, "Adding plugin ", plugin->getName());
  return m_plugins.insert(m_plugins.end(), std::move(plugin));
}

void Client::removePlugin(PluginVectorIter it) {
  if (it == m_plugins.end()) {
    throw std::runtime_error{"There is no such plugin!"};
  }
  m_plugins.erase(it);
}

std::vector<std::string> Client::listPlugins() const {
  std::vector<std::string> names;
  for (const auto& plugin : m_plugins) {
    names.push_back(plugin->getName());
  }

  return names;
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
      reinterpret_cast<std::unique_ptr<SoPlugin> (*)(PluginConfig)>
      (getPluginFunc);

  if (func == nullptr) {
    LOG(ERROR, "Could not load plugin from ", fname, ": ", dlerror());
    return nullptr;
  }

  std::unique_ptr<SoPlugin> plugin = func(config);
  m_dl_plugins[plugin->getId()] = pluginLibrary;

  return plugin;
}

std::unique_ptr<LuaPlugin> Client::loadLuaPlugin(const std::string& fname,
                                                 PluginConfig config) {
  return std::make_unique<LuaPlugin>(config);
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
  void * so_lib = m_dl_plugins[pluginId];
  removePlugin(it); 
  if (dlclose(so_lib) != 0) {
    LOG(ERROR, "Could not unload so library!");
  }

  auto plugin = loadPlugin(pluginId);
  (*plugin)->spawn();
}

asio::io_service& Client::getIoService() {
  return m_io_service;
}
