find_library(Readline_LIBRARIES readline)

set(PROGRAMS
    ircbot
    ircbotctl
)

foreach(PROGRAM ${PROGRAMS})
    add_executable(program-${PROGRAM} ${PROGRAM}.cpp)
    set_target_properties(program-${PROGRAM}
      PROPERTIES OUTPUT_NAME ${PROGRAM})
    target_link_libraries(program-${PROGRAM} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ircbot dl)
    install(TARGETS program-${PROGRAM}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endforeach()

target_link_libraries(program-ircbotctl ${Readline_LIBRARIES})
