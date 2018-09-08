#include "gtest/gtest.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "ircbot/cerr_log_output.hpp"
#include "ircbot/command_parser.hpp"
#include "ircbot/logger.hpp"
#include "ircbot/unexpected_character.hpp"

using namespace ircbot;

TEST(CommandParserTest, CorrectCommands) {
  Logger& logger = Logger::getInstance();
  logger.addOutput(std::make_unique<CerrLogOutput>(LogLevel::DEBUG));

  ParserConfig config{'/', false};

  std::vector<std::string> str_commands{"/test arg1 arg2",
                                        "/test arg1",
                                        "/CoM2 \"arg1\" \"arg'2\"",
                                        "/2Com 'arg1' arg2'arg3 arg4'",
                                        "/x",
                                        "/x",
                                        "/x arg\\ with\\ spaces' !'",
                                        "/x \"\\\"arg1\\\" arg2\""};

  CommandParser parser{config};
  CommandParser::Command command;

  // /test arg1 arg2
  command = parser.parse(str_commands[0]);
  EXPECT_EQ(command.command, "test");
  EXPECT_EQ(command.arguments[0], "arg1");
  EXPECT_EQ(command.arguments[1], "arg2");

  // /test arg1
  command = parser.parse(str_commands[1]);
  EXPECT_EQ(command.command, "test");
  EXPECT_EQ(command.arguments[0], "arg1");

  // /CoM2 "arg1" "arg'2"
  command = parser.parse(str_commands[2]);
  EXPECT_EQ(command.command, "CoM2");
  EXPECT_EQ(command.arguments[0], "arg1");
  EXPECT_EQ(command.arguments[1], "arg'2");

  // /2Com 'arg1' arg2'arg3 arg4'
  command = parser.parse(str_commands[3]);
  EXPECT_EQ(command.command, "2Com");
  EXPECT_EQ(command.arguments[0], "arg1");
  EXPECT_EQ(command.arguments[1], "arg2arg3 arg4");

  // /x
  command = parser.parse(str_commands[4]);
  EXPECT_EQ(command.command, "x");
  EXPECT_TRUE(command.arguments.empty());

  // /x
  command = parser.parse(str_commands[5]);
  EXPECT_EQ(command.command, "x");
  EXPECT_TRUE(command.arguments.empty());

  // /x arg\ with\ spaces' !'
  command = parser.parse(str_commands[6]);
  EXPECT_EQ(command.command, "x");
  EXPECT_EQ(command.arguments[0], "arg with spaces !");

  // /x "\"arg1\" arg2"
  command = parser.parse(str_commands[7]);
  EXPECT_EQ(command.command, "x");
  EXPECT_EQ(command.arguments[0], "\"arg1\" arg2");
}

TEST(CommandParserTest, IncorrectCommands) {
  ParserConfig config{'/', false};

  std::vector<std::string> str_commands{
      "/x! arg\n",
      "/x_ 1arg\n",
      "x arg\n",
      "!a\n",
  };

  CommandParser parser{config};
  EXPECT_THROW(parser.parse(str_commands[0]), UnexpectedCharacter);
  EXPECT_THROW(parser.parse(str_commands[1]), UnexpectedCharacter);
  EXPECT_THROW(parser.parse(str_commands[2]), UnexpectedCharacter);
  EXPECT_THROW(parser.parse(str_commands[3]), UnexpectedCharacter);
}
