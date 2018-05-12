#include "pyircbot/bot.hpp"

#include <iostream>

#include "message.pb.h"
#include "ircbot/version.hpp"

Bot::Bot(const std::string& hostname, uint16_t port, Plugin plugin) :
  m_hostname{hostname},
  m_port{port},
  m_plugin{plugin},
  m_socket{m_io},
  m_started{false} {
}

Bot::~Bot() {
  if (m_started)
    stop();

  wait();
}

std::string Bot::hostname() const {
  return m_hostname;
}

uint16_t Bot::port() const {
  return m_port;
}

void Bot::connect() {
  asio::ip::tcp::resolver resolver{m_io};
  auto endpoint_it = resolver.resolve({ m_hostname, std::to_string(m_port) });
  auto endpoint = endpoint_it->endpoint();

  m_socket.connect(endpoint);
}

bool Bot::connected() const {
  return m_socket.is_open();
}

void Bot::start() {
  m_started = true;
  m_io.run();
}

void Bot::async_start() {
  m_started = true;
  m_io_thread = std::move(std::thread{[this] { m_io.run(); }});
}

void Bot::stop() {
  m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
  m_socket.cancel();
  m_socket.close();
}

void Bot::wait() {
  if (m_io_thread.joinable())
    m_io_thread.join();
}

void Bot::send(const std::string& data) {
  m_socket.send(asio::buffer(data.data(), data.size()));
}

void Bot::receive() {
  auto callback = [this](const boost::system::error_code& ec, std::size_t bytes) {
    if (ec == 0) {
      parse(bytes);
    } else {
      return;
    }

    receive();
  };

  m_socket.async_receive(
    asio::buffer(m_buffer.data(), m_buffer.size()),
    callback
  );
}

void Bot::parse(size_t bytes) {
  ircbot::Message msg; 
  auto msg_str = std::string{m_buffer.data(), bytes};
  msg.ParseFromString(msg_str);

  switch (msg.type()) {
    case ircbot::Message::INIT_RESPONSE:
      initResponse(msg.init_resp());
      break;
    case ircbot::Message::IRC_MESSAGE:
      ircMessage(msg.irc_msg());
      break;
    case ircbot::Message::CONTROL_REQUEST:
      controlRequest(msg.ctrl_req());
      break;
  }
}

void Bot::initialize(const std::string& name, const std::string& token) {
  ircbot::Message msg;
  msg.set_type(ircbot::Message::INIT_REQUEST);
  
  ircbot::InitRequest* req = msg.mutable_init_req();
  req->set_name(name);
  req->set_token(token);

  std::string serialized;
  msg.SerializeToString(&serialized);

  send(serialized);
}

void Bot::initResponse(const ircbot::InitResponse& resp) {
  if (resp.status() == ircbot::InitResponse::OK) {
    std::cout << "initialized. can continue!\n";
  } else {
    std::cout << "some error occurred during initialization\n";
  }
}

void Bot::ircMessage(const ircbot::IrcMessage& irc_msg) {
  IRCMessage msg;
  msg.servername = irc_msg.servername();
  msg.user = irc_msg.user();
  msg.nick = irc_msg.nick();
  msg.host = irc_msg.host();
  msg.command = irc_msg.command();
  for (int i = 0; i < irc_msg.params_size(); ++i) {
    msg.params.push_back(irc_msg.params(i));
  }
  m_plugin.onMessage(msg);
}

void Bot::controlRequest(const ircbot::ControlRequest& req) {
  switch (req.type()) {
    case ircbot::ControlRequest::INIT:
      m_plugin.onInit();
      break;
    case ircbot::ControlRequest::SHUTDOWN:
      m_plugin.onShutdown();
      stop();
      break;
    case ircbot::ControlRequest::RELOAD:
      m_plugin.onReload();
      break;
    case ircbot::ControlRequest::RESTART:
      m_plugin.onRestart();
      break;
  }
}
