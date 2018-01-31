#include "ircbot/client.hpp"

#include <stdexcept>
#include <thread>
#include <chrono>
#include <stdexcept>

#include <dlfcn.h>

#include "ircbot/plugin.hpp"
#include "ircbot/lua_plugin.hpp"
#include "ircbot/helpers.hpp"

Client::Client(asio::io_service& io_service, Config cfg) :
    m_io_service{io_service},
    m_socket{io_service},
    m_cfg{cfg},
    m_running{false}
{
  try {
    std::string server = m_cfg.tree().get<std::string>("server");
    uint16_t port = m_cfg.tree().get<uint16_t>("port");

    connect(server, port);

    initializePlugins();

  } catch (pt::ptree_bad_path& exc) {
    LOG(ERROR, "Could not find path in configuration!");
    LOG(ERROR, exc.what());
    throw std::runtime_error{"Client initialization failed!"};
  } catch (pt::ptree_bad_data& exc) {
    LOG(ERROR, "Bad data format in Client's configuration!");
    LOG(ERROR, exc.what());
    throw std::runtime_error{"Client initialization failed!"};
  }
}

void Client::connect(std::string host, uint16_t port) {
  asio::ip::tcp::resolver resolver{m_io_service};

  boost::system::error_code ec;

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
  for (auto cfg : m_cfg.tree().get_child("plugins")) {
    LOG(INFO, "Processing plugin: ", cfg.first);
    auto name_pred = [pn = cfg.first] (const std::unique_ptr<Plugin>& p) {
      return p->name() == pn;
    };

    std::string soPath =
      cfg.second.get("soPath", std::string());

    std::string luaPath =
      cfg.second.get("luaPath", std::string());

    auto it = std::find_if(m_plugins.begin(), m_plugins.end(), name_pred);
    if (it != m_plugins.end()) {
      if (soPath.empty() and luaPath.empty()) {
        std::unique_ptr<Plugin>& it_plugin = *it;
        it_plugin->setConfig(Config(cfg.second));
      } else {
        LOG(WARNING, "Cannot load plugin ", cfg.first, ". Such plugin exists!");
      }

    } else {
      LOG(INFO, "There is no ", cfg.first, " plugin yet. Trying to load...");
      std::unique_ptr<Plugin> plugin{nullptr};
      if (not soPath.empty()) {
        LOG(INFO, "Loading SO plugin from: ", soPath);
        plugin = loadSoPlugin(soPath);
      } else if (not luaPath.empty()) {
        LOG(INFO, "Loading LUA plugin from: ", luaPath);
        plugin = std::make_unique<LuaPlugin>(*this, cfg.first, luaPath);
      }

      if (plugin != nullptr) {
        LOG(INFO, "Plugin ", cfg.first, " loaded. Adding to Client.");
        plugin->setConfig(Config(cfg.second));
        addPlugin(std::move(plugin));
      }
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
    DEBUG("Parsed commands: ", m_parser.commandsCount());
    while (m_parser.commandsCount() > 0) {
      auto cmd = m_parser.getCommand();
      DEBUG("Passing command to plugins: ", cmd.toString());
      for (auto& plugin : m_plugins) {
        if (plugin->filter(cmd)) {
          DEBUG("Plugin ", plugin->name(),
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
  LOG(INFO, "Adding plugin ", plugin->name());
  m_plugins.push_back(std::move(plugin));
}

void Client::removePlugin(const std::string& name) {
  auto pred = [&name](const auto& plugin) { return plugin->name() == name; };
  auto remove_it = std::find_if(m_plugins.begin(), m_plugins.end(), pred);

  if (remove_it == m_plugins.end()) {
    throw std::runtime_error{"Could not find such plugin!"};
  }

  m_plugins.erase(remove_it);
}

std::vector<std::string> Client::listPlugins() const {
  std::vector<std::string> names;
  for (const auto& plugin : m_plugins) {
    names.push_back(plugin->name());
  }

  return names;
}

std::unique_ptr<Plugin> Client::loadSoPlugin(const std::string& fname) {
  void* plugin = dlopen(fname.data(), RTLD_NOW);
  if (plugin == nullptr) {
    LOG(ERROR, "Could not load file ", fname, ": ", dlerror());
    return nullptr;
  }

  std::function<std::unique_ptr<Plugin>(Client*)> func =
      reinterpret_cast<std::unique_ptr<Plugin> (*)(Client*)>
      (dlsym(plugin, "getPlugin"));

  if (func == nullptr) {
    LOG(ERROR, "Could not load plugin from ", fname, ": ", dlerror());
    return nullptr;
  }

  return func(this);
}

void Client::startPlugins() {
  for (auto& plugin : m_plugins) {
    LOG(INFO, "Starting plugin ", plugin->name());
    plugin->spawn();
  }
}

void Client::stopPlugins() {
  for (auto& plugin : m_plugins) {
    LOG(INFO, "Stopping plugin ", plugin->name());
    plugin->stop();
  }
}
