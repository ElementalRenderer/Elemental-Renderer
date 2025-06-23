@echo off
setlocal enabledelayedexpansion

:: ElementalRenderer Setup Script for Windows
:: This script automates the build and installation process for ElementalRenderer

:: Default options
set BUILD_TYPE=Release
set BUILD_SHARED=OFF
set BUILD_TESTS=OFF
set BUILD_DOCS=OFF
set INSTALL_PREFIX=
set PARALLEL_JOBS=%NUMBER_OF_PROCESSORS%
set CLEAN_BUILD=false

:: Display help
:show_help
if "%1"=="--help" (
    echo Elemental NEON Setup Script for Windows
    echo Usage: %0 [options]
    echo.
    echo Options:
    echo   --help             Display this help message
    echo   --type TYPE        Build type: Debug, Release, RelWithDebInfo, MinSizeRel (default: Release)
    echo   --shared           Build as shared library
    echo   --tests            Build with tests enabled
    echo   --docs             Build documentation
    echo   --prefix PATH      Installation prefix
    echo   --jobs N           Number of parallel jobs (default: auto-detected)
    echo   --clean            Clean build directory before building
    echo.
    echo Example: %0 --type Debug --tests --jobs 4
    exit /b 0
)

:: Parse command line arguments
:parse_args
if "%1"=="" goto :end_parse_args
if "%1"=="--type" (
    set BUILD_TYPE=%2
    shift
    shift
    goto :parse_args
)
if "%1"=="--shared" (
    set BUILD_SHARED=ON
    shift
    goto :parse_args
)
if "%1"=="--tests" (
    set BUILD_TESTS=ON
    shift
    goto :parse_args
)
if "%1"=="--docs" (
    set BUILD_DOCS=ON
    shift
    goto :parse_args
)
if "%1"=="--prefix" (
    set INSTALL_PREFIX=%2
    shift
    shift
    goto :parse_args
)
if "%1"=="--jobs" (
    set PARALLEL_JOBS=%2
    shift
    shift
    goto :parse_args
)
if "%1"=="--clean" (
    set CLEAN_BUILD=true
    shift
    goto :parse_args
)
echo Unknown option: %1
call :show_help
exit /b 1

:end_parse_args

:: Validate build type
if not "%BUILD_TYPE%"=="Debug" (
    if not "%BUILD_TYPE%"=="Release" (
        if not "%BUILD_TYPE%"=="RelWithDebInfo" (
            if not "%BUILD_TYPE%"=="MinSizeRel" (
                echo Invalid build type: %BUILD_TYPE%
                echo Valid types are: Debug, Release, RelWithDebInfo, MinSizeRel
                exit /b 1
            )
        )
    )
)

:: Check for required tools
echo Checking dependencies...
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: cmake is not installed or not in PATH
    exit /b 1
)
where git >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: git is not installed or not in PATH
    exit /b 1
)
echo All required tools are installed.

:: Prepare build directory
echo Preparing build environment...
set BUILD_DIR=build

if "%CLEAN_BUILD%"=="true" (
    if exist "%BUILD_DIR%" (
        echo Cleaning existing build directory...
        rmdir /s /q "%BUILD_DIR%"
    )
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%" || (
    echo Error: Could not enter build directory
    exit /b 1
)

:: Configure CMake options
set CMAKE_ARGS=-DCMAKE_BUILD_TYPE=%BUILD_TYPE%
set CMAKE_ARGS=%CMAKE_ARGS% -DBUILD_SHARED_LIBS=%BUILD_SHARED%
set CMAKE_ARGS=%CMAKE_ARGS% -DBUILD_TESTS=%BUILD_TESTS%
set CMAKE_ARGS=%CMAKE_ARGS% -DBUILD_DOCUMENTATION=%BUILD_DOCS%

:: Add installation prefix if specified
if not "%INSTALL_PREFIX%"=="" (
    set CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX%
)

:: Configure the project
echo Configuring project with options:
echo   Build type: %BUILD_TYPE%
echo   Shared library: %BUILD_SHARED%
echo   Build tests: %BUILD_TESTS%
echo   Build documentation: %BUILD_DOCS%
if not "%INSTALL_PREFIX%"=="" (
    echo   Install prefix: %INSTALL_PREFIX%
)
echo   Parallel jobs: %PARALLEL_JOBS%

echo Running CMake...
cmake .. %CMAKE_ARGS% || (
    echo CMake configuration failed
    exit /b 1
)

:: Build the project
echo Building project...
cmake --build . --config %BUILD_TYPE% --parallel %PARALLEL_JOBS% || (
    echo Build failed
    exit /b 1
)

:: Run tests if enabled
if "%BUILD_TESTS%"=="ON" (
    echo Running tests...
    ctest --output-on-failure -C %BUILD_TYPE% || (
        echo Tests failed
        exit /b 1
    )
)

:: Build documentation if enabled
if "%BUILD_DOCS%"=="ON" (
    echo Building documentation...
    cmake --build . --target docs || (
        echo Documentation generation failed
    )
)

:: Installation
echo Build completed successfully!
echo.
set /p install_choice=Do you want to install ElementalRenderer? [y/N] 

if /i "%install_choice%"=="y" (
    echo Installing...
    cmake --install . --config %BUILD_TYPE% || (
        echo Installation failed
        exit /b 1
    )
    echo Installation completed!
) else (
    echo Skipping installation.
)

:: Return to the original directory
cd ..

echo.
echo ElementalRenderer setup completed successfully!
echo You can find the built library in the build directory.
if "%BUILD_DOCS%"=="ON" (
    echo Documentation is available in the build\docs directory.
)
echo.
echo To use ElementalRenderer in your projects, add the following to your CMakeLists.txt:
echo find_package(ElementalRenderer REQUIRED)
echo target_link_libraries(YourTarget PRIVATE ElementalRenderer)
echo.

endlocal