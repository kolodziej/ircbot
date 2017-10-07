SET(TESTS
    irc_parser_test
)

FOREACH(TEST ${TESTS})
    ADD_EXECUTABLE(${TEST} "${TEST}.cpp")
    TARGET_LINK_LIBRARIES(${TEST} gtest gtest_main ircbot)
ENDFOREACH()
