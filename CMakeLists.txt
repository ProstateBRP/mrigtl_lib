cmake_minimum_required(VERSION 3.10)
project(mrigtlbridge_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force disable compiler predefines in MOC
set(CMAKE_AUTOMOC_COMPILER_PREDEFINES OFF)

# Configure Qt automated tools
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTORCC TRUE)
set(CMAKE_AUTOUIC TRUE)

# Find required packages
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenIGTLink REQUIRED)

# No longer using Eigen3

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenIGTLink_INCLUDE_DIRS}
)

# Add source files
set(SOURCES
    src/main.cpp
    src/common.cpp
    src/signal_manager.cpp
    src/listener_base.cpp
    src/igtl_listener.cpp
    src/widget_base.cpp
    src/igtl_widget.cpp
    src/mrsim_listener.cpp
    src/mrsim_widget.cpp
    src/mr_igtl_bridge_window.cpp
)

# Add header files
set(HEADERS
    include/common.h
    include/signal_manager.h
    include/signal_wrap.h
    include/listener_base.h
    include/igtl_listener.h
    include/widget_base.h
    include/igtl_widget.h
    include/mrsim_listener.h
    include/mrsim_widget.h
    include/mr_igtl_bridge_window.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Widgets
    ${OpenIGTLink_LIBRARIES}
)