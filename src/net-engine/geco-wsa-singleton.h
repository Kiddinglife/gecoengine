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

#ifndef __INCLUDE_WSA_STARTUP_SINGLETON_H
#define __INCLUDE_WSA_STARTUP_SINGLETON_H

#include "geco-namesapces.h"
#include "geco-export.h"

#include <stdio.h>
#ifdef _WIN32
#include <WinSock2.h>
#endif

GECO_NET_BEGIN_NSPACE

class GECO_EXPORT  WSAStartupSingleton
{
    public:
    WSAStartupSingleton();
    ~WSAStartupSingleton();
    static void AddRef(void);
    static void Deref(void);

    protected:
    static int refCount;
};

//int WSAStartupSingleton::refCount = 0;
//
//void WSAStartupSingleton::AddRef(void)
//{
//#if defined(_WIN32) && !defined(WINDOWS_STORE_RT)
//
//    refCount++;
//
//    if (refCount != 1)
//        return;
//
//    WSADATA winsockInfo;
//    if (WSAStartup(MAKEWORD(2, 2), &winsockInfo) != 0)
//    {
//#if  defined(_DEBUG) && !defined(WINDOWS_PHONE_8)
//        DWORD dwIOError = GetLastError();
//        LPVOID messageBuffer;
//        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//            NULL, dwIOError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Default language
//            (LPTSTR)& messageBuffer, 0, NULL);
//        // something has gone wrong here...
//        printf_s("WSAStartup failed:Error code - %d,%s\n", dwIOError, messageBuffer);
//        //Free the buffer.
//        LocalFree(messageBuffer);
//#endif
//    }
//
//#endif
//}
//void WSAStartupSingleton::Deref(void)
//{
//#if defined(_WIN32) && !defined(WINDOWS_STORE_RT)
//    if (refCount == 0)
//        return;
//
//    if (refCount > 1)
//    {
//        refCount--;
//        return;
//    }
//    WSACleanup();
//    refCount = 0;
//#endif
//}
GECO_NET_END_NSPACE
#endif
