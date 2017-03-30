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
#ifndef __GECO_CONFIG_H__
#define __GECO_CONFIG_H__

#if (!defined(_DEBUG) && !defined(NDEBUG) && !defined(SHIPPING))
#error exactly one of GECO_DEBUG,GECO_REALSE,GECO_SHIPPING must be defined.
#endif 

#if (!defined(GECO_BUILD_STATIC_LIBS) && !defined(GECO_BUILD_DLL_LIBS))
#error exactly one of GECO_BUILD_STATIC_LIBS, GECO_BUILD_DLL_LIBS must be defined.
#endif

#if (defined(GECO_BUILD_STATIC_LIBS) && defined(GECO_BUILD_DLL_LIBS))
#error exactly one of GECO_BUILD_STATIC_LIBS, GECO_BUILD_DLL_LIBS must be defined.
#endif 

#if (defined(GECO_BUILD_DLL_LIBS) && !defined(GECO_BUILD_DLL) && !defined(GECO_EXPORT_DLL))
#error exactly one of GECO_BUILD_DLL, GECO_EXPORT_DLL must be defined if GECO_BUILD_DLL_LIBS is defined.
#endif 

/*
The operating system, must be one of: (Q_OS_x)

MACX   - Mac OS X
MAC9   - Mac OS 9
MSDOS  - MS-DOS and Windows
OS2    - OS/2
OS2EMX - XFree86 on OS/2 (not PM)
WIN32  - Win32 (Windows 95/98/ME and Windows NT/2000/XP)
CYGWIN - Cygwin
SOLARIS    - Sun Solaris
HPUX   - HP-UX
ULTRIX - DEC Ultrix
LINUX  - Linux
FREEBSD    - FreeBSD
NETBSD - NetBSD
OPENBSD    - OpenBSD
BSDI   - BSD/OS
IRIX   - SGI Irix
OSF    - HP Tru64 UNIX
SCO    - SCO OpenServer 5
UNIXWARE   - UnixWare 7, Open UNIX 8
AIX    - AIX
HURD   - GNU Hurd
DGUX   - DG/UX
RELIANT    - Reliant UNIX
DYNIX  - DYNIX/ptx
QNX    - QNX
QNX6   - QNX RTP 6.1
LYNX   - LynxOS
BSD4   - Any BSD 4.4 system
UNIX   - Any UNIX BSD/SYSV system
*/
#if defined(__APPLE__) && defined(__GNUC__)
#  define Q_OS_MACX
#elif defined(__MACOSX__)
#  define Q_OS_MACX
#elif defined(macintosh)
#  define Q_OS_MAC9
#elif defined(__CYGWIN__)
#  define Q_OS_CYGWIN
#elif defined(MSDOS) || defined(_MSDOS)
#  define Q_OS_MSDOS
#elif defined(__OS2__)
#  if defined(__EMX__)
#    define Q_OS_OS2EMX
#  else
#    define Q_OS_OS2
#  endif
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define Q_OS_WIN32
#  define Q_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define Q_OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define Q_OS_WIN32
#elif defined(__sun) || defined(sun)
#  define Q_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#  define Q_OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#  define Q_OS_ULTRIX
#elif defined(sinix)
#  define Q_OS_RELIANT
#elif defined(__linux__) || defined(__linux)
#  define Q_OS_LINUX
#elif defined(__FreeBSD__)
#  define Q_OS_FREEBSD
#  define Q_OS_BSD4
#elif defined(__NetBSD__)
#  define Q_OS_NETBSD
#  define Q_OS_BSD4
#elif defined(__OpenBSD__)
#  define Q_OS_OPENBSD
#  define Q_OS_BSD4
#elif defined(__bsdi__)
#  define Q_OS_BSDI
#  define Q_OS_BSD4
#elif defined(__sgi)
#  define Q_OS_IRIX
#elif defined(__osf__)
#  define Q_OS_OSF
#elif defined(_AIX)
#  define Q_OS_AIX
#elif defined(__Lynx__)
#  define Q_OS_LYNX
#elif defined(__GNU_HURD__)
#  define Q_OS_HURD
#elif defined(__DGUX__)
#  define Q_OS_DGUX
#elif defined(__QNXNTO__)
#  define Q_OS_QNX6
#elif defined(__QNX__)
#  define Q_OS_QNX
#elif defined(_SEQUENT_)
#  define Q_OS_DYNIX
#elif defined(_SCO_DS)                   /* SCO OpenServer 5 + GCC */
#  define Q_OS_SCO
#elif defined(__USLC__)                  /* all SCO platforms + UDK or OUDK */
#  define Q_OS_UNIXWARE
#  define Q_OS_UNIXWARE7
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */
#  define Q_OS_UNIXWARE
#  define Q_OS_UNIXWARE7
#else
#  error "Qt has not been ported to this OS - talk to qt-bugs@trolltech.com"
#endif

#if defined(Q_OS_MAC9) || defined(Q_OS_MACX)
#  define Q_OS_MAC
#endif

#if defined(Q_OS_MAC9) || defined(Q_OS_MSDOS) || defined(Q_OS_OS2) || defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#  undef Q_OS_UNIX
#elif !defined(Q_OS_UNIX)
#  define Q_OS_UNIX
#endif

#if defined (Q_OS_WIN32) || defined (Q_OS_WIN64)
#include "geco-config-win.h"
#elif defined (Q_OS_LINUX) || defined (Q_OS_BSD4)
// #include "geco-config-unix-like.h"
#else
#  error "unknow plateform"
#endif
#endif /* __GECO_CONFIG_H__ */
