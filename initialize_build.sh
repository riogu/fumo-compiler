#!/bin/bash
set -e

PROJECT_NAME="${PWD##*/}"
BUILD_DIR="build"

echo "Initializing build for $PROJECT_NAME..."

if [ -d "$BUILD_DIR" ]; then
    echo "Build directory '$BUILD_DIR' already exists. Removing it..."
    rm -rf "$BUILD_DIR"
fi

mkdir "$BUILD_DIR"
cd "$BUILD_DIR"
cmake -S .. -B . -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
ninja
cd ..

echo "Build initialization complete!"
