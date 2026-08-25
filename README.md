# Slope examples

Standalone examples for [slope](https://github.com/baptiste-genest/slope), a
C++ library for presentations that compute. Each directory is one example: an
executable, its `deck.yaml` manifest and its assets.

| Example | What it shows |
| --- | --- |
| [`spheres`](spheres) | a whole scene in one fragment shader, animated by the deck's keyframes and tuned while it runs |
| [`arap`](arap) | as-rigid-as-possible deformation: an Eigen solver, the slides that explain it, and a handle to drag during the talk |

## Building

Slope needs `pdflatex`, ImageMagick's `convert` and, for video, `ffmpeg`. On
Debian or Ubuntu:

```
sudo apt install texlive-latex-extra imagemagick ffmpeg
```

ImageMagick must be allowed to convert pdf to png. See
[the docs](https://slopedoc.github.io/cmake/) if `convert` refuses.

CMake fetches slope, which fetches polyscope, eigen, spdlog, fmt, yaml-cpp and
luajit. The first configure takes a while.

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

## Running

Slope needs to be told where the project folder is:

```
./build/spheres/spheres --project_path spheres
```

`./run.sh spheres` does the same and builds first. Point `--project_path` at
the sources: `deck.yaml`, the shaders and the snippets are then read from the
repository, so editing one of them changes the running presentation on the next
frame, and the Tuner saves back into it.

Useful flags: `--export` renders every step and writes a pdf next to the deck
instead of presenting, `--resolution 1600x900` sets the window size. The full
list is in [the docs](https://slopedoc.github.io/options/).

## Adding an example

Create a directory next to the others and add one line to the root
`CMakeLists.txt`:

```cmake
add_subdirectory(my_example)
```

Then give it a `CMakeLists.txt` of its own:

```cmake
add_executable(my_example main.cpp)
target_link_libraries(my_example PRIVATE slope)
```
