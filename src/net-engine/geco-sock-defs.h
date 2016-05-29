/*
 * Copyright (c) 2016
 * Geco Gaming Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for GECO purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation. Geco Gaming makes no
 * representations about the suitability of this software for GECO
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

// created on 19-March-2016 by Jackie Zhang

#ifndef __INCLUDE_GECO_SOCK_DEFS_H
#define __INCLUDE_GECO_SOCK_DEFS_H

/// Internal
#if defined(WINDOWS_STORE_RT)

#include "WinRTSocketAdapter.h"
#define accept__ WinRTAccept
#define connect__ WinRTConnect
#define closesocket__ WinRTClose
#define socket__ WinRTCreateDatagramSocket
#define bind__ WinRTBind
#define getsockname__ RNS2_WindowsStore8::WinRTGetSockName
#define getsockopt__ WinRTGetSockOpt
#define inet_addr__ RNS2_WindowsStore8::WinRTInet_Addr
#define ioctlsocket__ RNS2_WindowsStore8::WinRTIOCTLSocket
#define listen__ WinRTListen
#define recv__ WinRTRecv
#define recvfrom__ WinRTRecvFrom
#define select__ WinRTSelect
#define send__ WinRTSend
#define sendto__ WinRTSendTo
#define setsockopt__ RNS2_WindowsStore8::WinRTSetSockOpt
#define shutdown__ WinRTShutdown
#define WSASendTo__ WinRTSendTo

#else

#   if defined(_WIN32)
#define closesocket__ closesocket
#define select__ select
#   elif defined(__native_client__)
#define select__ select
#   else
#define closesocket__ close
#define select__ select
#   endif

#define accept__ accept
#define connect__ connect
#define socket__ socket
#define bind__ bind
#define getsockname__ getsockname
#define getsockopt__ getsockopt

///将字符串形式的IP地址转换为按网络字节顺序的整型值
/// convert the string ip addr to intege in network order
#define inet_addr__ inet_addr
#define ioctlsocket__ ioctlsocket
#define listen__ listen
#define recv__ recv
#define recvfrom__ recvfrom
#define sendto__ sendto
#define send__ send
#define setsockopt__ setsockopt
#define shutdown__ shutdown
#define WSASendTo__ WSASendTo
#endif

#endif //end
