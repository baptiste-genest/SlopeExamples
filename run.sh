#!/usr/bin/env bash
# Build (if needed) and run one example.
#
#   ./run.sh spheres            present it
#   ./run.sh spheres --export   render every slide and write a pdf
set -e
cd "$(dirname "$0")"

example=${1:?usage: ./run.sh <example> [slope options...]}
shift

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"

# --project_path points at the sources, so deck.yaml and the shaders reload
# while the program runs and the Tuner saves back into the repo
./build/"$example"/"$example" --project_path "$example" "$@"
