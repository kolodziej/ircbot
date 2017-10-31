include_directories("${CMAKE_SOURCE_DIR}/library/include")

set(PLUGINS
    init
    ping
    helloworld
)

foreach(PLUGIN ${PLUGINS})
    add_library(plugin-${PLUGIN} MODULE ${PLUGIN}/${PLUGIN}.cpp)
    set_target_properties(plugin-${PLUGIN}
        PROPERTIES OUTPUT_NAME "${PLUGIN}" PREFIX "")
    install(TARGETS plugin-${PLUGIN}
        DESTINATION "usr/lib/ircbot/plugins/"
    )
endforeach()
