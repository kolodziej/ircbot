set(TESTS
    irc_parser_test
    client_test
)

foreach(TEST ${TESTS})
    add_executable(${TEST} "${TEST}.cpp")
    target_link_libraries(${TEST} gtest gtest_main ircbot dl)
endforeach()
