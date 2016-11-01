//{future header message}
#ifndef __FvTimeStamp_H__
#define __FvTimeStamp_H__

#include "FvKernel.h"
#include "FvBaseTypes.h"

#ifndef _XBOX360
#ifdef unix
// #define FV_USE_RDTSC
#else // unix
// # define FV_USE_RDTSC
#endif // !unix
#endif // !_XBOX360

#ifdef unix

enum FvTimingMethod
{
	RDTSC_TIMING_METHOD,
	GET_TIME_OF_DAY_TIMING_METHOD,
	GET_TIME_TIMING_METHOD,
	NO_TIMING_METHOD,
};

extern FvTimingMethod g_eTimingMethod;

FV_INLINE FvUInt64 FV_KERNEL_API TimestampRDTSC()
{
	FvUInt64	ret;
	__asm__ __volatile__ (
		"rdtsc":
		"=A"	(ret) 	
	);
	return ret;
}

#include <sys/time.h>

FV_INLINE FvUInt64 FV_KERNEL_API TimestampGetTimeofday()
{
	timeval tv;
	gettimeofday( &tv, NULL );

	return 1000000ULL * FvUInt64( tv.tv_sec ) + FvUInt64( tv.tv_usec );
}

#include <asm/unistd.h>

FV_INLINE FvUInt64 FV_KERNEL_API TimestampGetTime()
{
	timespec tv;
	FV_VERIFY(syscall( __NR_clock_gettime, CLOCK_MONOTONIC, &tv ) == 0);

	return 1000000000ULL * tv.tv_sec + tv.tv_nsec;
}

FV_INLINE 
FvUInt64 FV_KERNEL_API Timestamp()
{
#ifdef FV_USE_RDTSC
	return TimestampRDTSC();
#else // FV_USE_RDTSC
	if (g_eTimingMethod == RDTSC_TIMING_METHOD)
		return TimestampRDTSC();
	else if (g_eTimingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return TimestampGetTimeofday();
	else //if (g_timingMethod == GET_TIME_TIMING_METHOD)
		return TimestampGetTime();

#endif // !FV_USE_RDTSC
}

#elif defined(_WIN32)

#ifdef FV_USE_RDTSC
#pragma warning (push)
#pragma warning (disable: 4035)
FV_INLINE FvUInt64 FV_KERNEL_API Timestamp()
{
	__asm rdtsc
}
#pragma warning (pop)
#else // FV_USE_RDTSC

#ifdef _XBOX360
#include <xtl.h>
#else // _XBOX360
#include <windows.h>
#endif // _XBOX360

FV_INLINE FvUInt64 FV_KERNEL_API Timestamp()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter( &counter );
	return counter.QuadPart;
}

#endif // FV_USE_RDTSC

#elif defined( PLAYSTATION3 )

FV_INLINE FvUInt64 FV_KERNEL_API Timestamp()
{
	FvUInt64 ts;
	SYS_TIMEBASE_GET( ts );
	return ts;
}

#else
	#error Unsupported platform!
#endif

FvUInt64 FV_KERNEL_API StampsPerSecond();
double FV_KERNEL_API StampsPerSecondD();

FvUInt64 FV_KERNEL_API StampsPerSecondRDTSC();
double FV_KERNEL_API StampsPerSecondDRDTSC();

FvUInt64 FV_KERNEL_API StampsPerSecondGetTimeofday();
double FV_KERNEL_API StampsPerSecondDGetTimeofday();

#endif // __FvTimeStamp_H__