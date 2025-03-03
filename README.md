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

The project uses CMake for building:

```bash
# Create a build directory
mkdir build
cd build

# Configure (set the path to your OpenIGTLink build)
cmake -DOpenIGTLink_DIR=/path/to/OpenIGTLink-build ..

# Build
make
```

## Running

From the build directory:

```bash
./mrigtlbridge_cpp
```

## Architecture

The application has the following main components:

1. **Signal Manager**: A central hub for inter-component communication
2. **Listeners**: Background threads for OpenIGTLink and MR simulator communication
3. **Widgets**: UI components for each system
4. **Main Window**: The application shell that contains the widgets

## License

See the LICENSE file for details.
