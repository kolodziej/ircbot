#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "ircbot/logger.hpp"
#include "ircbot/irc_parser.hpp"
#include "ircbot/irc_message.hpp"

TEST(IRCParserTest, CorrectSimpleMessages) {
  std::vector<std::string> str_messages{
    ":KolK1!~KolK2@example.com PRIVMSG #cmake :testing message\r\n",
    "PRIVMSG #agaran :dzień dobry\r\n",
    ":TestServer PRIVMSG #cmake parameter :testing message\r\n",
    ":TestServer 123 param1 param2\r\n"
  };

  IRCParser parser;

  for (const auto& msg : str_messages) {
    parser.parse(msg);
  }

  ASSERT_EQ(str_messages.size(), parser.messagesCount());

  std::vector<IRCMessage> messages;
  while (not parser.messagesEmpty()) {
    messages.push_back(parser.getMessage());
  }

  ASSERT_EQ(messages.size(), str_messages.size());

  ASSERT_TRUE(messages[0].servername.empty());
  ASSERT_EQ(messages[0].nick, "KolK1");
  ASSERT_EQ(messages[0].user, "~KolK2");
  ASSERT_EQ(messages[0].host, "example.com");
  ASSERT_EQ(messages[0].command, "PRIVMSG");
  ASSERT_EQ(messages[0].params.size(), 2u);
  ASSERT_EQ(messages[0].params[0], "#cmake");
  ASSERT_EQ(messages[0].params[1], "testing message");

  ASSERT_TRUE(messages[1].servername.empty());
  ASSERT_TRUE(messages[1].nick.empty());
  ASSERT_TRUE(messages[1].user.empty());
  ASSERT_TRUE(messages[1].host.empty());
  ASSERT_EQ(messages[1].command, "PRIVMSG");
  ASSERT_EQ(messages[1].params.size(), 2u);
  ASSERT_EQ(messages[1].params[0], "#agaran");
  ASSERT_EQ(messages[1].params[1], "dzień dobry");

  ASSERT_EQ(messages[2].servername, "TestServer");
  ASSERT_TRUE(messages[2].nick.empty());
  ASSERT_TRUE(messages[2].user.empty());
  ASSERT_TRUE(messages[2].host.empty());
  ASSERT_EQ(messages[2].command, "PRIVMSG");
  ASSERT_EQ(messages[2].params.size(), 3u);
  ASSERT_EQ(messages[2].params[0], "#cmake");
  ASSERT_EQ(messages[2].params[1], "parameter");
  ASSERT_EQ(messages[2].params[2], "testing message");

  ASSERT_EQ(messages[3].servername, "TestServer");
  ASSERT_TRUE(messages[3].nick.empty());
  ASSERT_TRUE(messages[3].user.empty());
  ASSERT_TRUE(messages[3].host.empty());
  ASSERT_EQ(messages[3].command, "123");
  ASSERT_EQ(messages[3].params.size(), 2u);
  ASSERT_EQ(messages[3].params[0], "param1");
  ASSERT_EQ(messages[3].params[1], "param2");
}

TEST(IRCParserTest, NoCR) {
  std::string str_cmd =
      ":User!~TestUser@example.com PRIVMSG #cmake testing message :long message\n";

  IRCParser parser;
  parser.parse(str_cmd);
  ASSERT_EQ(parser.messagesCount(), 1u);

  auto msg = parser.getMessage();

  ASSERT_TRUE(msg.servername.empty());
  ASSERT_EQ(msg.nick, "User");
  ASSERT_EQ(msg.user, "~TestUser");
  ASSERT_EQ(msg.host, "example.com");
  ASSERT_EQ(msg.command, "PRIVMSG");
  ASSERT_EQ(msg.params.size(), 4u);
  ASSERT_EQ(msg.params[0], "#cmake");
  ASSERT_EQ(msg.params[1], "testing");
  ASSERT_EQ(msg.params[2], "message");
  ASSERT_EQ(msg.params[3], "long message");
}
