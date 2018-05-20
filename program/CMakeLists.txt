set(PROGRAMS
    ircbot
)

foreach(PROGRAM ${PROGRAMS})
    add_executable(program-${PROGRAM} ${PROGRAM}.cpp)
    set_target_properties(program-${PROGRAM}
      PROPERTIES OUTPUT_NAME ${PROGRAM})
    target_link_libraries(program-${PROGRAM}
        ircbot dl
        ${Boost_LIBRARIES}
        ${Protobuf_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${PYTHON_LIBRARIES} 
    )
endforeach()

install(TARGETS program-ircbot
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
