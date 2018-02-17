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
#include "ircbot/helpers.hpp"

Client::Client(asio::io_service& io_service, Config cfg) :
    m_io_service{io_service},
    m_socket{io_service},
    m_cfg{cfg},
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
    std::string path = pluginConfig.get("path", std::string());
    if (path.empty()) {
      LOG(WARNING, "Some plugin has no path field in configuration! Omitting.");
      continue;
    }

    LOG(INFO, "Processing plugin: ", path);

    namespace fs = boost::filesystem;
    std::string ext = fs::path(path).extension().string();
    if (ext.empty()) {
      LOG(WARNING, "No extension in plugin's path: ", path, ". Omitting.");
      continue;
    }

    if (ext == ".so") {
      LOG(INFO, "Loading plugin from shared library: ", path);
      PluginConfig cfg{
        shared_from_this(),
        pluginId,
        pluginConfig
      };
      auto plugin = loadSoPlugin(path, cfg);

      if (plugin == nullptr) {
        LOG(ERROR, "Could not load plugin from file: ", path);
      } else {
        LOG(INFO, "Plugin ", plugin->getName(), " loaded!");
        addPlugin(std::move(plugin));
      }
    } else if (ext == ".builtin") {
      LOG(WARNING, "Support for builtin plugins is in development! Omitting.");
    } else {
      LOG(WARNING, "Unsupported plugin type: ", ext, ". Omitting!");
    }
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
    LOG(INFO, "Parsed commands: ", m_parser.commandsCount());
    while (m_parser.commandsCount() > 0) {
      auto cmd = m_parser.getCommand();
      LOG(INFO, "Passing command to plugins: ", cmd.toString());
      for (auto& plugin : m_plugins) {
        if (plugin->filter(cmd)) {
          DEBUG("Plugin ", plugin->getName(),
                " filtering passed for command: ", cmd.toString(true));
          plugin->receive(cmd);
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

void Client::disconnect() {
  m_running = false;
  m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
  m_socket.close();
}

void Client::send(IRCCommand cmd) {
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

void Client::signal(int signum) {
  switch (signum) {
    case SIGTERM:
    case SIGINT:
      LOG(INFO, "Received signal: ", signum)
      stopAsyncReceive();
      disconnect();
      for (auto& plugin : m_plugins) {
        plugin->stop();
      }
      break;
  }
}

void Client::addPlugin(std::unique_ptr<Plugin>&& plugin) {
  LOG(INFO, "Adding plugin ", plugin->getName());
  m_plugins.push_back(std::move(plugin));
}

void Client::removePlugin(const std::string& name) {
  auto pred = [&name](const auto& plugin) { return plugin->getName() == name; };
  auto remove_it = std::find_if(m_plugins.begin(), m_plugins.end(), pred);

  if (remove_it == m_plugins.end()) {
    throw std::runtime_error{"Could not find such plugin!"};
  }

  m_plugins.erase(remove_it);
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
  plugin->setDlLibrary(pluginLibrary);

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

void Client::restartPlugin(const std::string& name) {
  LOG(INFO, "Trying to restart plugin: ", name);
}

void Client::reloadPlugin(const std::string& name) {
  LOG(INFO, "Trying to reload plugin: ", name);
  auto pred = [name] (const std::unique_ptr<Plugin>& plugin) {
    return (plugin->getName() == name);
  };
  auto it = std::find_if(m_plugins.begin(), m_plugins.end(), pred);
  if (it == m_plugins.end()) {
    LOG(ERROR, "Could not find plugin: ", name);
    return;
  }

  // unload
  // load
  // start
}
