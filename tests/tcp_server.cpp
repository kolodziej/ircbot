#include "gtest/gtest.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "ircbot/tcp_server.hpp"

#include <boost/asio.hpp>

using namespace ircbot;

TEST(TcpServer, TestPing) { boost::asio::io_service io; }
