![Screenshot](https://avatars.githubusercontent.com/u/211567544?s=400&u=200775d4f2c2771985ecdb53eeee052edcbfa23a&v=4)

# ElementalRenderer

A simple lightweight rendering library for C++ that provides essential 3D graphics capabilities with minimal overhead.

## Overview

ElementalRenderer is designed to be an easy-to-use yet powerful rendering library for C++ applications. It provides core rendering functionality while maintaining a small footprint and efficient performance.

## Features

- **Scene Management**: Create and manage 3D scenes with an intuitive API
- **Mesh Support**: Load and render 3D mesh data
- **Lighting System**: Various light types with customizable properties
- **Camera Controls**: Flexible camera system for scene navigation
- **Material System**: Define surface properties for realistic rendering
- **Shader Support**: Custom shader implementation and management
- **Texture Handling**: Load and apply textures to 3D objects

## Requirements

- C++17 or later
- CMake 3.10 or later
- GPU with OpenGL support

## Dependencies

ElementalRenderer uses the following third-party libraries:
- OpenGL
- GLFW (for window management)
- GLM (OpenGL Mathematics)
- stb_image (for texture loading)

## Installation

### Using CMake

#### Prerequisites

Ensure you have the following installed:
- Git
- CMake (3.10 or later)
- A C++17 compatible compiler (GCC, Clang, MSVC)

#### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/ElementalRenderer.git
   cd ElementalRenderer
   ```

2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure the project:
   ```bash
   cmake ..
   ```
   
   Optional configuration flags:
   - `-DBUILD_SHARED_LIBS=ON` - Build as shared library instead of static
   - `-DBUILD_TESTS=ON` - Build test suite
   - `-DCMAKE_BUILD_TYPE=Release` - Set build type (Debug, Release, RelWithDebInfo, MinSizeRel)
   - `-DCMAKE_INSTALL_PREFIX=/your/install/path` - Set custom installation path

4. Build the project:
   ```bash
   cmake --build .
   ```
   
   On multi-core systems, you can speed up the build:
   ```bash
   cmake --build . --parallel 8  # Use 8 cores
   ```

5. Run the tests (if enabled):
   ```bash
   ctest
   ```

6. Install the library:
   ```bash
   cmake --install .
   ```

### Using the setup script

For convenience, you can also use the provided setup script:
