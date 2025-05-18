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
- **Node-based Shader Editor**: Visual shader creation with both graph and code views
- **Texture Handling**: Load and apply textures to 3D objects
- **Customizable Lighting Models**: Easily swap between Oren-Nayar, Cook-Torrance, and custom BRDFs
- **Heuristic Render Graph**: Automatically builds an optimized dependency graph of render passes without manual setup

## Lighting Models

ElementalRenderer provides a flexible system for applying different Bidirectional Reflectance Distribution Functions (BRDFs) to your materials:

- **Phong**: Classic lighting model with ambient, diffuse, and specular components
- **Blinn-Phong**: Improved version using halfway vector for more realistic specular highlights
- **Oren-Nayar**: Physically-based diffuse model that accounts for microfacet roughness, ideal for matte surfaces
- **Cook-Torrance**: Advanced microfacet specular model for realistic metals and other complex materials
- **GGX/Trowbridge-Reitz**: Modern microfacet model with improved specular highlights
- **Physically Based Rendering (PBR)**: Full PBR implementation with energy conservation
- **Custom BRDF**: Easily implement and inject your own reflection models without modifying core code

Lighting models can be swapped at runtime for any material with just a single function call.

## Node-based Shader Editor

ElementalRenderer includes a powerful node-based shader editor that allows you to create and edit shaders visually:

- **Visual Node Graph**: Create shaders by connecting nodes in a visual graph
- **Code View**: See and edit the generated HLSL code in real-time
- **Hybrid Workflow**: Seamlessly switch between visual editing and code editing
- **Node Library**: Extensive library of shader nodes for various operations
- **Custom Nodes**: Create your own custom nodes for specialized effects
- **Live Preview**: See your shader changes in real-time
- **Save/Load**: Save your shader graphs and load them later

The shader editor supports various node types:
- Math operations (Add, Multiply, Dot Product, etc.)
- Vector operations
- Texture sampling
- Input parameters (Position, Normal, UV, etc.)
- Output parameters (Color, Normal, Emission, etc.)

### Using the Shader Editor

To open the shader editor:

```cpp
// Getter for shader
auto& editorManager = ElementalRenderer::ShaderEditorManager::getInstance();

// Initialize the editor
editorManager.initialize(1280, 720);

// Show the editor
editorManager.showEditor();

// Create a shader from the current graph
auto shader = editorManager.createShaderFromGraph();
```

The shader editor provides a powerful way to create complex shaders without writing code, while still allowing advanced users to fine-tune the generated code.

## Heuristic Render Graph

ElementalRenderer includes a Heuristic Render Graph system that automatically builds an optimized dependency graph of render passes:

- **Automatic Dependency Detection**: Automatically determines dependencies between render passes based on resource usage
- **Optimal Execution Order**: Uses topological sorting to determine the optimal execution order
- **Resource Tracking**: Tracks read/write operations on resources to ensure correct ordering
- **Flexible Pass Definition**: Define custom render passes with lambda functions
- **Cycle Detection**: Detects and reports dependency cycles

The render graph system eliminates the need for manual setup of render pass dependencies, making it easier to create complex rendering pipelines:

```cpp
// Create a render graph
auto renderGraph = std::make_shared<ElementalRenderer::RenderGraph>("MyRenderGraph");

// Add render passes with their resource dependencies
auto shadowPass = std::make_shared<ElementalRenderer::RenderPass>("ShadowPass", []() {
    // Shadow mapping code here
});
shadowPass->addWriteResource("ShadowMap");
renderGraph->addPass(shadowPass);

auto geometryPass = std::make_shared<ElementalRenderer::RenderPass>("GeometryPass", []() {
    // Geometry rendering code here
});
geometryPass->addReadResource("ShadowMap");
geometryPass->addWriteResource("GBuffer");
renderGraph->addPass(geometryPass);

// Build and execute the render graph
renderGraph->buildDependencyGraph();
renderGraph->execute();
```

The system automatically determines that the GeometryPass depends on the ShadowPass because it reads from the "ShadowMap" resource that ShadowPass writes to, basically ensuring they execute in the correct order.

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
```bash
setup.sh
```
