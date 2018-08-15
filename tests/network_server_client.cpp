#include "gtest/gtest.h"

#include "ircbot/cerr_log_output.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/network/server.hpp"

#include <string>

#include <boost/asio.hpp>

using namespace ircbot;

namespace asio = boost::asio;

class TestClient : public network::Client<asio::ip::tcp::socket> {
 public:
  TestClient(asio::ip::tcp::endpoint endpoint)
      : network::Client<asio::ip::tcp::socket>{endpoint} {}

 private:
  void onWrite(const size_t bytes_transferred) {
    LOG(INFO, "Client ", endpoint(), " transferred ", bytes_transferred,
        " bytes");
  }

  void onRead(const std::string& data) {
    LOG(INFO, "Client ", endpoint(), " received: ", data);
  }
};

TEST(NetworkServerClientTest, OneConnection) {
  const uint16_t port{8888};
  const std::string host{"127.0.0.1"};

  Logger& logger = Logger::getInstance();
  logger.addOutput(std::make_unique<CerrLogOutput>(LogLevel::DEBUG));

  auto& ctx = network::ContextProvider::getInstance();

  asio::ip::tcp::endpoint server_endpoint{asio::ip::tcp::v4(), port};
  network::Server<asio::ip::tcp> server{server_endpoint};
  server.start();

  asio::ip::tcp::resolver resolver{ctx.getContext()};
  auto endpoint_iterator = resolver.resolve({host, std::to_string(port)});
  auto endpoint = *endpoint_iterator;

  network::Client<asio::ip::tcp::socket> client{endpoint};
  client.connect();
  client.send("testing message");
  client.disconnect();

  LOG(INFO, "Running context!");
  ctx.run();

  LOG(INFO, "Stopping server");
  server.stop();

  LOG(INFO, "Stopping context");
  ctx.stop();
}
