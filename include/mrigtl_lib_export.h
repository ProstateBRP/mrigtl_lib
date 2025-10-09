#ifndef MRIGTL_LIB_EXPORT_H
#define MRIGTL_LIB_EXPORT_H

#include <QtCore/qglobal.h>

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

#endif // MRIGTL_LIB_EXPORT_H