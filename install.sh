#!/bin/bash

# install.sh - Install fumo compiler system-wide

set -e  # Exit on any error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COMPILER_NAME="fumo"
BUILD_DIR="$SCRIPT_DIR/build"
BINARY_PATH="$BUILD_DIR/$COMPILER_NAME"

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if running as root
check_root() {
    if [[ $EUID -eq 0 ]]; then
        print_error "This script should not be run as root!"
        print_error "Run as regular user - it will ask for sudo when needed."
        exit 1
    fi
}

# Function to build the compiler if needed
build_compiler() {
    print_status "Checking if compiler binary exists..."
    
    if [[ ! -f "$BINARY_PATH" ]]; then
        print_warning "Compiler binary not found. Building..."
        
        if [[ -f "$SCRIPT_DIR/rebuild.sh" ]]; then
            cd "$SCRIPT_DIR"
            bash rebuild.sh
        elif [[ -f "$SCRIPT_DIR/Makefile" ]]; then
            cd "$SCRIPT_DIR"
            make
        elif [[ -f "$SCRIPT_DIR/CMakeLists.txt" ]]; then
            mkdir -p "$BUILD_DIR"
            cd "$BUILD_DIR"
            cmake ..
            make
        else
            print_error "No build system found (rebuild.sh, Makefile, or CMakeLists.txt)"
            exit 1
        fi
        
        if [[ ! -f "$BINARY_PATH" ]]; then
            print_error "Build failed - binary not created"
            exit 1
        fi
    fi
    
    print_status "Compiler binary found at: $BINARY_PATH"
}

# Function to install to /usr/local/bin (preferred)
install_system_wide() {
    local target_dir="/usr/local/bin"
    local target_path="$target_dir/$COMPILER_NAME"
    
    print_status "Installing $COMPILER_NAME to $target_dir..."
    
    # Check if /usr/local/bin exists and is in PATH
    if [[ ! -d "$target_dir" ]]; then
        print_error "$target_dir does not exist"
        return 1
    fi
    
    # Copy the binary
    sudo cp "$BINARY_PATH" "$target_path"
    sudo chmod +x "$target_path"
    
    print_status "Successfully installed to $target_path"
    
    # Verify installation
    if command -v "$COMPILER_NAME" >/dev/null 2>&1; then
        print_status "✓ '$COMPILER_NAME' command is now available globally"
        print_status "Version check:"
        "$COMPILER_NAME" --version 2>/dev/null || "$COMPILER_NAME" --help | head -1 || echo "  (no version info available)"
    else
        print_warning "$target_dir might not be in your PATH"
        print_warning "You may need to restart your terminal or run: export PATH=\"$target_dir:\$PATH\""
    fi
    
    return 0
}

# Function to install to ~/.local/bin (user-local)
install_user_local() {
    local target_dir="$HOME/.local/bin"
    local target_path="$target_dir/$COMPILER_NAME"
    
    print_status "Installing $COMPILER_NAME to $target_dir (user-local)..."
    
    # Create directory if it doesn't exist
    mkdir -p "$target_dir"
    
    # Copy the binary
    cp "$BINARY_PATH" "$target_path"
    chmod +x "$target_path"
    
    print_status "Successfully installed to $target_path"
    
    # Check if ~/.local/bin is in PATH
    if [[ ":$PATH:" == *":$target_dir:"* ]]; then
        print_status "✓ '$COMPILER_NAME' command is now available globally"
    else
        print_warning "$target_dir is not in your PATH"
        print_warning "Add this line to your ~/.bashrc or ~/.zshrc:"
        echo "export PATH=\"\$HOME/.local/bin:\$PATH\""
        print_warning "Then restart your terminal or run: source ~/.bashrc"
    fi
}

# Function to create a symbolic link (development mode)
install_symlink() {
    local target_dir="/usr/local/bin"
    local target_path="$target_dir/$COMPILER_NAME"
    
    print_status "Creating symbolic link in $target_dir (development mode)..."
    
    # Remove existing link/binary if it exists
    if [[ -L "$target_path" ]]; then
        sudo rm "$target_path"
        print_status "Removed existing symbolic link"
    elif [[ -f "$target_path" ]]; then
        print_warning "Existing binary found at $target_path"
        read -p "Replace it with symbolic link? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_status "Installation cancelled"
            return 1
        fi
        sudo rm "$target_path"
    fi
    
    # Create symbolic link
    sudo ln -s "$BINARY_PATH" "$target_path"
    
    print_status "✓ Symbolic link created: $target_path -> $BINARY_PATH"
    print_status "✓ '$COMPILER_NAME' command is now available globally"
    print_warning "Note: This links to your development build. Rebuild to update the installed version."
}

# Function to uninstall
uninstall() {
    local locations=("/usr/local/bin/$COMPILER_NAME" "$HOME/.local/bin/$COMPILER_NAME")
    local found=false
    
    print_status "Uninstalling $COMPILER_NAME..."
    
    for location in "${locations[@]}"; do
        if [[ -f "$location" ]] || [[ -L "$location" ]]; then
            found=true
            if [[ "$location" == "/usr/local/bin/$COMPILER_NAME" ]]; then
                sudo rm "$location"
            else
                rm "$location"
            fi
            print_status "Removed: $location"
        fi
    done
    
    if [[ "$found" == false ]]; then
        print_warning "No installation found"
    else
        print_status "✓ Uninstall complete"
    fi
}

# Main installation menu
main() {
    check_root
    
    echo "Fumo Compiler Installation Script"
    echo "================================="
    
    if [[ "$1" == "--uninstall" ]]; then
        uninstall
        return 0
    fi
    
    build_compiler
    
    echo ""
    echo "Choose installation method:"
    echo "1) System-wide (/usr/local/bin) - requires sudo"
    echo "2) User-local (~/.local/bin) - no sudo required"
    echo "3) Development symlink (/usr/local/bin -> build/) - requires sudo"
    echo "4) Uninstall"
    echo "5) Cancel"
    echo ""
    
    read -p "Enter choice (1-5): " choice
    
    case $choice in
        1)
            install_system_wide
            ;;
        2)
            install_user_local
            ;;
        3)
            install_symlink
            ;;
        4)
            uninstall
            ;;
        5)
            print_status "Installation cancelled"
            ;;
        *)
            print_error "Invalid choice"
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
