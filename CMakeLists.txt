cmake_minimum_required(VERSION 3.14)
project(slope_examples LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(slope)
include(example)

# one directory per example, each an executable linking slope, easiest first
add_subdirectory(hello)
add_subdirectory(orbit)
add_subdirectory(shaders)
add_subdirectory(ripple)
add_subdirectory(spheres)
add_subdirectory(arap)
