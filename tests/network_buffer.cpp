#include "gtest/gtest.h"

#include "ircbot/network/buffer.hpp"

TEST(NetworkBuffer, Append) {
  using ircbot::network::Buffer;

  Buffer buf{10};
  EXPECT_TRUE(buf.empty());
  std::string tmp{"0123456789"};
  buf.append(tmp);

  EXPECT_EQ(buf.size(), tmp.size());

  EXPECT_TRUE(buf.full());
}

TEST(NetworkBuffer, Get0) {
  using ircbot::network::Buffer;

  std::string tmp{"0123456789"};
  const size_t buf_size = tmp.size();
  Buffer buf{buf_size};

  buf.append(tmp);

  const size_t len = 3;
  auto data = buf.get(len);
  std::string part{tmp.begin(), tmp.begin() + len};
  EXPECT_EQ(data, part);

  EXPECT_EQ(buf.size(), buf_size - len);
}

TEST(NetworkBuffer, Get1) {
  using ircbot::network::Buffer;

  std::string tmp{"0123456789"};
  const size_t buf_size = tmp.size();
  Buffer buf{buf_size};

  buf.append(tmp);
  std::string part{"abc"};
  buf.append(part);

  auto data = buf.get(buf_size);

  std::string x{"3456789abc"};
  EXPECT_EQ(data, x);
}

TEST(NetworkBuffer, Overflow0) {
  using ircbot::network::Buffer;

  std::string tmp{"0123456789"};
  std::string rtmp{tmp.rbegin(), tmp.rend()};
  const size_t buf_size = tmp.size();
  Buffer buf{buf_size};
  buf.append(tmp);
  buf.append(rtmp);

  auto data = buf.get(buf_size);
  EXPECT_EQ(data, rtmp);
}

TEST(NetworkBuffer, Overflow1) {
  using ircbot::network::Buffer;

  std::string tmp{"01234567899876543210"};
  const size_t buf_size = tmp.size() / 2;
  Buffer buf{buf_size};
  buf.append(tmp);
  auto data = buf.get();

  std::string test{tmp.begin() + buf_size, tmp.end()};
  EXPECT_EQ(data, test);
}
