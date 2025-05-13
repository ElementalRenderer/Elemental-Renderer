#!/bin/bash

# ElementalRenderer Setup Script
# This script automates the build and installation process for ElementalRenderer

# Default options
BUILD_TYPE="Release"
BUILD_SHARED=OFF
BUILD_TESTS=OFF
BUILD_DOCS=OFF
INSTALL_PREFIX=""
PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)  # Auto-detect number of CPU cores
CLEAN_BUILD=false

# Color codes for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Display help
show_help() {
    echo -e "${BLUE}ElementalRenderer Setup Script${NC}"
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -h, --help             Display this help message"
    echo "  -t, --type TYPE        Build type: Debug, Release, RelWithDebInfo, MinSizeRel (default: Release)"
    echo "  -s, --shared           Build as shared library"
    echo "  --tests                Build with tests enabled"
    echo "  --docs                 Build documentation"
    echo "  -p, --prefix PATH      Installation prefix"
    echo "  -j, --jobs N           Number of parallel jobs (default: auto-detected)"
    echo "  --clean                Clean build directory before building"
    echo ""
    echo "Example: $0 --type Debug --tests --jobs 4"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -s|--shared)
            BUILD_SHARED=ON
            shift
            ;;
        --tests)
            BUILD_TESTS=ON
            shift
            ;;
        --docs)
            BUILD_DOCS=ON
            shift
            ;;
        -p|--prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            exit 1
            ;;
    esac
done

# Validate build type
case $BUILD_TYPE in
    Debug|Release|RelWithDebInfo|MinSizeRel)
        ;;
    *)
        echo -e "${RED}Invalid build type: $BUILD_TYPE${NC}"
        echo "Valid types are: Debug, Release, RelWithDebInfo, MinSizeRel"
        exit 1
        ;;
esac

# Check for required tools
echo -e "${BLUE}Checking dependencies...${NC}"
for cmd in cmake git; do
    if ! command -v $cmd &> /dev/null; then
        echo -e "${RED}Error: $cmd is not installed or not in PATH${NC}"
        exit 1
    fi
done
echo -e "${GREEN}All required tools are installed.${NC}"

# Prepare build directory
echo -e "${BLUE}Preparing build environment...${NC}"
BUILD_DIR="build"

if [ "$CLEAN_BUILD" = true ] && [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning existing build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || { echo -e "${RED}Error: Could not enter build directory${NC}"; exit 1; }

# Configure CMake options
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
CMAKE_ARGS+=" -DBUILD_SHARED_LIBS=$BUILD_SHARED"
CMAKE_ARGS+=" -DBUILD_TESTS=$BUILD_TESTS"
CMAKE_ARGS+=" -DBUILD_DOCUMENTATION=$BUILD_DOCS"

# Add installation prefix if specified
if [ -n "$INSTALL_PREFIX" ]; then
    CMAKE_ARGS+=" -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
fi

# Configure the project
echo -e "${BLUE}Configuring project with options:${NC}"
echo -e "  Build type: ${GREEN}$BUILD_TYPE${NC}"
echo -e "  Shared library: ${GREEN}$BUILD_SHARED${NC}"
echo -e "  Build tests: ${GREEN}$BUILD_TESTS${NC}"
echo -e "  Build documentation: ${GREEN}$BUILD_DOCS${NC}"
if [ -n "$INSTALL_PREFIX" ]; then
    echo -e "  Install prefix: ${GREEN}$INSTALL_PREFIX${NC}"
fi
echo -e "  Parallel jobs: ${GREEN}$PARALLEL_JOBS${NC}"

echo -e "${BLUE}Running CMake...${NC}"
cmake .. $CMAKE_ARGS || { echo -e "${RED}CMake configuration failed${NC}"; exit 1; }

# Build the project
echo -e "${BLUE}Building project...${NC}"
cmake --build . --parallel "$PARALLEL_JOBS" || { echo -e "${RED}Build failed${NC}"; exit 1; }

# Run tests if enabled
if [ "$BUILD_TESTS" = "ON" ]; then
    echo -e "${BLUE}Running tests...${NC}"
    ctest --output-on-failure || { echo -e "${RED}Tests failed${NC}"; exit 1; }
fi

# Build documentation if enabled
if [ "$BUILD_DOCS" = "ON" ]; then
    echo -e "${BLUE}Building documentation...${NC}"
    cmake --build . --target docs || { echo -e "${YELLOW}Documentation generation failed${NC}"; }
fi

# Installation
echo -e "${GREEN}Build completed successfully!${NC}"
echo ""
echo -e "${BLUE}Do you want to install ElementalRenderer?${NC} [y/N] "
read -r install_choice

if [[ $install_choice =~ ^[Yy]$ ]]; then
    echo -e "${BLUE}Installing...${NC}"
    cmake --install . || { echo -e "${RED}Installation failed${NC}"; exit 1; }
    echo -e "${GREEN}Installation completed!${NC}"
else
    echo -e "${YELLOW}Skipping installation.${NC}"
fi

# Return to the original directory
cd ..

echo ""
echo -e "${GREEN}ElementalRenderer setup completed successfully!${NC}"
echo -e "You can find the built library in the ${BLUE}build${NC} directory."
if [ "$BUILD_DOCS" = "ON" ]; then
    echo -e "Documentation is available in the ${BLUE}build/docs${NC} directory."
fi
echo ""
echo -e "To use ElementalRenderer in your projects, add the following to your CMakeLists.txt:"
echo -e "${BLUE}find_package(ElementalRenderer REQUIRED)${NC}"
echo -e "${BLUE}target_link_libraries(YourTarget PRIVATE ElementalRenderer)${NC}"
echo ""
# Revert to first commit, add and commit everything as single commit.
git reset $(git commit-tree HEAD^{tree} -m "Elemental added")

name=$(git config user.name)
email=$(git config user.email)

# If this script is called with an argument, use that as the amended author
# EX: ./setup.sh "Author Name <author_email@email.com>"
if [[ "$1" ]]; then
	git commit --amend --author="$1"
else
	git commit --amend --author="$name <$email>"
fi

# Remove the remote (you probably want your own instead).
git remote remove origin
