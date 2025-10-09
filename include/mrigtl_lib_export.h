#ifndef MRIGTL_LIB_EXPORT_H
#define MRIGTL_LIB_EXPORT_H

#include <QtCore/qglobal.h>

// Platform-specific export handling
// On Windows: Use class-level exports for Qt classes, function-level exports become empty to avoid conflicts
// On other platforms: Use function-level exports, class-level exports become empty
#ifdef _WIN32
  #if defined(mrigtl_lib_shared_EXPORTS)
    #define MRIGTL_LIB_EXPORT Q_DECL_EXPORT
  #else
    #define MRIGTL_LIB_EXPORT Q_DECL_IMPORT
  #endif
  #define MRIGTL_QT_EXPORT MRIGTL_LIB_EXPORT
  // Redefine MRIGTL_LIB_EXPORT to empty for Qt class members to avoid DLL interface conflicts
  #undef MRIGTL_LIB_EXPORT
  #define MRIGTL_LIB_EXPORT
#else
  #if defined(mrigtl_lib_shared_EXPORTS)
    #define MRIGTL_LIB_EXPORT Q_DECL_EXPORT
  #else
    #define MRIGTL_LIB_EXPORT Q_DECL_IMPORT
  #endif
  #define MRIGTL_QT_EXPORT
#endif

#endif // MRIGTL_LIB_EXPORT_H