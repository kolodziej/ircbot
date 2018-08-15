set(TESTS
    irc_parser_test
    command_parser_test
    network_buffer
    network_server_client
)

foreach(TEST ${TESTS})
    add_executable(${TEST} "${TEST}.cpp")
    target_link_libraries(${TEST} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${Protobuf_LIBRARIES} gtest gtest_main ircbot dl)
    add_test(${TEST} ${TEST})
endforeach()
