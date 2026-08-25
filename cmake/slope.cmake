# Pulls slope from GitHub. It fetches polyscope, eigen, spdlog, fmt, yaml-cpp
# and luajit itself, so this is the only dependency to declare here.
# Set GIT_TAG to a release (v0.1.4, ...) for a fixed version.
include(FetchContent)
FetchContent_Declare(
  slope
  GIT_REPOSITORY https://github.com/baptiste-genest/slope.git
  GIT_TAG main
)
FetchContent_MakeAvailable(slope)
