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

// created on 20-March-2016 by Jackie Zhang

#ifndef NET_TIME_H_
#define NET_TIME_H_

#include "geco-basic-type.h"
#include "geco-net-config.h"
#include "geco-export.h"

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

static bool initialized = false;

// Define USE_TIME_MS_64BITS if you want to use large types for GetTime
/// (takes more bandwidth when you transmit time though!)
// You would want to do this if your system is going to run long enough 
/// to overflow the millisecond counter (over a month)
#if USE_TIME_MS_64BITS ==1
typedef ulonglong Time;
typedef uint TimeMS;
typedef ulonglong TimeUS;
#else
typedef uint Time;
typedef uint TimeMS;
typedef ulonglong TimeUS;
#endif

/// win32 soes not have gettimeofday(), we add it to make it compatiable in plateforms
#if defined(_WIN32) && !defined(__GNUC__)  &&!defined(__GCCXML__)
#include <time.h>
#include "geco-wins-includes.h"
#if !defined(WINDOWS_PHONE_8)
// To call timeGetTime, on Code::Blocks, this needs to be libwinmm.a instead
#pragma comment(lib, "Winmm.lib")
#endif
struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};
#if defined(WINDOWS_STORE_RT)
struct timeval
{
    long    tv_sec;
    long    tv_usec;
};
#endif
GECO_EXPORT extern int  gettimeofday(struct timeval *tv, struct timezone *tz);
static TimeUS GetTimeUS_Windows(void)
{
    if (initialized == false)
    {
        initialized = true;

        // Save the current process
#if !defined(_WIN32_WCE)
        //		HANDLE mProc = GetCurrentProcess();

        // Get the current Affinity
#if _MSC_VER >= 1400 && defined (_M_X64)
        //		GetProcessAffinityMask(mProc, (PDWORD_PTR)&mProcMask, (PDWORD_PTR)&mSysMask);
#else
        //		GetProcessAffinityMask(mProc, &mProcMask, &mSysMask);
#endif
        //		mThread = GetCurrentThread();

#endif // _WIN32_WCE
    }

    // 9/26/2010 In China running LuDaShi, QueryPerformanceFrequency has to be called every time because CPU clock speeds can be different
    TimeUS curTime;
    LARGE_INTEGER PerfVal;
    LARGE_INTEGER yo1;

    QueryPerformanceFrequency(&yo1);
    QueryPerformanceCounter(&PerfVal);

    __int64 quotient, remainder;
    quotient = ((PerfVal.QuadPart) / yo1.QuadPart);
    remainder = ((PerfVal.QuadPart) % yo1.QuadPart);
    curTime = (TimeUS)quotient*(TimeUS)1000000 + (remainder*(TimeUS)1000000 / yo1.QuadPart);

#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
    return NormalizeTime(curTime);
#else
    return curTime;
#endif // #if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
}
#else
#include <sys/time.h>
#include <unistd.h>
static TimeUS initialTime;
static TimeUS GetTimeUS_Linux(void)
{
    timeval tp;

    if( initialized == false )
    {
        gettimeofday(&tp, 0);
        initialized = true;
        // I do this because otherwise Time in milliseconds won't work as it will underflow when dividing by 1000 to do the conversion
        initialTime = ( tp.tv_sec ) * (TimeUS) 1000000 + ( tp.tv_usec );
    }

    // GCC
    TimeUS curTime;
    gettimeofday(&tp, 0);

    curTime = ( tp.tv_sec ) * (TimeUS) 1000000 + ( tp.tv_usec );

#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
    return NormalizeTime(curTime - initialTime);
#else
    return curTime - initialTime;
#endif // #if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
}
#endif

/// Return the time as 64 bit
/// \note The maximum delta between returned calls is 1 second - however, RakNet calls this constantly anyway. See NormalizeTime() in the cpp.
inline GECO_EXPORT extern TimeUS Get64BitsTimeUS(void)
{
#if   defined(_WIN32)
    return GetTimeUS_Windows();
#else
    return GetTimeUS_Linux();
#endif
}

/// Same as GetTimeMS 
/// Holds the time in either a 32 or 64 bit variable, depending on __GET_TIME_64BIT
// GetTime
inline GECO_EXPORT extern Time  GetTimeMS(void)
{
    return (Time)(Get64BitsTimeUS() / 1000);
}

/// Return the time as 32 bit
/// \note The maximum delta between returned calls is 1 second - however, RakNet calls this constantly anyway. See NormalizeTime() in the cpp.
// GetTimeMS
inline GECO_EXPORT TimeMS Get32BitsTimeMS(void)
{
    return (TimeMS)(Get64BitsTimeUS() / 1000);
}

inline GECO_EXPORT extern bool  GreaterThan(Time a, Time b)
{
    // a > b?
    const Time halfSpan = (Time)(((Time)(const Time)-1) / (Time)2);
    return b != a && b - a > halfSpan;
}
inline GECO_EXPORT extern bool  LessThan(Time a, Time b)
{
    // a < b?
    const Time halfSpan = ((Time)(const Time)-1) / (Time)2;
    return b != a && b - a < halfSpan;
}

extern GECO_EXPORT void GecoSleep(unsigned int ms);
#endif