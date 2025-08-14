#!/bin/bash

# Exit on any error
set -e

# Function to get the project root directory
get_project_root() {
    local current_dir="$PWD"
    while [[ ! -d "$current_dir/Tools" && "$current_dir" != "/" ]]; do
        current_dir="$(dirname "$current_dir")"
    done
    
    if [[ ! -d "$current_dir/Tools" ]]; then
        echo "Error: Could not find project root directory containing Tools folder"
        exit 1
    fi
    
    echo "$current_dir"
}

# Determine if we need sudo (only for non-root, non-CI environments)
if [[ -z "$CI" && "$(id -u)" != "0" ]]; then
    if ! command -v sudo &> /dev/null; then
        echo "Error: sudo is not available. Please install sudo first."
        exit 1
    fi
    SUDO="sudo"
else
    SUDO=""
fi

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
    if ! git clone "$repo_url" "$dir_name"; then
        echo "Failed to clone $dir_name"
        exit 1
    fi
}

# Main installation process
main() {
    # Get project root and validate directory structure
    local project_root=$(get_project_root)
    cd "$project_root"

    # Navigate to ThirdParty directory
    if [ ! -d "ThirdParty" ]; then
        mkdir -p ThirdParty
    fi
    cd ThirdParty

    # Install extra-cmake-modules
    echo "Setting up extra-cmake-modules..."
    setup_repo "https://invent.kde.org/frameworks/extra-cmake-modules.git" "extra-cmake-modules"
    cd extra-cmake-modules
    configure_cmake "extra-cmake-modules"
    make
    $SUDO make install
    cd ..

    # Install layer-shell-qt
    echo "Setting up layer-shell-qt..."
    setup_repo "https://invent.kde.org/plasma/layer-shell-qt.git" "layer-shell-qt"
    cd layer-shell-qt
    
    # Set up Qt paths
    local QT_DIR="../../../aqt/6.9.1/gcc_64"
    local QT_PATHS="$QT_DIR/bin:$PATH"
    
    # Verify Qt directory exists
    if [ ! -d "$QT_DIR" ]; then
        echo "Error: Qt directory not found at $QT_DIR"
        echo "Please ensure Qt is installed correctly"
        exit 1
    fi
    
    PATH="$QT_PATHS" configure_cmake "layer-shell-qt" "-DCMAKE_PREFIX_PATH=$QT_DIR"
    make
    $SUDO make install
    cd ..

    echo "Installation completed successfully!"
}

# Run the main function
main