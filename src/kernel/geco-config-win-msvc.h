/*
* Geco Gaming Company
* All Rights Reserved.
* Copyright (c)  2016 GECOEngine.
*
* GECOEngine is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* GECOEngine is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.

* You should have received a copy of the GNU Lesser General Public License
* along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*
*/

//{future header message}
#ifndef __GECO_CONFIG_WIN_MSVC_H__
#define __GECO_CONFIG_WIN_MSVC_H__

#ifndef __GECO_CONFIG_WIN_H__
#error Use geco-config-win.h in geco-config.h instead of this header
#endif /* __GECO_CONFIG_WIN_H__ */

#define GECO_CC_NAME "Visual C++"
#define GECO_CC_PREPROCESSOR "CL.EXE"
#define GECO_CC_PREPROCESSOR_ARGS "-nologo -E"

#define GECO_CC_MAJOR_VERSION (_MSC_VER / 100 - 6)
#define GECO_CC_MINOR_VERSION (_MSC_VER % 100)
#define GECO_CC_BETA_VERSION (0)

#if !defined (GECO_LD_DECORATOR_STR)
# if defined (_DEBUG)
#  define GECO_LD_DECORATOR_STR "d"
# endif  /* _DEBUG */
#endif  /* FR_LD_DECORATOR_STR */

#if !defined(_NATIVE_WCHAR_T_DEFINED)
#define GECO_LACKS_NATIVE_WCHAR_T
#endif

// Win Mobile 与 PC的退出方式不同
#if defined (_WIN32_WCE)
#  define GECO_ENDTHREADEX(STATUS) ExitThread ((DWORD) STATUS)
#else
#  define GECO_ENDTHREADEX(STATUS) ::_endthreadex ((DWORD) STATUS)
#endif /* _WIN32_WCE */

#if (_MSC_VER >= 1500)
# include "geco-config-win-msvc-9.h"
#elif (_MSC_VER >= 1400)
# include "geco-config-win-msvc-8.h"
#elif (_MSC_VER >= 1310)
# include "geco-config-win-msvc-7.h"
#else
# error This version of Microsoft Visual C++ is not supported.
#endif

#define GECO_INT64_FORMAT_SPECIFIER "%I64d"
#define GECO_UINT64_FORMAT_SPECIFIER "%I64u"

// 为 /W4 设置关闭警告 

#if !defined (ALL_WARNINGS) && defined(_MSC_VER) && !defined(ghs) && !defined(__MINGW32__)
#     pragma warning(disable: 4127)  /* constant expression for TRACE/ASSERT */
#     pragma warning(disable: 4134)  /* message map member fxn casts */
#     pragma warning(disable: 4511)  /* private copy constructors are good to have */
#     pragma warning(disable: 4512)  /* private operator= are good to have */
#     pragma warning(disable: 4514)  /* unreferenced inlines are common */
#     pragma warning(disable: 4710)  /* private constructors are disallowed */
#     pragma warning(disable: 4705)  /* statement has no effect in optimized code */
#     pragma warning(disable: 4791)  /* loss of debugging info in retail version */
#     pragma warning(disable: 4275)  /* deriving exported class from non-exported */
#     pragma warning(disable: 4251)  /* using non-exported as public in exported */
#     pragma warning(disable: 4786)  /* identifier was truncated to '255' characters in the browser information */
#     pragma warning(disable: 4097)  /* typedef-name used as synonym for class-name */
#     pragma warning(disable: 4800)  /* converting int to boolean */
# if defined (__INTEL_COMPILER)
#     pragma warning(disable: 1744)  /* field of class type without a DLL interface used in a class with a DLL interface */
#     pragma warning(disable: 1738)
# endif
#endif /* !ALL_WARNINGS && _MSV_VER && !ghs && !__MINGW32__ */

// STRICT type checking in WINDOWS.H enhances type safety for Windows
// programs by using distinct types to represent all the different
// HANDLES in Windows. So for example, STRICT prevents you from
// mistakenly passing an HPEN to a routine expecting an HBITMAP.
// Note that we only use this if we
#   if defined (GECO_HAS_STRICT) && (GECO_HAS_STRICT != 0)
#     if !defined (STRICT)   /* may already be defined */
#       define STRICT
#     endif /* !STRICT */
#   endif /* GECO_HAS_STRICT */

#endif /* __GECO_CONFIG_WIN_MSVC_H__ */
