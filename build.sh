#\!/bin/bash

# Simple build script without using CMake

# Create build directory
mkdir -p build
cd build

# Create moc output directory
mkdir -p moc

# Find Qt5 moc binary
MOC_BIN=$(find /usr -name moc -type f -path "*/qt5*" | head -1)
if [ -z "$MOC_BIN" ]; then
    echo "Qt5 moc not found. Please install Qt5."
    exit 1
fi

# Find OpenIGTLink include and lib directories
OPENIGTLINK_INCLUDE=/usr/local/include/OpenIGTLink
OPENIGTLINK_LIB=/usr/local/lib

# Compiler flags
CXX_FLAGS="-std=c++17 -I../include -I${OPENIGTLINK_INCLUDE} -fPIC -Wall -Wextra"
LD_FLAGS="-L${OPENIGTLINK_LIB} -lOpenIGTLink -framework QtCore -framework QtWidgets"

# Run moc on all header files
for header in ../include/*.h; do
    base=$(basename "$header" .h)
    echo "Running moc on $header..."
    "$MOC_BIN" -o "moc/moc_${base}.cpp" "$header"
done

# Compile all source files
echo "Compiling source files..."
for src in ../src/*.cpp moc/moc_*.cpp; do
    base=$(basename "$src" .cpp)
    echo "  $src -> $base.o"
    g++ -c "$src" -o "$base.o" ${CXX_FLAGS}
done

# Link everything together
echo "Linking..."
g++ -o mrigtlbridge_cpp *.o ${LD_FLAGS}

echo "Build complete\!"
