FILE(GLOB_RECURSE SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.frag")

add_executable(arap main.cpp arap.cpp snippets.lua star.frag energy.frag)
target_link_libraries(arap PRIVATE slope)

# Run with --project_path arap. deck.yaml, snippets.lua and the shaders are read
# from this folder, and reload while the program runs.
