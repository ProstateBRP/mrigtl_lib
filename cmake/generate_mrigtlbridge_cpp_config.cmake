# Generate the mrigtlbridge_cppConfig.cmake file in the build tree.  Also configure
# one for installation.  The file tells external projects how to use
# mrigtlbridge_cpp.

#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# Generate CMake lines that will define the mrigtlbridge_cpp_SOURCE_DIR in the mrigtlbridge_cppConfig.cmake.
# We want this to happen only in the mrigtlbridge_cppConfig.cmake of the build dir, not in the
# installed or relocatable one.
SET(mrigtlbridge_cpp_CONFIG_CODE "
# The mrigtlbridge_cpp source tree.
# For backward compatibility issues we still need to define this variable, although
# it is highly probable that it will cause more harm than being useful. 
# Use mrigtlbridge_cpp_INCLUDE_DIRS instead, since mrigtlbridge_cpp_SOURCE_DIR may point to non-existent directory
IF(NOT mrigtlbridge_cpp_LEGACY_REMOVE)
  SET(mrigtlbridge_cpp_SOURCE_DIR \"${mrigtlbridge_cpp_SOURCE_DIR}\")
ENDIF(NOT mrigtlbridge_cpp_LEGACY_REMOVE)"
)

# The "use" file.
SET(mrigtlbridge_cpp_USE_FILE ${mrigtlbridge_cpp_BINARY_DIR}/Usemrigtlbridge_cpp.cmake)

# The build settings file.
SET(mrigtlbridge_cpp_BUILD_SETTINGS_FILE ${mrigtlbridge_cpp_BINARY_DIR}/mrigtlbridge_cppBuildSettings.cmake)

# Library directory.
SET(mrigtlbridge_cpp_LIBRARY_DIRS_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Determine the include directories needed.
SET(mrigtlbridge_cpp_INCLUDE_DIRS_CONFIG
  ${mrigtlbridge_cpp_INCLUDE_DIRS}
)

#-----------------------------------------------------------------------------
# Configure mrigtlbridge_cppConfig.cmake for the build tree.
CONFIGURE_FILE(${mrigtlbridge_cpp_SOURCE_DIR}/mrigtlbridge_cppConfig.cmake.in
               ${mrigtlbridge_cpp_BINARY_DIR}/mrigtlbridge_cppConfig.cmake @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Settings specific to the install tree.

# store old mrigtlbridge_cpp_LIBRARY_TARGETS_FILE
SET(mrigtlbridge_cpp_LIBRARY_TARGETS_FILE_BUILDTREE ${mrigtlbridge_cpp_LIBRARY_TARGETS_FILE})

# The library dependencies file.
SET(mrigtlbridge_cpp_LIBRARY_TARGETS_FILE "\${mrigtlbridge_cpp_INSTALL_PREFIX}/${mrigtlbridge_cpp_INSTALL_PACKAGE_DIR}/mrigtlbridge_cppTargets.cmake")

# The "use" file.
SET(mrigtlbridge_cpp_USE_FILE \${mrigtlbridge_cpp_INSTALL_PREFIX}/${mrigtlbridge_cpp_INSTALL_PACKAGE_DIR}/Usemrigtlbridge_cpp.cmake)

# The build settings file.
SET(mrigtlbridge_cpp_BUILD_SETTINGS_FILE \${mrigtlbridge_cpp_INSTALL_PREFIX}/${mrigtlbridge_cpp_INSTALL_PACKAGE_DIR}/mrigtlbridge_cppBuildSettings.cmake)

# Include directories.
SET(mrigtlbridge_cpp_INCLUDE_DIRS_CONFIG \${mrigtlbridge_cpp_INSTALL_PREFIX}/${mrigtlbridge_cpp_INSTALL_INCLUDE_DIR})
FOREACH(DIR ${mrigtlbridge_cpp_INCLUDE_RELATIVE_DIRS})
  LIST(APPEND mrigtlbridge_cpp_INCLUDE_DIRS_CONFIG \${mrigtlbridge_cpp_INSTALL_PREFIX}/${mrigtlbridge_cpp_INSTALL_INCLUDE_DIR}/${DIR})
ENDFOREACH(DIR)
IF(mrigtlbridge_cpp_INCLUDE_DIRS_SYSTEM)
  LIST(APPEND mrigtlbridge_cpp_INCLUDE_DIRS_CONFIG ${mrigtlbridge_cpp_INCLUDE_DIRS_SYSTEM})
ENDIF()

# Link directories.
SET(mrigtlbridge_cpp_LIBRARY_DIRS_CONFIG "\${mrigtlbridge_cpp_INSTALL_PREFIX}/${mrigtlbridge_cpp_INSTALL_LIB_DIR}")

#-----------------------------------------------------------------------------
# Configure mrigtlbridge_cppConfig.cmake for the install tree.

# Construct the proper number of GET_FILENAME_COMPONENT(... PATH)
# calls to compute the installation prefix.
STRING(REGEX REPLACE "/" ";" mrigtlbridge_cpp_INSTALL_PACKAGE_DIR_COUNT
  "${mrigtlbridge_cpp_INSTALL_PACKAGE_DIR}")
SET(mrigtlbridge_cpp_CONFIG_CODE "
# Compute the installation prefix from this mrigtlbridge_cppConfig.cmake file location.
GET_FILENAME_COMPONENT(mrigtlbridge_cpp_INSTALL_PREFIX \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)")
FOREACH(p ${mrigtlbridge_cpp_INSTALL_PACKAGE_DIR_COUNT})
  SET(mrigtlbridge_cpp_CONFIG_CODE
    "${mrigtlbridge_cpp_CONFIG_CODE}\nGET_FILENAME_COMPONENT(mrigtlbridge_cpp_INSTALL_PREFIX \"\${mrigtlbridge_cpp_INSTALL_PREFIX}\" PATH)"
    )
ENDFOREACH(p)


CONFIGURE_FILE(${mrigtlbridge_cpp_SOURCE_DIR}/mrigtlbridge_cppConfig.cmake.in
               ${mrigtlbridge_cpp_BINARY_DIR}/Utilities/mrigtlbridge_cppConfig.cmake @ONLY IMMEDIATE)

# restore old mrigtlbridge_cpp_LIBRARY_TARGETS_FILE
SET(mrigtlbridge_cpp_LIBRARY_TARGETS_FILE ${mrigtlbridge_cpp_LIBRARY_TARGETS_FILE_BUILDTREE})
