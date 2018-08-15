#include "gtest/gtest.h"

#include "ircbot/cerr_log_output.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/network/client.hpp"
#include "ircbot/network/server.hpp"

#include <chrono>
#include <string>
#include <thread>

#include <boost/asio.hpp>

using namespace ircbot;

namespace asio = boost::asio;

class NetworkServerClientTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    Logger& logger = Logger::getInstance();
    logger.addOutput(std::make_unique<CerrLogOutput>(LogLevel::DEBUG));
  }

  virtual void TearDown() { Logger::getInstance().clearOutputs(); }
};

class TestClient : public network::Client<asio::ip::tcp> {
 public:
  TestClient(network::Client<asio::ip::tcp>::ProtocolType::endpoint endpoint)
      : network::Client<asio::ip::tcp>{endpoint} {}

  TestClient(network::Client<asio::ip::tcp>::ProtocolType::socket&& socket)
      : network::Client<asio::ip::tcp>{std::move(socket)} {}

 private:
  void onWrite(const size_t bytes_transferred) {
    LOG(INFO, "Client ", endpoint(), " transferred ", bytes_transferred,
        " bytes");
  }

  void onRead(const std::string& data) {
    LOG(INFO, "Client ", endpoint(), " received: ", data);
  }
};

TEST_F(NetworkServerClientTest, MultiConnections) {
  const uint16_t port{8889};
  const std::string host{"127.0.0.1"};

  auto& ctx = network::ContextProvider::getInstance();
  ctx.run();

  TestClient::ProtocolType::endpoint server_endpoint{asio::ip::tcp::v4(), port};
  network::Server<TestClient> server{server_endpoint};
  server.start();

  TestClient::ProtocolType::resolver resolver{ctx.getContext()};
  auto endpoint_iterator = resolver.resolve({host, std::to_string(port)});
  auto endpoint = endpoint_iterator->endpoint();

  std::vector<std::unique_ptr<TestClient>> clients;
  for (int i = 0; i < 10; ++i) {
    auto p = std::make_unique<TestClient>(endpoint);
    p->connect();
    p->receive();
    std::string msg = "testing_message_" + std::to_string(i);
    p->send(msg);
    clients.emplace_back(std::move(p));
  }

  // @TODO: Wait for all send operations
  using namespace std::literals::chrono_literals;
  std::this_thread::sleep_for(5s);

  // clients.clear();
  server.stop();
  ctx.stop();
}
