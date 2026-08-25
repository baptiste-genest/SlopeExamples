# Builds one example. The manifest, the snippets and the shaders are listed as
# sources so they show up in the IDE beside the code, marked header-only so
# nothing compiles them. CONFIGURE_DEPENDS re-globs when a file is added.
function(slope_example name)
    file(GLOB code   CONFIGURE_DEPENDS "*.cpp" "*.h")
    file(GLOB assets CONFIGURE_DEPENDS "*.yaml" "*.lua" "*.frag" "*.glsl" "*.tex")

    set_source_files_properties(${assets} PROPERTIES HEADER_FILE_ONLY TRUE)

    add_executable(${name} ${code} ${assets})
    target_link_libraries(${name} PRIVATE slope)

    source_group("deck" FILES ${assets})
endfunction()
