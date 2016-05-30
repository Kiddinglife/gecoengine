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

#include "geco-time.h"

#if defined(GET_TIME_SPIKE_LIMIT)&&GET_TIME_SPIKE_LIMIT > 0
#include "JACKIE_Simple_Mutex.h"
static TimeUS lastNormalizedReturnedValue = 0;
static TimeUS lastNormalizedInputValue = 0;
/// This constraints timer forward jumps to 1 second, and does not let it jump backwards
/// See http://support.microsoft.com/kb/274323 where the timer can sometimes jump forward by hours or days
/// This also has the effect where debugging a sending system won't treat the time spent halted past 1 second as elapsed network time
TimeUS NormalizeTime(TimeUS timeIn)
{
    TimeUS diff, lastNormalizedReturnedValueCopy;
    static JackieSimpleMutex mutex;

    mutex.Lock();
    if( timeIn >= lastNormalizedInputValue )
    {
        diff = timeIn - lastNormalizedInputValue;
        if( diff > GET_TIME_SPIKE_LIMIT )
            lastNormalizedReturnedValue += GET_TIME_SPIKE_LIMIT;
        else
            lastNormalizedReturnedValue += diff;
    } else
        lastNormalizedReturnedValue += GET_TIME_SPIKE_LIMIT;

    lastNormalizedInputValue = timeIn;
    lastNormalizedReturnedValueCopy = lastNormalizedReturnedValue;
    mutex.Unlock();

    return lastNormalizedReturnedValueCopy;
}
#endif // #if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0

/// Add gettimeofday() to win32 plateform
#if defined(_WIN32) && !defined(__GNUC__)  &&!defined(__GCCXML__)
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
#if defined(WINDOWS_PHONE_8) || defined(WINDOWS_STORE_RT)
    // _tzset not supported
    (void) tv;
    (void) tz;
#else

    FILETIME ft;
    unsigned __int64 tmpres = 0;
    static int tzflag;

    if (0 != tv)
    {
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres /= 10;  /*convert into microseconds*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (0 != tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

#endif

    return 0;
}
#endif


#if defined(_WIN32)
#include "geco-wins-includes.h" // Sleep
#else
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
static pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;
#endif

#if defined(WINDOWS_PHONE_8) || defined(WINDOWS_STORE_RT)
#include "../DependentExtensions/WinPhone8/ThreadEmulation.h"
using namespace ThreadEmulation;
#endif
void GecoSleep(unsigned int ms)
{
#if defined(_WIN32)
    Sleep(ms);
#else
    /// Single thread sleep code thanks to Furquan Shaikh, 
    /// http://somethingswhichidintknow.blogspot.com/2009/09/sleep-in-pthread.html
    ///  Modified slightly from the original
    struct timespec timeToWait;
    struct timeval now;
    int rt;

    gettimeofday(&now, NULL);

    long seconds = ms / 1000;
    long nanoseconds = ( ms - seconds * 1000 ) * 1000000;
    timeToWait.tv_sec = now.tv_sec + seconds;
    timeToWait.tv_nsec = now.tv_usec * 1000 + nanoseconds;

    if( timeToWait.tv_nsec >= 1000000000 )
    {
        timeToWait.tv_nsec -= 1000000000;
        timeToWait.tv_sec++;
    }

    pthread_mutex_lock(&fakeMutex);
    rt = pthread_cond_timedwait(&fakeCond, &fakeMutex, &timeToWait);
    pthread_mutex_unlock(&fakeMutex);
#endif
}