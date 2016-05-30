/*
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __JACKIE_THREAD_H
#define __JACKIE_THREAD_H

#include "geco-export.h"
#include "geco-namesapces.h"

#if defined(_WIN32_WCE)
#include "WindowsIncludes.h"
#endif

#if defined(WINDOWS_PHONE_8) || defined(WINDOWS_STORE_RT)
#include "../DependentExtensions/WinPhone8/ThreadEmulation.h"
using namespace ThreadEmulation;
#endif

#if defined(_WIN32)
#include "geco-wins-includes.h"
#include <stdio.h>
#if !defined(_WIN32_WCE)
#include <process.h>
#endif
#else
#include <pthread.h>
#endif


GECO_NET_BEGIN_NSPACE

/// To define a thread, use JACKIE_THREAD_DECLARATION(functionName);
#if defined(_WIN32_WCE) || defined(WINDOWS_PHONE_8) || defined(WINDOWS_STORE_RT)
typedef LPTHREAD_START_ROUTINE ThreadFunc;
#define JACKIE_THREAD_DECLARATION(functionName)\
DWORD WINAPI functionName(LPVOID lpThreadParameter)
#elif defined(_WIN32)
typedef unsigned int(__stdcall *ThreadFunc) (void* arguments);
#define JACKIE_THREAD_DECLARATION(functionName)\
unsigned int __stdcall functionName( void* arguments )
#else
typedef void* (*ThreadFunc)(void* arguments);
#define JACKIE_THREAD_DECLARATION(functionName) \
void* functionName(void* arguments)
#endif

class GECO_EXPORT JACKIE_Thread
{
    /// Create a thread, simplified to be cross platform without all the extra junk
    /// To then start that thread, call RakCreateThread(functionName, arguments);
    /// \param[in] start_address Function you want to call
    /// \param[in] arglist Arguments to pass to the function
    /// \return 0=success. >0 = error code

    /*
    nice value 	Win32 Priority
    -20 to -16 	THREAD_PRIORITY_HIGHEST
    -15 to -6 	THREAD_PRIORITY_ABOVE_NORMAL
    -5 to +4 	THREAD_PRIORITY_NORMAL
    +5 to +14 	THREAD_PRIORITY_BELOW_NORMAL
    +15 to +19 	THREAD_PRIORITY_LOWEST
    */
    public:
    static int Create(ThreadFunc start_address, void *arglist, int priority = 0);
    static unsigned int JackieGetCurrentThreadId(void);
};

GECO_NET_END_NSPACE
#endif
