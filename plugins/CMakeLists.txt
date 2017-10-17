set(PLUGINS
    helloworld
)

foreach(PLUGIN ${PLUGINS})
    add_library(plugin-${PLUGIN} MODULE ${PLUGIN}/${PLUGIN}.cpp)
    set_target_properties(plugin-${PLUGIN}
        PROPERTIES OUTPUT_NAME "${PLUGIN}" PREFIX "")
endforeach()
