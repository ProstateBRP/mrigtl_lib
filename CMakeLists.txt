cmake_minimum_required(VERSION 3.10)
project(mrigtl_lib VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable position independent code for all library code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Force disable compiler predefines in MOC
set(CMAKE_AUTOMOC_COMPILER_PREDEFINES OFF)

# Re-enable Qt automated tools
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTORCC TRUE)
set(CMAKE_AUTOUIC TRUE)

# Find required packages - CMake 3.29.3 should work fine with Qt6
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if(Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
    message(STATUS "Using Qt6 version ${Qt6_VERSION}")
    # qt_standard_project_setup() requires Qt 6.2+
    if(Qt6_VERSION VERSION_GREATER_EQUAL "6.2.0")
        qt_standard_project_setup()
    endif()
else()
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    set(QT_VERSION_MAJOR 5)
    message(STATUS "Falling back to Qt5")
endif()
find_package(OpenIGTLink REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenIGTLink_INCLUDE_DIRS}
)

# Separate library sources from executable sources
set(LIB_SOURCES
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

# Executable source is just the main file
set(EXE_SOURCES
    src/main.cpp
)

# Add header files
set(HEADERS
    include/common.h
    include/mrigtl_lib_export.h
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

# Create shared library
add_library(${PROJECT_NAME}_shared SHARED ${LIB_SOURCES} ${HEADERS})
set_target_properties(${PROJECT_NAME}_shared PROPERTIES 
    OUTPUT_NAME ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Define export macro for Windows DLL
target_compile_definitions(${PROJECT_NAME}_shared PRIVATE mrigtl_lib_shared_EXPORTS)

# Create static library
add_library(${PROJECT_NAME}_static STATIC ${LIB_SOURCES} ${HEADERS})
set_target_properties(${PROJECT_NAME}_static PROPERTIES 
    OUTPUT_NAME ${PROJECT_NAME}_static
    VERSION ${PROJECT_VERSION}
)

# Link libraries to shared library
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME}_shared
        Qt6::Core
        Qt6::Widgets
        ${OpenIGTLink_LIBRARIES}
    )
else()
    target_link_libraries(${PROJECT_NAME}_shared
        Qt5::Core
        Qt5::Widgets
        ${OpenIGTLink_LIBRARIES}
    )
endif()

# Link libraries to static library
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME}_static
        Qt6::Core
        Qt6::Widgets
        ${OpenIGTLink_LIBRARIES}
    )
else()
    target_link_libraries(${PROJECT_NAME}_static
        Qt5::Core
        Qt5::Widgets
        ${OpenIGTLink_LIBRARIES}
    )
endif()

# Create executable that uses the shared library
add_executable(${PROJECT_NAME}_exe ${EXE_SOURCES})
set_target_properties(${PROJECT_NAME}_exe PROPERTIES 
    OUTPUT_NAME ${PROJECT_NAME}
)

# Link executable with the shared library
target_link_libraries(${PROJECT_NAME}_exe
    ${PROJECT_NAME}_shared
)

# Add alias targets for better CMake integration
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}_shared)
add_library(${PROJECT_NAME}::${PROJECT_NAME}_static ALIAS ${PROJECT_NAME}_static)

# Install targets
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}_shared ${PROJECT_NAME}_static ${PROJECT_NAME}_exe
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install headers
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# Install export targets
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Export targets for build tree
export(EXPORT ${PROJECT_NAME}Targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
)

## Copy the Usemrigtl_lib.cmake file to the binary tree for backward compatability.
#CONFIGURE_FILE(${mrigtl_lib_SOURCE_DIR}/Usemrigtl_lib.cmake.in
#  ${mrigtl_lib_BINARY_DIR}/Usemrigtl_lib.cmake COPYONLY IMMEDIATE)
#
## Create the OpenIGTLinkConfig.cmake file containing the OpenIGTLink configuration.
#INCLUDE(${mrigtl_lib_SOURCE_DIR}/cmake/generate_mrigtl_lib_config.cmake)

## Configure and install CMake package config file
#include(CMakePackageConfigHelpers)
#configure_package_config_file(
#    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
#    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
#    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
#)

#write_basic_package_version_file(
#    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
#    VERSION ${PROJECT_VERSION}
#    COMPATIBILITY SameMajorVersion
#)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Configure and install CMake package config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

