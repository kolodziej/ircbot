include_directories("${CMAKE_SOURCE_DIR}/library/include")

include("${CMAKE_SOURCE_DIR}/cmake/plugin.cmake")

set(PLUGINS
    init
    ping
    join
    helloworld
    clipboard
    simple_commands
)

foreach(PLUGIN ${PLUGINS})
    add_subdirectory(${PLUGIN})
endforeach()
