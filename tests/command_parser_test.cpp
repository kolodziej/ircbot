#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "ircbot/logger.hpp"
#include "ircbot/command_parser.hpp"
#include "ircbot/cerr_log_output.hpp"

TEST(CommandParserTest, CorrectCommands) {
  Logger& logger = Logger::getInstance();
  logger.addOutput(std::make_unique<CerrLogOutput>(LogLevel::DEBUG));

  ParserConfig config{'/'};

  std::vector<std::string> str_commands{
    "/test arg1 arg2\r\n",
    "/test arg1\n",
    "/CoM2 \"arg1\" \"arg'2\"\n",
    "/2Com 'arg1' arg2'arg3 arg4'\n",
    "/x\n",
    "/x\r\n",
    "/x arg\\ with\\ spaces' !'\n"
  };

  CommandParser parser{config};
  for (const auto& cmd : str_commands) {
    parser.parse(cmd);
  }

  EXPECT_EQ(parser.commandsCount(), str_commands.size());

  std::vector<CommandParser::Command> commands;
  while (not parser.commandsEmpty()) {
    commands.push_back(parser.getCommand());
  }

  ASSERT_TRUE(commands.size() >= 1);

  // /test arg1 arg2\r\n
  EXPECT_EQ(commands[0].command, "test");
  EXPECT_EQ(commands[0].arguments[0], "arg1");
  EXPECT_EQ(commands[0].arguments[1], "arg2");

  ASSERT_TRUE(commands.size() >= 2);

  // /test arg1\n
  EXPECT_EQ(commands[1].command, "test");
  EXPECT_EQ(commands[1].arguments[0], "arg1");

  ASSERT_TRUE(commands.size() >= 3);

  // /CoM2 \"arg1\" \"arg'2\"\n
  EXPECT_EQ(commands[2].command, "CoM2");
  EXPECT_EQ(commands[2].arguments[0], "arg1");
  EXPECT_EQ(commands[2].arguments[1], "arg'2");

  ASSERT_TRUE(commands.size() >= 4);

  // /2Com 'arg1' arg2'arg3 arg4'\n
  EXPECT_EQ(commands[3].command, "2Com");
  EXPECT_EQ(commands[3].arguments[0], "arg1");
  EXPECT_EQ(commands[3].arguments[1], "arg2arg3 arg4");

  ASSERT_TRUE(commands.size() >= 5);

  // /x
  EXPECT_EQ(commands[4].command, "x");
  EXPECT_TRUE(commands[4].arguments.empty());

  ASSERT_TRUE(commands.size() >= 6);

  // /x\r\n
  EXPECT_EQ(commands[5].command, "x");
  EXPECT_TRUE(commands[5].arguments.empty());

  ASSERT_TRUE(commands.size() >= 7);

  // /x arg\ with\ spaces' !' 
  EXPECT_EQ(commands[6].command, "x");
  EXPECT_EQ(commands[6].arguments[0], "arg with spaces !");
}

TEST(CommandParserTest, IncorrectCommands) {
  ParserConfig config{'/'};

  std::vector<std::string> str_commands{
    "/x! arg\n",
    "/x_ 1arg\n",
    "x arg\n",
    "!a\n",
  };

  CommandParser parser{config};
  for (const auto& cmd : str_commands) {
    parser.parse(cmd);
  }

  EXPECT_EQ(parser.commandsCount(), 0u);
}
