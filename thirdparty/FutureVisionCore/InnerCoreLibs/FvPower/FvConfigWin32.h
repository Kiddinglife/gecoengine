//{future header message}
#ifndef __FvConfigWin32_H__
#define __FvConfigWin32_H__

#if !defined (__BORLANDC__)
#    include "FvConfigWin32Common.h"
#endif /* !__BORLANDC__ */

#if defined (__BORLANDC__)
#    include "FvConfigWin32Borland.h"
#elif defined (_MSC_VER)
#    include "FvConfigWin32MSVC.h"
#elif defined (ghs)
#    include "FvConfigWin32GHS.h"
#elif defined (__MINGW32__)
#    include "FvConfigWin32MingW.h"
#elif defined (__DMC__)
#    include "FvConfigWin32DMC.h"
#else
#    error Compiler is not supported
#endif

#endif /* __FvConfigWin32_H__ */
