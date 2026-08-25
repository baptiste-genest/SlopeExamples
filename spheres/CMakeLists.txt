add_executable(spheres main.cpp)
target_link_libraries(spheres PRIVATE slope)

# Nothing is copied next to the binary. Run with --project_path spheres, and
# deck.yaml and the shader are read from this folder, where the Tuner saves too.
