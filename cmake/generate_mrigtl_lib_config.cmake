# Generate the mrigtl_libConfig.cmake file in the build tree.  Also configure
# one for installation.  The file tells external projects how to use
# mrigtl_lib.

#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# Generate CMake lines that will define the mrigtl_lib_SOURCE_DIR in the mrigtl_libConfig.cmake.
# We want this to happen only in the mrigtl_libConfig.cmake of the build dir, not in the
# installed or relocatable one.
SET(mrigtl_lib_CONFIG_CODE "
# The mrigtl_lib source tree.
# For backward compatibility issues we still need to define this variable, although
# it is highly probable that it will cause more harm than being useful. 
# Use mrigtl_lib_INCLUDE_DIRS instead, since mrigtl_lib_SOURCE_DIR may point to non-existent directory
IF(NOT mrigtl_lib_LEGACY_REMOVE)
  SET(mrigtl_lib_SOURCE_DIR \"${mrigtl_lib_SOURCE_DIR}\")
ENDIF(NOT mrigtl_lib_LEGACY_REMOVE)"
)

# The "use" file.
SET(mrigtl_lib_USE_FILE ${mrigtl_lib_BINARY_DIR}/Usemrigtl_lib.cmake)

# The build settings file.
SET(mrigtl_lib_BUILD_SETTINGS_FILE ${mrigtl_lib_BINARY_DIR}/mrigtl_libBuildSettings.cmake)

# Library directory.
SET(mrigtl_lib_LIBRARY_DIRS_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Determine the include directories needed.
SET(mrigtl_lib_INCLUDE_DIRS_CONFIG
  ${mrigtl_lib_INCLUDE_DIRS}
)

#-----------------------------------------------------------------------------
# Configure mrigtl_libConfig.cmake for the build tree.
CONFIGURE_FILE(${mrigtl_lib_SOURCE_DIR}/mrigtl_libConfig.cmake.in
               ${mrigtl_lib_BINARY_DIR}/mrigtl_libConfig.cmake @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Settings specific to the install tree.

# store old mrigtl_lib_LIBRARY_TARGETS_FILE
SET(mrigtl_lib_LIBRARY_TARGETS_FILE_BUILDTREE ${mrigtl_lib_LIBRARY_TARGETS_FILE})

# The library dependencies file.
SET(mrigtl_lib_LIBRARY_TARGETS_FILE "\${mrigtl_lib_INSTALL_PREFIX}/${mrigtl_lib_INSTALL_PACKAGE_DIR}/mrigtl_libTargets.cmake")

# The "use" file.
SET(mrigtl_lib_USE_FILE \${mrigtl_lib_INSTALL_PREFIX}/${mrigtl_lib_INSTALL_PACKAGE_DIR}/Usemrigtl_lib.cmake)

# The build settings file.
SET(mrigtl_lib_BUILD_SETTINGS_FILE \${mrigtl_lib_INSTALL_PREFIX}/${mrigtl_lib_INSTALL_PACKAGE_DIR}/mrigtl_libBuildSettings.cmake)

# Include directories.
SET(mrigtl_lib_INCLUDE_DIRS_CONFIG \${mrigtl_lib_INSTALL_PREFIX}/${mrigtl_lib_INSTALL_INCLUDE_DIR})
FOREACH(DIR ${mrigtl_lib_INCLUDE_RELATIVE_DIRS})
  LIST(APPEND mrigtl_lib_INCLUDE_DIRS_CONFIG \${mrigtl_lib_INSTALL_PREFIX}/${mrigtl_lib_INSTALL_INCLUDE_DIR}/${DIR})
ENDFOREACH(DIR)
IF(mrigtl_lib_INCLUDE_DIRS_SYSTEM)
  LIST(APPEND mrigtl_lib_INCLUDE_DIRS_CONFIG ${mrigtl_lib_INCLUDE_DIRS_SYSTEM})
ENDIF()

# Link directories.
SET(mrigtl_lib_LIBRARY_DIRS_CONFIG "\${mrigtl_lib_INSTALL_PREFIX}/${mrigtl_lib_INSTALL_LIB_DIR}")

#-----------------------------------------------------------------------------
# Configure mrigtl_libConfig.cmake for the install tree.

# Construct the proper number of GET_FILENAME_COMPONENT(... PATH)
# calls to compute the installation prefix.
STRING(REGEX REPLACE "/" ";" mrigtl_lib_INSTALL_PACKAGE_DIR_COUNT
  "${mrigtl_lib_INSTALL_PACKAGE_DIR}")
SET(mrigtl_lib_CONFIG_CODE "
# Compute the installation prefix from this mrigtl_libConfig.cmake file location.
GET_FILENAME_COMPONENT(mrigtl_lib_INSTALL_PREFIX \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)")
FOREACH(p ${mrigtl_lib_INSTALL_PACKAGE_DIR_COUNT})
  SET(mrigtl_lib_CONFIG_CODE
    "${mrigtl_lib_CONFIG_CODE}\nGET_FILENAME_COMPONENT(mrigtl_lib_INSTALL_PREFIX \"\${mrigtl_lib_INSTALL_PREFIX}\" PATH)"
    )
ENDFOREACH(p)


CONFIGURE_FILE(${mrigtl_lib_SOURCE_DIR}/mrigtl_libConfig.cmake.in
               ${mrigtl_lib_BINARY_DIR}/Utilities/mrigtl_libConfig.cmake @ONLY IMMEDIATE)

# restore old mrigtl_lib_LIBRARY_TARGETS_FILE
SET(mrigtl_lib_LIBRARY_TARGETS_FILE ${mrigtl_lib_LIBRARY_TARGETS_FILE_BUILDTREE})
