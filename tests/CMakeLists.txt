set(TESTS
    irc_parser_test
    command_parser_test
)

foreach(TEST ${TESTS})
    add_executable(${TEST} "${TEST}.cpp")
    target_link_libraries(${TEST} gtest gtest_main ircbot dl)
    add_test(${TEST} ${TEST})
endforeach()
