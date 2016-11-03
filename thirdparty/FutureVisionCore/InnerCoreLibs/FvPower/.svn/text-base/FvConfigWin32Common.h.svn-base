//{future header message}
#ifndef __FvConfigWin32Common_H__
#define __FvConfigWin32Common_H__



#ifndef __FvConfigWin32_H__
#error Use FvConfigWin32.h in config.h instead of this header
#endif /* __FvConfigWin32_H__ */

// Windows Mobile (CE) stuff is primarily further restrictions to what's
// in the rest of this file. Also, it defined FV_HAS_WINCE, which is used
// in this file.
#if defined (_WIN32_WCE)
#  include "FvConfigWinCE.h"
#endif /* _WIN32_WCE */

// Complain if WIN32 is not already defined.
#if !defined (WIN32) && !defined (FV_HAS_WINCE)
# error Please define WIN32 in your project settings.
#endif

#define FV_WIN32
#if defined (_WIN64) || defined (WIN64)
#  define FV_WIN64

// Use 64-bit file offsets by default in the WIN64 case, similar to
// what 64-bit UNIX systems do.
#  ifndef _FILE_OFFSET_BITS
#    define _FILE_OFFSET_BITS 64
#  endif  /* !_FILE_OFFSET_BITS */
#endif /* _WIN64 || WIN64 */

#if !defined (_WIN32_WINNT)
# define _WIN32_WINNT 0x0400
#endif

// If the invoking procedure turned off debugging by setting NDEBUG, then
// also set FV_NDEBUG, unless the user has already set it.
#if defined (NDEBUG)
#  if !defined (FV_NDEBUG)
#    define FV_NDEBUG
#  endif /* FV_NDEBUG */
#endif /* FV_NDEBUG */

// FV_HAS_MFC 定义为 1 表示你希望使用MFC函数
#if !defined (FV_HAS_MFC)
# define FV_HAS_MFC 0
#endif

// FV_USES_STATIC_MFC always implies FV_HAS_MFC
#if defined (FV_USES_STATIC_MFC)
# if defined (FV_HAS_MFC)
#   undef FV_HAS_MFC
# endif
# define FV_HAS_MFC 1
#endif /* FV_USES_STATIC_MFC */

// FV_HAS_STRICT 定义为 1表示你希望使用 STRICT 类型检查
#if !defined (FV_HAS_STRICT)
# define FV_HAS_STRICT 0
#endif

// MFC itself defines STRICT.
#if defined (FV_HAS_MFC) && (FV_HAS_MFC != 0)
# undef FV_HAS_STRICT
# define FV_HAS_STRICT 1
#endif

// 定义为1表示你希望使用的库线程安全
#if !defined (FV_MT_SAFE)
# define FV_MT_SAFE 1
#endif /* FV_MT_SAFE */

// Define the special export macros needed to export symbols outside a dll
#if !defined(__BORLANDC__)
#define FV_HAS_CUSTOM_EXPORT_MACROS 1
#define FV_Proper_Export_Flag __declspec (dllexport)
#define FV_Proper_Import_Flag __declspec (dllimport)
#define FV_EXPORT_SINGLETON_DECLARATION(T) template class __declspec (dllexport) T
#define FV_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) template class __declspec (dllexport) SINGLETON_TYPE<CLASS, LOCK>;
#define FV_IMPORT_SINGLETON_DECLARATION(T) extern template class T
#define FV_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) extern template class SINGLETON_TYPE <CLASS, LOCK>;
#endif /* !__BORLANDC__ */

#if !defined (FD_SETSIZE)
#define FD_SETSIZE 1024
#endif /* FD_SETSIZE */

// Windows doesn't like 65536 ;-) If 65536 is specified, it is
// silently ignored by the OS, i.e., setsockopt does not fail, and you
// get stuck with the default size of 8k.
#define FV_DEFAULT_MAX_SOCKET_BUFSIZ 65535

// It seems like Win32 does not have a limit on the number of buffers
// that can be transferred by the scatter/gather type of I/O
// functions, e.g., WSASend and WSARecv.  We are setting this to be 64
// for now.  The typically use case is to create an I/O vector array
// of size FV_IOV_MAX on the stack and then filled in.  Note that we
// probably don't want too big a value for FV_IOV_MAX since it may
// mostly go to waste or the size of the activation record may become
// excessively large.
#if !defined (FV_IOV_MAX)
# define FV_IOV_MAX 64
#endif /* FV_IOV_MAX */

#if !defined (FV_LACKS_LONGLONG_T) && !defined (__MINGW32__)
#define FV_INT64_TYPE  signed __int64
#define FV_UINT64_TYPE unsigned __int64
#endif /* (ghs) */

#if defined (__MINGW32__)
#define FV_INT64_TYPE  signed long long
#define FV_UINT64_TYPE unsigned long long
#endif

// Always use WS2 when available
#if !defined(FV_HAS_WINSOCK2)
# define FV_HAS_WINSOCK2 0
#endif /* !defined(FV_HAS_WINSOCK2) */

#if defined (FV_HAS_WINSOCK2) && (FV_HAS_WINSOCK2 != 0)
# if !defined (_WINSOCK2API_)
// will also include windows.h, if not present
#  include /**/ <winsock2.h>
// WinCE 4 doesn't define the Exxx values without the WSA prefix, so do that
// here. This is all lifted from the #if 0'd out part of winsock2.h.
#  if defined (UNDER_CE)
#    define EWOULDBLOCK             WSAEWOULDBLOCK
#    define EINPROGRESS             WSAEINPROGRESS
#    define EALREADY                WSAEALREADY
#    define ENOTSOCK                WSAENOTSOCK
#    define EDESTADDRREQ            WSAEDESTADDRREQ
#    define EMSGSIZE                WSAEMSGSIZE
#    define EPROTOTYPE              WSAEPROTOTYPE
#    define ENOPROTOOPT             WSAENOPROTOOPT
#    define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#    define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#    define EOPNOTSUPP              WSAEOPNOTSUPP
#    define EPFNOSUPPORT            WSAEPFNOSUPPORT
#    define EAFNOSUPPORT            WSAEAFNOSUPPORT
#    define EADDRINUSE              WSAEADDRINUSE
#    define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#    define ENETDOWN                WSAENETDOWN
#    define ENETUNREACH             WSAENETUNREACH
#    define ENETRESET               WSAENETRESET
#    define ECONNABORTED            WSAECONNABORTED
#    define ECONNRESET              WSAECONNRESET
#    define ENOBUFS                 WSAENOBUFS
#    define EISCONN                 WSAEISCONN
#    define ENOTCONN                WSAENOTCONN
#    define ESHUTDOWN               WSAESHUTDOWN
#    define ETOOMANYREFS            WSAETOOMANYREFS
#    define ETIMEDOUT               WSAETIMEDOUT
#    define ECONNREFUSED            WSAECONNREFUSED
#    define ELOOP                   WSAELOOP
#    define ENAMETOOLONG            WSAENAMETOOLONG
#    define EHOSTDOWN               WSAEHOSTDOWN
#    define EHOSTUNREACH            WSAEHOSTUNREACH
#    define ENOTEMPTY               WSAENOTEMPTY
#    define EPROCLIM                WSAEPROCLIM
#    define EUSERS                  WSAEUSERS
#    define EDQUOT                  WSAEDQUOT
#    define ESTALE                  WSAESTALE
#    define EREMOTE                 WSAEREMOTE
#  endif /* UNDER_CE */
# endif /* _WINSOCK2API */

# if defined (FV_HAS_FORE_ATM_WS2)
#  include /**/ <ws2atm.h>
# endif /*FV_HAS_FORE_ATM_WS2 */

// CE doesn't have Microsoft Winsock 2 extensions
# if !defined _MSWSOCK_ && !defined (FV_HAS_WINCE)
#  include /**/ <mswsock.h>
# endif /* _MSWSOCK_ */

# if defined (_MSC_VER)
#  if defined (FV_HAS_WINCE)
#    pragma comment(lib, "ws2.lib")
#  else
#    pragma comment(lib, "ws2_32.lib")
#    pragma comment(lib, "mswsock.lib")
#  endif /* FV_HAS_WINCE */
# endif /* _MSC_VER */

# define FV_WSOCK_VERSION 2, 0
#else
# if !defined (_WINSOCKAPI_)
// will also include windows.h, if not present
#  include /**/ <winsock.h>
# endif /* _WINSOCKAPI */

// PharLap ETS has its own winsock lib, so don't grab the one
// supplied with the OS.
# if defined (_MSC_VER) && !defined (UNDER_CE) && !defined (FV_HAS_PHARLAP)
#  pragma comment(lib, "wsock32.lib")
# endif /* _MSC_VER */

// We can't use recvmsg and sendmsg unless WinSock 2 is available
# define FV_LACKS_RECVMSG
# define FV_LACKS_SENDMSG

// Version 1.1 of WinSock
# define FV_WSOCK_VERSION 1, 1
#endif /* FV_HAS_WINSOCK2 */

// Platform supports IP multicast on Winsock 2
#if defined (FV_HAS_WINSOCK2) && (FV_HAS_WINSOCK2 != 0)
# define FV_HAS_IP_MULTICAST
#endif /* FV_HAS_WINSOCK2 */



#endif /* __FvConfigWin32Common_H__ */
