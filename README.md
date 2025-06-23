![Screenshot](https://avatars.githubusercontent.com/u/211567544?s=400&u=200775d4f2c2771985ecdb53eeee052edcbfa23a&v=4)

# ElementalRenderer

A simple lightweight rendering library for C++ that provides essential 3D graphics capabilities with minimal overhead.

## Overview
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
- **ImGui Integration**: Clean, immediate-mode interface for tweaking parameters and visualizing data
- **GPU Profiling**: Real-time performance metrics and visualization tools
- **Custom Node SDK**: Create your own shader nodes using C++ or JSON definitions

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

The shader editor provides a powerful way to create complex shaders without writing code, while still allowing advanced users to fine-tune the generated code.

## Custom Node Authoring SDK

ElementalRenderer now includes a powerful SDK for creating custom shader nodes:

- **JSON-based Nodes**: Create simple nodes using JSON definitions
- **C++ Nodes**: Implement complex nodes with full access to the shader system
- **Hot Reloading**: JSON nodes can be modified and reloaded at runtime
- **Comprehensive API**: Well-documented interface for node creation
- **Example Nodes**: Sample implementations to get you started

The Custom Node SDK makes it easy to extend the shader editor with your own specialized functionality.

## ImGui Integration

ElementalRenderer now features a comprehensive ImGui integration for creating user interfaces:

- **Material Editor**: Tweak material properties with sliders and color pickers
- **Scene Hierarchy**: Navigate and modify scene objects
- **Performance Metrics**: Real-time display of frame times and GPU usage
- **Render Graph Visualization**: Interactive view of the render pipeline
- **Shader Graph Editor**: Visual editing of shader graphs
- **Dockable Windows**: Flexible UI layout with dockable panels

The ImGui integration provides a modern, immediate-mode interface for interacting with all aspects of the renderer.

## GPU Profiling and Debug Tools

ElementalRenderer includes comprehensive profiling and debugging tools:

- **Frame Timing**: Detailed breakdown of CPU and GPU time per frame
- **Render Pass Profiling**: Performance metrics for each render pass
- **GPU Memory Usage**: Track memory allocation and usage
- **Render Graph Visualization**: Visual representation of the render pipeline
- **Resource Tracking**: Monitor resource creation, usage, and destruction

These tools help identify performance bottlenecks and optimize rendering code.

## Heuristic Render Graph

ElementalRenderer includes a Heuristic Render Graph system that automatically builds an optimized dependency graph of render passes:

- **Automatic Dependency Detection**: Automatically determines dependencies between render passes based on resource usage
- **Optimal Execution Order**: Uses topological sorting to determine the optimal execution order
- **Resource Tracking**: Tracks read/write operations on resources to ensure correct ordering
- **Flexible Pass Definition**: Define custom render passes with lambda functions
- **Cycle Detection**: Detects and reports dependency cycles

The render graph system eliminates the need for manual setup of render pass dependencies, making it easier to create complex rendering pipelines.

The system automatically determines that the GeometryPass depends on the ShadowPass because it reads from the "ShadowMap" resource that ShadowPass writes to, ensuring they execute in the correct order.

## Demo Scenes

ElementalRenderer comes with several demo scenes showcasing different features:

- **Shadow Mapping**: Demonstrates shadow techniques
- **PBR**: Shows physically-based rendering with various materials
- **Post-Processing**: Illustrates post-processing effects
- **Custom BRDF**: Examples of custom lighting models
- **Node Shader**: Demonstrates the node-based shader system

These demos provide practical examples of how to use the various features of ElementalRenderer.

## Documentation

Comprehensive documentation is available:

- **Getting Started Guide**: Quick introduction to using ElementalRenderer
- **API Reference**: Detailed documentation of all classes and functions
- **Tutorials**: Step-by-step guides for common tasks
- **Examples**: Code samples demonstrating specific features

## Requirements

- C++17 or later
- CMake 3.14 or later
- GPU with OpenGL support

## Dependencies

ElementalRenderer uses the following third-party libraries:
- OpenGL
- GLFW (for window management)
- GLM (OpenGL Mathematics)
- ImGui (for user interface)
- stb_image (for texture loading)

## Getting Started

See the [Getting Started Guide](docs/GettingStarted.md) for instructions on how to set up and use ElementalRenderer in your project.

# CHANGELOG.md

## Version 0.2.0 (Current)

### Added
- **ImGui Integration**
    - Added comprehensive ImGui-based user interface
    - Material editor with real-time parameter tweaking
    - Scene hierarchy browser
    - Render graph visualization
    - Shader graph editor integration

- **GPU Profiling and Debug Tools**
    - Frame timing metrics
    - Render pass profiling
    - GPU memory usage tracking
    - Resource visualization
    - Performance overlay

- **Custom Node Authoring SDK**
    - JSON-based node definition system
    - C++ API for complex node implementation
    - Hot-reloading support for rapid iteration
    - Comprehensive documentation and examples

- **Documentation**
    - Added comprehensive getting started guide
    - Created API reference documentation
    - Added tutorials for common tasks
    - Included code samples for all major features

- **Demo Scenes**
    - Shadow mapping demonstration
    - PBR material showcase
    - Post-processing effects examples
    - Custom BRDF implementation samples
    - Node-based shader examples

### Improved
- Enhanced shader editor with better node organization
- Optimized render graph performance
- Improved PBR lighting model accuracy
- Better texture handling and memory management
- More intuitive camera controls
- Expanded material system capabilities

### Fixed
- Corrected shadow mapping artifacts
- Fixed memory leaks in resource management
- Resolved threading issues in render pipeline
- Addressed shader compilation errors on some platforms
- Fixed JSON parsing in configuration files
