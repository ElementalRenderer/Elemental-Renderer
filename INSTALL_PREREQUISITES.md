# Installing CMake and C++ Compiler on Windows

This guide will walk you through the process of installing CMake and a C++ compiler on Windows, which are prerequisites for building the ElementalRenderer project.

## Installing CMake

CMake is a cross-platform build system generator that is used to control the software compilation process.

### Option 1: Using the Installer (Recommended)

1. Visit the official CMake download page: https://cmake.org/download/
2. Under "Binary distributions", download the Windows installer (e.g., `cmake-3.26.4-windows-x86_64.msi`)
3. Run the installer and follow these steps:
   - Accept the license agreement
   - Choose the installation option "Add CMake to the system PATH for all users" or "Add CMake to the system PATH for current user"
   - Complete the installation

### Option 2: Using the ZIP Archive

1. Visit the official CMake download page: https://cmake.org/download/
2. Under "Binary distributions", download the Windows ZIP file (e.g., `cmake-3.26.4-windows-x86_64.zip`)
3. Extract the ZIP file to a location of your choice (e.g., `C:\Program Files\CMake`)
4. Add the `bin` directory to your system PATH:
   - Right-click on "This PC" or "My Computer" and select "Properties"
   - Click on "Advanced system settings"
   - Click on "Environment Variables"
   - Under "System variables" (or "User variables" if you prefer), find the "Path" variable and click "Edit"
   - Click "New" and add the path to the `bin` directory (e.g., `C:\Program Files\CMake\bin`)
   - Click "OK" on all dialogs to save the changes

### Verifying CMake Installation

To verify that CMake is correctly installed:

1. Open a new Command Prompt or PowerShell window
2. Type `cmake --version` and press Enter
3. You should see the CMake version information displayed

## Installing a C++ Compiler

### Option 1: Visual Studio (Recommended)

Visual Studio is Microsoft's integrated development environment (IDE) that includes the MSVC (Microsoft Visual C++) compiler.

1. Visit the Visual Studio download page: https://visualstudio.microsoft.com/downloads/
2. Download the Visual Studio Community edition (free for individual developers)
3. Run the installer and follow these steps:
   - When prompted for workloads, select "Desktop development with C++"
   - You can customize the installation to include specific components, but the default selection should be sufficient
   - Complete the installation

### Option 2: MinGW-w64 (Alternative)

MinGW-w64 provides a Windows port of the GCC compiler.

1. Visit the MinGW-w64 download page: https://www.mingw-w64.org/downloads/
2. Download the installer (e.g., `mingw-w64-install.exe`)
3. Run the installer and follow these steps:
   - Choose your architecture (x86_64 for 64-bit, i686 for 32-bit)
   - Choose your threads (posix is recommended)
   - Choose your exception handling (seh for 64-bit, dwarf for 32-bit)
   - Choose your build revision (latest available)
   - Choose your installation directory (e.g., `C:\mingw-w64`)
4. Add the `bin` directory to your system PATH:
   - Right-click on "This PC" or "My Computer" and select "Properties"
   - Click on "Advanced system settings"
   - Click on "Environment Variables"
   - Under "System variables" (or "User variables" if you prefer), find the "Path" variable and click "Edit"
   - Click "New" and add the path to the `bin` directory (e.g., `C:\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin`)
   - Click "OK" on all dialogs to save the changes

### Verifying C++ Compiler Installation

To verify that the C++ compiler is correctly installed:

#### For Visual Studio (MSVC):
1. Open a "Developer Command Prompt for VS" (search for it in the Start menu)
2. Type `cl` and press Enter
3. You should see information about the Microsoft C/C++ Compiler

#### For MinGW-w64:
1. Open a Command Prompt or PowerShell window
2. Type `g++ --version` and press Enter
3. You should see the GCC version information displayed

## Next Steps

After installing CMake and a C++ compiler, you can proceed to build the ElementalRenderer project:

1. Open a Command Prompt or PowerShell window
2. Navigate to the ElementalRenderer directory
3. Run the setup script:
   ```
   setup.bat
   ```

Or follow the manual build process:

1. Create a build directory:
   ```
   mkdir build
   cd build
   ```

2. Configure the project:
   ```
   cmake ..
   ```

3. Build the project:
   ```
   cmake --build . --config Release
   ```

For more detailed build instructions, refer to the README.md file in the ElementalRenderer repository.