# Slope examples

Standalone examples for [slope](https://github.com/baptiste-genest/slope), a
C++ library for presentations that compute. Each directory is one example: an
executable, its `deck.yaml` manifest and its assets.

Ordered by difficulty, easiest first.

| Example | What it shows |
| --- | --- |
| [`hello`](hello) | slides written entirely in `deck.yaml`, with steps, labels, a box, an arrow and a stack |
| [`orbit`](orbit) | a moving 3D scene built from Lua snippets, with a tunable orbit and a label that tracks the moon |
| [`shaders`](shaders) | three fragment shaders, with uniforms declared in the manifest, a 2D distance field and a 3D one traced along polyscope's rays |
| [`ripple`](ripple) | the smallest useful C++ side, with a mesh, an updater whose shape is a Lua snippet, and the two ways of showing a scalar field |
| [`spheres`](spheres) | a whole scene in one fragment shader, animated by the deck's keyframes and tuned while it runs |
| [`arap`](arap) | as-rigid-as-possible deformation: an Eigen solver, the slides that explain it, and a handle to drag during the talk |

The first three have nothing in their `main.cpp` beyond the call that starts the
show. `ripple` is where C++ starts, at about fifty lines. Start at the top.

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
slope_example(my_example)
```

That picks up the `.cpp` and `.h` files to build, and lists `deck.yaml`, the
`.lua` snippets and the `.frag` shaders as sources too, so they appear in the
IDE beside the code. Nothing is copied next to the binary, the example reads
them from its own directory through `--project_path`.
