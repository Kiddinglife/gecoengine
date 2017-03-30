//{future header message}
#ifndef __GECO_CONFIG_WIN_COMMON_H__
#define __GECO_CONFIG_WIN_COMMON_H__

#ifndef __GECO_CONFIG_WIN_H__
#error Use geco-config-win.h in geco-config.h instead of this header
#endif /* __GECO_CONFIG_WIN_H__ */

// Windows Mobile (CE) stuff is primarily further restrictions to what's
// in the rest of this file. Also, it defined GECO_HAS_WINCE, which is used
// in this file.
#if defined (_WIN32_WCE)
#    include "geco-config-wince.h"
#endif /* _WIN32_WCE */

// Complain if WIN32 is not already defined.
#if !defined (WIN32) && !defined (GECO_HAS_WINCE)
# error Please define WIN32 in your project settings.
#endif

#define GECO_WIN32
#if defined (_WIN64) || defined (WIN64)
#  define GECO_WIN64

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
// also set GECO_NDEBUG, unless the user has already set it.
#if defined (NDEBUG)
#  if !defined (GECO_NDEBUG)
#    define GECO_NDEBUG
#  endif /* GECO_NDEBUG */
#endif /* GECO_NDEBUG */

// GECO_HAS_MFC 定义为 1 表示你希望使用MFC函数
#if !defined (GECO_HAS_MFC)
# define GECO_HAS_MFC 0
#endif

// GECO_USES_STATIC_MFC always implies GECO_HAS_MFC
#if defined (GECO_USES_STATIC_MFC)
# if defined (GECO_HAS_MFC)
#   undef GECO_HAS_MFC
# endif
# define GECO_HAS_MFC 1
#endif /* GECO_USES_STATIC_MFC */

// GECO_HAS_STRICT 定义为 1表示你希望使用 STRICT 类型检查
#if !defined (GECO_HAS_STRICT)
# define GECO_HAS_STRICT 0
#endif

// MFC itself defines STRICT.
#if defined (GECO_HAS_MFC) && (GECO_HAS_MFC != 0)
# undef GECO_HAS_STRICT
# define GECO_HAS_STRICT 1
#endif

// 定义为1表示你希望使用的库线程安全
#if !defined (GECO_MT_SAFE)
# define GECO_MT_SAFE 1
#endif /* GECO_MT_SAFE */

// Define the special export macros needed to export symbols outside a dll
#if !defined(__BORLANDC__)
#define GECO_HAS_CUSTOM_EXPORT_MACROS 1
#define GECO_Proper_Export_Flag __declspec (dllexport)
#define GECO_Proper_Import_Flag __declspec (dllimport)
#define GECO_EXPORT_SINGLETON_DECLARATION(T) template class __declspec (dllexport) T
#define GECO_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) template class __declspec (dllexport) SINGLETON_TYPE<CLASS, LOCK>;
#define GECO_IMPORT_SINGLETON_DECLARATION(T) extern template class T
#define GECO_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) extern template class SINGLETON_TYPE <CLASS, LOCK>;
#endif /* !__BORLANDC__ */

#if !defined (FD_SETSIZE)
#define FD_SETSIZE 1024
#endif /* FD_SETSIZE */

// Windows doesn't like 65536 ;-) If 65536 is specified, it is
// silently ignored by the OS, i.e., setsockopt does not fail, and you
// get stuck with the default size of 8k.
#define GECO_DEFAULT_MAX_SOCKET_BUFSIZ 65535

// It seems like Win32 does not have a limit on the number of buffers
// that can be transferred by the scatter/gather type of I/O
// functions, e.g., WSASend and WSARecv.  We are setting this to be 64
// for now.  The typically use case is to create an I/O vector array
// of size GECO_IOV_MAX on the stack and then filled in.  Note that we
// probably don't want too big a value for GECO_IOV_MAX since it may
// mostly go to waste or the size of the activation record may become
// excessively large.
#if !defined (GECO_IOV_MAX)
# define GECO_IOV_MAX 64
#endif /* GECO_IOV_MAX */

#if !defined (GECO_LACKS_LONGLONG_T) && !defined (__MINGW32__)
#define GECO_INT64_TYPE  signed __int64
#define GECO_UINT64_TYPE unsigned __int64
#endif /* (ghs) */

#if defined (__MINGW32__)
#define GECO_INT64_TYPE  signed long long
#define GECO_UINT64_TYPE unsigned long long
#endif

// Always use WS2 when available
#if !defined(GECO_HAS_WINSOCK2)
# define GECO_HAS_WINSOCK2 0
#endif /* !defined(GECO_HAS_WINSOCK2) */

#if defined (GECO_HAS_WINSOCK2) && (GECO_HAS_WINSOCK2 != 0)
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

# if defined (GECO_HAS_FORE_ATM_WS2)
#  include /**/ <ws2atm.h>
# endif /*GECO_HAS_FORE_ATM_WS2 */

// CE doesn't have Microsoft Winsock 2 extensions
# if !defined _MSWSOCK_ && !defined (GECO_HAS_WINCE)
#  include /**/ <mswsock.h>
# endif /* _MSWSOCK_ */

# if defined (_MSC_VER)
#  if defined (GECO_HAS_WINCE)
#    pragma comment(lib, "ws2.lib")
#  else
#    pragma comment(lib, "ws2_32.lib")
#    pragma comment(lib, "mswsock.lib")
#  endif /* GECO_HAS_WINCE */
# endif /* _MSC_VER */

# define GECO_WSOCK_VERSION 2, 0
#else
# if !defined (_WINSOCKAPI_)
// will also include windows.h, if not present
#  include /**/ <winsock.h>
# endif /* _WINSOCKAPI */

// PharLap ETS has its own winsock lib, so don't grab the one
// supplied with the OS.
# if defined (_MSC_VER) && !defined (UNDER_CE) && !defined (GECO_HAS_PHARLAP)
#  pragma comment(lib, "wsock32.lib")
# endif /* _MSC_VER */

// We can't use recvmsg and sendmsg unless WinSock 2 is available
# define GECO_LACKS_RECVMSG
# define GECO_LACKS_SENDMSG

// Version 1.1 of WinSock
# define GECO_WSOCK_VERSION 1, 1
#endif /* GECO_HAS_WINSOCK2 */

// Platform supports IP multicast on Winsock 2
#if defined (GECO_HAS_WINSOCK2) && (GECO_HAS_WINSOCK2 != 0)
# define GECO_HAS_IP_MULTICAST
#endif /* GECO_HAS_WINSOCK2 */

#endif /* __GECO_CONFIG_WIN_COMMON_H__ */
