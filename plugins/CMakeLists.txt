set(PLUGINS
    helloworld
)

foreach(PLUGIN ${PLUGINS})
    add_library(${PLUGIN} MODULE ${PLUGIN}/${PLUGIN}.cpp)
endforeach()
