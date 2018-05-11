#ifndef _PYIRCBOT_BOT_HPP
#define _PYIRCBOT_BOT_HPP

#include <thread>
#include <atomic>
#include <array>
#include <pybind11/pybind11.h>
#include <boost/asio.hpp>

#include "pyircbot/plugin.hpp"

#include "init.pb.h"
#include "irc_message.pb.h"
#include "control.pb.h"

namespace py = pybind11;
namespace asio = boost::asio;

class Bot {
 public:
  Bot(const std::string& hostname, uint16_t port, Plugin plugin);
  ~Bot();

  std::string hostname() const;
  uint16_t port() const;

  void start();
  void async_start() {}
  void stop();
  void send(const std::string& data);

 public: // change to private
  void connect();

  void receive();
  void initialize(const std::string& name, const std::string& token);

  void initResponse(const ircbot::InitResponse& resp);
  void ircMessage(const ircbot::IrcMessage& irc_msg);
  void controlRequest(const ircbot::ControlRequest& req);

  void parse(size_t bytes);

 private:
  const std::string m_hostname;
  const uint16_t m_port;

  Plugin m_plugin;

  asio::io_service m_io;
  asio::ip::tcp::socket m_socket;

  std::thread m_io_thread;

  std::array<char, 8192> m_buffer;

  bool m_started;
};

#endif
