#ifndef MRIGTL_LIB_EXPORT_H
#define MRIGTL_LIB_EXPORT_H

#ifdef _WIN32
  #ifdef mrigtl_lib_shared_EXPORTS
    #define MRIGTL_LIB_EXPORT __declspec(dllexport)
  #else
    #define MRIGTL_LIB_EXPORT __declspec(dllimport)
  #endif
#else
  #define MRIGTL_LIB_EXPORT
#endif

#endif // MRIGTL_LIB_EXPORT_H