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
- A shared library: `libmrigtl_lib.so` (Linux) or `libmrigtl_lib.dylib` (macOS)
- A static library: `libmrigtl_lib_static.a`
- An executable: `mrigtl_sim`

## Running

From the build directory:

```bash
./mrigtl_sim
```

## Using the Library

### Including in Your Project

To use the library in your own CMake project:

```cmake
# Find the package
find_package(mrigtl_lib REQUIRED)

# Link against the library
target_link_libraries(your_target
    mrigtl_lib::mrigtl_lib
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
#include <mrigtl_lib/signal_manager.h>
#include <mrigtl_lib/mr_igtl_bridge_window.h>

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

## Scripts

### log_to_tracking.py

Converts a Siemens ICE tracking log file into a tracking CSV file.

The log contains lines such as:

```
216|2025/10/28-17:28:40.707348|ICE.IceWWTracking.DLL|MrIrisContainer|BG debug, fill header for track coil RX3 and proj X, positions (19.4856, 0, 0)|
```

For each coil channel (e.g. `RX3`), the script combines the `proj X`, `proj Y`, and `proj Z` lines into a single `(X, Y, Z)` sample. It identifies the value in each line by which position slot is nonzero rather than by the `proj` label, so it also handles log variants where the labels are shifted (e.g. `W`/`X`/`Y`/`Z`).

Usage:

```bash
python3 script/log_to_tracking.py <log_file> <csv_file>
```

Example:

```bash
python3 script/log_to_tracking.py logviewer.log tracking.csv
```

The output CSV has the columns: `channel, timestamp, X, Y, Z`.

## License

See the LICENSE file for details.
