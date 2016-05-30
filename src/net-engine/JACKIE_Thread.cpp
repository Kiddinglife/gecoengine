/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "JACKIE_Thread.h"
#include "geco-basic-type.h"
#include "geco-malloc-interface.h"
#include "geco-globals.h"

using namespace geco::net;


int JACKIE_Thread::Create(ThreadFunc start_address, void *arglist, int priority)
{
#ifdef _WIN32
	HANDLE threadHandle;
	unsigned threadID = 0;

#if  defined(WINDOWS_PHONE_8) || defined(WINDOWS_STORE_RT)
	threadHandle = CreateThread(NULL,0,start_address,arglist,CREATE_SUSPENDED, 0);
#elif defined _WIN32_WCE
	threadHandle = CreateThread(NULL,MAX_ALLOC_STACK_COUNT*2,start_address,arglist,0,(DWORD*)&threadID);
#else
	threadHandle = (HANDLE)_beginthreadex(NULL, MAX_ALLOC_STACK_COUNT * 2,start_address, arglist, 0, &threadID);
#endif

	SetThreadPriority(threadHandle, priority);

#if defined(WINDOWS_PHONE_8) || defined(WINDOWS_STORE_RT)
	ResumeThread(threadHandle);
#endif

	if (threadHandle == 0)
	{
		return 1;
	}
	else
	{
		CloseHandle(threadHandle);
		return 0;
	}
	return 0;
#else
	/// Create thread Linux
	pthread_t threadHandle;
	pthread_attr_t attr;
	sched_param param;
	param.sched_priority=priority;
	pthread_attr_init( &attr );
	pthread_attr_setschedparam(&attr, &param);
	pthread_attr_setstacksize(&attr, MAX_ALLOC_STACK_COUNT*2);
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	int res = pthread_create( &threadHandle, &attr, start_address, arglist );
	assert(res==0 && "pthread_create in RakThread.cpp failed.");
	return res;
#endif
}

unsigned int  JACKIE_Thread::JackieGetCurrentThreadId(void)
{
#ifdef _WIN32
	return (unsigned int)GetCurrentThreadId();
#elif defined GCC
	return 0;
#endif
	return 0;
}
