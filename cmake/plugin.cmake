function(compile_plugin name files)
    add_library(plugin-${name} MODULE ${files})
    set_target_properties(plugin-${name}
        PROPERTIES OUTPUT_NAME "${name}" PREFIX "")
    install(TARGETS plugin-${name}
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/ircbot/plugins/"
    )
endfunction(compile_plugin)
