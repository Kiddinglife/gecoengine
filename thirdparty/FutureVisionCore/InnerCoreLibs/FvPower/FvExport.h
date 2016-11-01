//{future header message}
#ifndef __FvExport_H__
#define __FvExport_H__

#include "FvConfig.h"

#if defined (FV_AS_STATIC_LIBS)
# if !defined (FV_HAS_DLL)
#   define FV_HAS_DLL 0
# endif /* ! FV_HAS_DLL */
#else
# if !defined (FV_HAS_DLL)
#   define FV_HAS_DLL 1
# endif /* ! FV_HAS_DLL */
#endif /* FV_AS_STATIC_LIB */

#if defined (FV_HAS_DLL)
#  if (FV_HAS_DLL == 1)
#    if defined (FV_BUILD_DLL)
#      define FV_Export FV_Proper_Export_Flag
#      define FV_SINGLETON_DECLARATION(T) FV_EXPORT_SINGLETON_DECLARATION (T)
#      define FV_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) FV_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#    else
#      define FV_Export FV_Proper_Import_Flag
#      define FV_SINGLETON_DECLARATION(T) FV_IMPORT_SINGLETON_DECLARATION (T)
#      define FV_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) FV_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#    endif /* FV_BUILD_DLL */
#  else
#    define FV_Export
#    define FV_SINGLETON_DECLARATION(T)
#    define FV_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif   /* ! FV_HAS_DLL == 1 */
#else
#  define FV_Export
#  define FV_SINGLETON_DECLARATION(T)
#  define FV_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif     /* FV_HAS_DLL */



#endif /* __FvExport_H__ */
