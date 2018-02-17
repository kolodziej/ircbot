set(PROGRAMS
    ircbot
)

foreach(PROGRAM ${PROGRAMS})
    add_executable(program-${PROGRAM} ${PROGRAM}.cpp)
    set_target_properties(program-${PROGRAM}
      PROPERTIES OUTPUT_NAME ${PROGRAM})
    target_link_libraries(program-${PROGRAM} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ircbot dl)
endforeach()

install(TARGETS program-ircbot
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
