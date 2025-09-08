#!/bin/bash
set -e

PROJECT_NAME="${PWD##*/}"
BUILD_DIR="build"
BINARY="$BUILD_DIR/$PROJECT_NAME"
#!/bin/bash
set -e

PROJECT_NAME="${PWD##*/}"
BUILD_DIR="build"

export ASAN_OPTIONS="detect_leaks=0"

echo "Building $PROJECT_NAME..."

if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: $BUILD_DIR directory not found"
    echo "Run: cmake -B build -S . -GNinja"
    exit 1
fi

cd "$BUILD_DIR"
ninja
cd ..

echo "Build complete!"
