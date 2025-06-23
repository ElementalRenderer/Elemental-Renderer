# How to Run ElementalRenderer

This guide will walk you through the process of building and running the ElementalRenderer project on Windows.

## Prerequisites

Before you can build and run ElementalRenderer, you need to install:

1. **CMake** (version 3.14 or later)
2. **C++ Compiler** (Visual Studio 2019/2022 with C++ workload recommended)

Detailed installation instructions for these prerequisites can be found in the [INSTALL_PREREQUISITES.md](INSTALL_PREREQUISITES.md) file.

## Building the Project

Once you have installed the prerequisites, you can build the project using one of the following methods:

### Method 1: Using the Command Line

1. Open a Command Prompt or PowerShell window
2. Navigate to the ElementalRenderer directory
3. Create a build directory:
   ```
   mkdir build
   cd build
   ```
4. Configure the project:
   ```
   cmake ..
   ```
5. Build the project:
   ```
   cmake --build . --config Release
   ```

### Method 2: Using Visual Studio

1. Open Visual Studio
2. Select "Open a local folder" and navigate to the ElementalRenderer directory
3. Visual Studio should automatically detect the CMake configuration
4. Click on "Build" > "Build All" to build the project

## Running the Project

After building the project, you can run it using one of the following methods:

### Method 1: Using the Command Line

1. Navigate to the build directory:
   ```
   cd build\Release
   ```
2. Run the executable:
   ```
   main.exe
   ```

### Method 2: Using Visual Studio

1. In Visual Studio, after building the project, click on "Debug" > "Start Without Debugging" (or press Ctrl+F5)

## Expected Output

When you run the ElementalRenderer demo, you should see:

1. A console window displaying:
   ```
   Elemental Renderer Demo
   Version: 0.1.0
   Rendering a single frame...
   Elemental Renderer Demo completed successfully!
   ```
2. A window showing a 3D scene with a test object

## Troubleshooting

If you encounter issues:

### CMake Not Found

If you see an error like "cmake is not recognized as an internal or external command", it means CMake is not installed or not in your PATH. Follow the instructions in [INSTALL_PREREQUISITES.md](INSTALL_PREREQUISITES.md) to install CMake.

### Build Errors

If you encounter build errors:
1. Make sure you have a C++ compiler installed
2. Check that you have the required dependencies (OpenGL, GLFW, GLM)
3. Try building with the Debug configuration: `cmake --build . --config Debug`

### Runtime Errors

If the program builds but crashes when running:
1. Make sure your graphics drivers are up to date
2. Check that your system supports OpenGL
3. Try running the Debug version of the executable

## Command Line Arguments

The ElementalRenderer demo supports the following command line arguments:

- `--width <value>`: Set the window width (default: 1280)
- `--height <value>`: Set the window height (default: 720)
- `--fullscreen`: Run in fullscreen mode
- `--vsync`: Enable vertical sync
- `--debug`: Enable debug mode

Example:
```
main.exe --width 1920 --height 1080 --fullscreen
```