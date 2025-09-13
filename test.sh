#!/bin/bash

# Smart test runner that only recompiles when necessary

BINARY="test-fumo"
SOURCE="src/compiler_test.cpp"

# Function to check if recompilation is needed
needs_rebuild() {
    # If binary doesn't exist, rebuild
    if [ ! -f "$BINARY" ]; then
        return 0
    fi
    
    # If source is newer than binary, rebuild
    if [ "$SOURCE" -nt "$BINARY" ]; then
        return 0
    fi
    
    # Check if any header files in src/ are newer than binary
    if find src/ -name "*.h" -o -name "*.hpp" 2>/dev/null | while read header; do
        if [ "$header" -nt "$BINARY" ]; then
            exit 0  # Found a newer header
        fi
    done | grep -q .; then
        return 0
    fi
    
    # No rebuild needed
    return 1
}

# Check for --force-rebuild flag
force_rebuild=false
args=()
for arg in "$@"; do
    if [ "$arg" = "--force-rebuild" ]; then
        force_rebuild=true
    else
        args+=("$arg")
    fi
done

# Rebuild if necessary or forced
if $force_rebuild || needs_rebuild; then
    echo "Compiling test runner..."
    if ! clang++ -std=c++23 -Wno-ignored-attributes "$SOURCE" -o "$BINARY"; then
        echo "Compilation failed!"
        exit 1
    fi
    echo "Compilation successful."
fi

# Run the test with remaining arguments
exec "./$BINARY" "${args[@]}"
