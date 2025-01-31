#!/bin/bash

# Exit on any error
set -e

# Check if we're in the Tools directory
if [[ $(basename "$PWD") != "Tools" ]]; then
    echo "Error: This script must be run from the Tools directory"
    echo "Current directory: $PWD"
    echo "Please change to the Tools directory and try again"
    exit 1
fi

# Check if we're running in CI
if [ -n "$CI" ]; then
    # In CI environment, we don't need sudo
    SUDO=""
else
    # Check if script was invoked with sudo
    if [ "$(id -u)" = "0" ]; then
        echo "Error: Please do not run this script with sudo!"
        echo "The script will ask for sudo password when needed."
        echo "Run it as a normal user: ./install-dependencies.sh"
        exit 1
    fi
    # For local development, use sudo
    SUDO="sudo"
fi


# Function to check if sudo is available (only for non-CI)
check_sudo_available() {
    if [ -z "$CI" ] && ! command -v sudo &> /dev/null; then
        echo "Error: sudo is not available. Please install sudo first."
        exit 1
    fi
}

# Function to handle cmake configuration with error checking
configure_cmake() {
    local dir=$1
    local extra_args=${2:-}
    
    echo "Configuring $dir..."
    if ! cmake . $extra_args; then
        echo "CMAKE configuration failed for $dir"
        exit 1
    fi
}

# Function to handle repository setup with cleanup
setup_repo() {
    local repo_url=$1
    local dir_name=$2
    
    if [ -d "$dir_name" ]; then
        echo "Removing existing $dir_name directory for clean install..."
        rm -rf "$dir_name"
    fi
    
    echo "Cloning $dir_name..."
    git clone "$repo_url" "$dir_name"
}

# Main installation process
main() {
    # Check if sudo is available for later use
    check_sudo_available

    # Move up to the root directory of the project
    cd ..

    # Navigate to ThirdParty directory
    if [ ! -d "ThirdParty" ]; then
        echo "Error: ThirdParty directory not found at $(pwd)"
        exit 1
    fi
    cd ThirdParty

    # Install extra-cmake-modules
    echo "Setting up extra-cmake-modules..."
    setup_repo "https://invent.kde.org/frameworks/extra-cmake-modules.git" "extra-cmake-modules"
    cd extra-cmake-modules
    configure_cmake "extra-cmake-modules"
    make
    echo "Running make install for extra-cmake-modules (requires sudo)..."
    sudo make install
    cd ..

    # Install layer-shell-qt
    echo "Setting up layer-shell-qt..."
    setup_repo "https://invent.kde.org/plasma/layer-shell-qt.git" "layer-shell-qt"
    cd layer-shell-qt
    
    QT_DIR="../../../aqt/6.8.2/gcc_64"
    QT_PATHS="$QT_DIR/bin:$PATH"
    
    PATH="$QT_PATHS" configure_cmake "layer-shell-qt" "-DCMAKE_PREFIX_PATH=$QT_DIR"
    make
    echo "Running make install for layer-shell-qt (requires sudo)..."
    sudo make install
    cd ..

    echo "Installation completed successfully!"
}

# Run the main function
main