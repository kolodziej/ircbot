set(PROGRAMS
    client
)

foreach(PROGRAM ${PROGRAMS})
    add_executable(${PROGRAM} ${PROGRAM}.cpp)
    target_link_libraries(${PROGRAM} ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT} "ircbot")
endforeach()
