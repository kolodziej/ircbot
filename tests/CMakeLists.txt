CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

INCLUDE_DIRECTORIES(
    "../googletest/googletest/include"
)

LINK_DIRECTORIES(
    "../googletest/googletest"
)

ADD_SUBDIRECTORY("../googletest/googletest")


SET(TESTS 
    irc_interpreter
)

FOREACH(TEST "${TESTS}")
    ADD_EXECUTABLE("${TEST}" "${TEST}.cpp")
    TARGET_LINK_LIBRARIES("${TEST}" "gtest gtest_main")
ENDFOREACH(TEST)
