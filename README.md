# MR-OpenIGTLink Bridge (C++ Implementation)

A C++ implementation of the MR-OpenIGTLink Bridge originally written in Python.

## Overview

This application provides a bridge between MRI scanners/simulators and OpenIGTLink-compatible surgical navigation systems. It enables:

1. Communication via OpenIGTLink protocol
2. Scan plane control
3. MR image streaming
4. Tracking data integration

## Dependencies

- Qt5 (Core, Widgets)
- OpenIGTLink
- CMake 3.10+
- C++17 compatible compiler

## Building

The project uses CMake for building both libraries and executable:

```bash
# Create a build directory
mkdir build
cd build

# Configure (set the path to your OpenIGTLink build)
cmake -DOpenIGTLink_DIR=/path/to/OpenIGTLink-build ..

# Build
make
```

This will generate:
- A shared library: `libmrigtlbridge_cpp.so` (Linux) or `libmrigtlbridge_cpp.dylib` (macOS)
- A static library: `libmrigtlbridge_cpp_static.a`
- An executable: `mrigtlbridge_cpp`

## Running

From the build directory:

```bash
./mrigtlbridge_cpp
```

## Using the Library

### Including in Your Project

To use the library in your own CMake project:

```cmake
# Find the package
find_package(mrigtlbridge_cpp REQUIRED)

# Link against the library
target_link_libraries(your_target
    mrigtlbridge_cpp::mrigtlbridge_cpp
)
```

### Installation

Install the library system-wide:

```bash
cd build
make install
```

This will install:
- The libraries to the system library directory
- The headers to the system include directory
- The executable to the system binary directory

### Examples

Basic usage:

```cpp
#include <mrigtlbridge_cpp/signal_manager.h>
#include <mrigtlbridge_cpp/mr_igtl_bridge_window.h>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    
    // Create signal manager
    auto signalManager = std::make_shared<mrigtlbridge::SignalManager>();
    
    // Create main window
    mrigtlbridge::MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
```

## Architecture

The application has the following main components:

1. **Signal Manager**: A central hub for inter-component communication
2. **Listeners**: Background threads for OpenIGTLink and MR simulator communication
3. **Widgets**: UI components for each system
4. **Main Window**: The application shell that contains the widgets

## License

See the LICENSE file for details.
