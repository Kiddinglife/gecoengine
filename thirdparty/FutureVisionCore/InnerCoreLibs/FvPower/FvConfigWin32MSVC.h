//{future header message}
#ifndef __FvConfigWin32MSVC_H__
#define __FvConfigWin32MSVC_H__



#ifndef __FvConfigWin32_H__
#error Use FvConfigWin32.h in config.h instead of this header
#endif /* __FvConfigWin32_H__ */

#define FV_CC_NAME "Visual C++"
#define FV_CC_PREPROCESSOR "CL.EXE"
#define FV_CC_PREPROCESSOR_ARGS "-nologo -E"

#define FV_CC_MAJOR_VERSION (_MSC_VER / 100 - 6)
#define FV_CC_MINOR_VERSION (_MSC_VER % 100)
#define FV_CC_BETA_VERSION (0)

#if !defined (FV_LD_DECORATOR_STR)
# if defined (_DEBUG)
#  define FV_LD_DECORATOR_STR "d"
# endif  /* _DEBUG */
#endif  /* FR_LD_DECORATOR_STR */

#if !defined(_NATIVE_WCHAR_T_DEFINED)
#define FV_LACKS_NATIVE_WCHAR_T
#endif

// Win Mobile 与 PC的退出方式不同
#if defined (_WIN32_WCE)
#  define FV_ENDTHREADEX(STATUS) ExitThread ((DWORD) STATUS)
#else
#  define FV_ENDTHREADEX(STATUS) ::_endthreadex ((DWORD) STATUS)
#endif /* _WIN32_WCE */

#if (_MSC_VER >= 1500)
# include "FvConfigWin32MSVC9.h"
#elif (_MSC_VER >= 1400)
# include "FvConfigWin32MSVC8.h"
#elif (_MSC_VER >= 1310)
# include "FvConfigWin32MSVC7.h"
#else
# error This version of Microsoft Visual C++ is not supported.
#endif

#define Fv_INT64_FORMAT_SPECIFIER "%I64d"
#define Fv_UINT64_FORMAT_SPECIFIER "%I64u"

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
#   if defined (FV_HAS_STRICT) && (FV_HAS_STRICT != 0)
#     if !defined (STRICT)   /* may already be defined */
#       define STRICT
#     endif /* !STRICT */
#   endif /* FV_HAS_STRICT */



#endif /* __FvConfigWin32MSVC_H__ */
