#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "ircbot/logger.hpp"
#include "ircbot/command_parser.hpp"

TEST(CommandParserTest, CorrectCommands) {
  ParserConfig config{'/'};

  std::vector<std::string> commands{
    "/test arg1 arg2\r\n",
    "/test arg1\n",
    "/CoM2 \"arg1\" \"arg'2\"\n",
    "/2Com \'arg1\' arg2\'arg3 arg4\'\n"
  };

  CommandParser parser{config};
  for (const auto& cmd : commands) {
    parser.parse(cmd);
  }
}
