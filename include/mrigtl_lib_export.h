#ifndef MRIGTL_LIB_EXPORT_H
#define MRIGTL_LIB_EXPORT_H

#include <QtCore/qglobal.h>

// Detect if we're building a static library
// Static libraries don't need any DLL export/import decorations
#if defined(mrigtl_lib_static_EXPORTS) || defined(mrigtl_lib_static_IMPORTS) || defined(MRIGTL_STATIC_DEFINE)
  // Static library - no exports needed
  #define MRIGTL_LIB_EXPORT
  #define MRIGTL_QT_EXPORT
#else
  // Shared library (DLL) - use appropriate exports
  #if defined(mrigtl_lib_shared_EXPORTS)
    #define MRIGTL_LIB_EXPORT Q_DECL_EXPORT
  #else
    #define MRIGTL_LIB_EXPORT Q_DECL_IMPORT
  #endif

  // Platform-specific Qt class export handling
  // On Windows, we use class-level exports for Qt classes to ensure MOC symbols are exported
  // On other platforms, we use function-level exports to avoid any potential issues
  #ifdef _WIN32
    #define MRIGTL_QT_EXPORT MRIGTL_LIB_EXPORT
  #else
    #define MRIGTL_QT_EXPORT
  #endif
#endif

#endif // MRIGTL_LIB_EXPORT_H