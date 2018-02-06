include_directories("${CMAKE_SOURCE_DIR}/library/include")

include("${CMAKE_SOURCE_DIR}/cmake/plugin.cmake")

set(PLUGINS
    init
    ping
    helloworld
    clipboard
)

foreach(PLUGIN ${PLUGINS})
    add_subdirectory(${PLUGIN})
endforeach()
