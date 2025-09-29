#ifndef MRIGTL_LIB_EXPORT_H
#define MRIGTL_LIB_EXPORT_H

#include <QtCore/qglobal.h>

#if defined(mrigtl_lib_shared_EXPORTS)
  #define MRIGTL_LIB_EXPORT Q_DECL_EXPORT
#else
  #define MRIGTL_LIB_EXPORT Q_DECL_IMPORT
#endif

#endif // MRIGTL_LIB_EXPORT_H