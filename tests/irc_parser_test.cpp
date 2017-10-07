#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "ircbot/logger.hpp"
#include "ircbot/irc_parser.hpp"
#include "ircbot/irc_command.hpp"

TEST(IRCParserTest, CorrectSimpleMessages) {
  std::vector<std::string> str_commands{
    ":KolK1!~KolK2@example.com PRIVMSG #cmake :testing message\r\n",
    "PRIVMSG #agaran :dzień dobry\r\n",
    ":TestServer PRIVMSG #cmake parameter :testing message\r\n",
    ":TestServer 123 param1 param2\r\n"
  };

  IRCParser parser;

  for (const auto& cmd : str_commands) {
    parser.parse(cmd);
  }

  ASSERT_EQ(str_commands.size(), parser.commandsCount());

  std::vector<IRCCommand> commands;
  while (not parser.commandsEmpty()) {
    commands.push_back(parser.getCommand());
  }

  ASSERT_EQ(commands.size(), str_commands.size());

  ASSERT_TRUE(commands[0].servername.empty());
  ASSERT_EQ(commands[0].nick, "KolK1");
  ASSERT_EQ(commands[0].user, "~KolK2");
  ASSERT_EQ(commands[0].host, "example.com");
  ASSERT_EQ(commands[0].command, "PRIVMSG");
  ASSERT_EQ(commands[0].params.size(), 2);
  ASSERT_EQ(commands[0].params[0], "#cmake");
  ASSERT_EQ(commands[0].params[1], "testing message");

  ASSERT_TRUE(commands[1].servername.empty());
  ASSERT_TRUE(commands[1].nick.empty());
  ASSERT_TRUE(commands[1].user.empty());
  ASSERT_TRUE(commands[1].host.empty());
  ASSERT_EQ(commands[1].command, "PRIVMSG");
  ASSERT_EQ(commands[1].params.size(), 2);
  ASSERT_EQ(commands[1].params[0], "#agaran");
  ASSERT_EQ(commands[1].params[1], "dzień dobry");

  ASSERT_EQ(commands[2].servername, "TestServer");
  ASSERT_TRUE(commands[2].nick.empty());
  ASSERT_TRUE(commands[2].user.empty());
  ASSERT_TRUE(commands[2].host.empty());
  ASSERT_EQ(commands[2].command, "PRIVMSG");
  ASSERT_EQ(commands[2].params.size(), 3);
  ASSERT_EQ(commands[2].params[0], "#cmake");
  ASSERT_EQ(commands[2].params[1], "parameter");
  ASSERT_EQ(commands[2].params[2], "testing message");

  ASSERT_EQ(commands[3].servername, "TestServer");
  ASSERT_TRUE(commands[3].nick.empty());
  ASSERT_TRUE(commands[3].user.empty());
  ASSERT_TRUE(commands[3].host.empty());
  ASSERT_EQ(commands[3].command, "123");
  ASSERT_EQ(commands[3].params.size(), 2);
  ASSERT_EQ(commands[3].params[0], "param1");
  ASSERT_EQ(commands[3].params[1], "param2");
}

TEST(IRCParserTest, NoCR) {
  std::string str_cmd =
      ":User!~TestUser@example.com PRIVMSG #cmake testing message :long message\n";

  IRCParser parser;
  parser.parse(str_cmd);
  ASSERT_EQ(parser.commandsCount(), 1);

  auto cmd = parser.getCommand();

  ASSERT_TRUE(cmd.servername.empty());
  ASSERT_EQ(cmd.nick, "User");
  ASSERT_EQ(cmd.user, "~TestUser");
  ASSERT_EQ(cmd.host, "example.com");
  ASSERT_EQ(cmd.command, "PRIVMSG");
  ASSERT_EQ(cmd.params.size(), 4);
  ASSERT_EQ(cmd.params[0], "#cmake");
  ASSERT_EQ(cmd.params[1], "testing");
  ASSERT_EQ(cmd.params[2], "message");
  ASSERT_EQ(cmd.params[3], "long message");
}
