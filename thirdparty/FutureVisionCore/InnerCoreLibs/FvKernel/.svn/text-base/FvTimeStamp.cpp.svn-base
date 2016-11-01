#include "FvTimestamp.h"
#include "FvDebug.h"

#ifdef unix

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef FV_USE_RDTSC
FvTimingMethod g_eTimingMethod = RDTSC_TIMING_METHOD;
#else // FV_USE_RDTSC
FvTimingMethod g_eTimingMethod = NO_TIMING_METHOD;
#endif // FV_USE_RDTSC

namespace
{
const char *TimingMethod()
{
	switch (g_eTimingMethod)
	{
		case NO_TIMING_METHOD:
			return "none";

		case RDTSC_TIMING_METHOD:
			return "rdtsc";

		case GET_TIME_OF_DAY_TIMING_METHOD:
			return "gettimeofday";

		case GET_TIME_TIMING_METHOD:
			return "gettime";

		default:
			return "Unknown";
	}
}
}

static FvUInt64 CalcStampsPerSecondRDTSC()
{
	struct timeval	tvBefore,	tvSleep = {0, 500000},	tvAfter;
	FvUInt64 stampBefore,	stampAfter;

	gettimeofday( &tvBefore, NULL );
	gettimeofday( &tvBefore, NULL );

	gettimeofday( &tvBefore, NULL );
	stampBefore = Timestamp();

	select( 0, NULL, NULL, NULL, &tvSleep );

	gettimeofday( &tvAfter, NULL );
	gettimeofday( &tvAfter, NULL );

	gettimeofday( &tvAfter, NULL );
	stampAfter = Timestamp();

	FvUInt64 microDelta =
		(tvAfter.tv_usec + 1000000 - tvBefore.tv_usec) % 1000000;
	FvUInt64 stampDelta = stampAfter - stampBefore;

	return ( stampDelta * 1000000ULL ) / microDelta;
}

static FvUInt64 CalcStampsPerSecondGetTime()
{
	return 1000000000ULL;
}

static FvUInt64 CalcStampsPerSecondGetTimeofday()
{
	return 1000000ULL;
}

static FvUInt64 CalcStampsPerSecond()
{
	static bool bFirstTime = true;
	if (bFirstTime)
	{
		FV_WATCH( "TimingMethod", &::TimingMethod );
		bFirstTime = false;
	}

#ifdef FV_USE_RDTSC
	return CalcStampsPerSecondRDTSC();
#else // FV_USE_RDTSC
	if (g_eTimingMethod == RDTSC_TIMING_METHOD)
		return CalcStampsPerSecondRDTSC();
	else if (g_eTimingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return CalcStampsPerSecondGetTimeofday();
	else if (g_eTimingMethod == GET_TIME_TIMING_METHOD)
		return CalcStampsPerSecondGetTime();
	else
	{
		char *pcTimingMethod = getenv( "FV_TIMING_METHOD" );
		if (!pcTimingMethod || strcmp( pcTimingMethod, "rdtsc" ) == 0)
		{
			g_eTimingMethod = RDTSC_TIMING_METHOD;
		}
		else if (strcmp( pcTimingMethod, "gettimeofday" ) == 0)
		{
			g_eTimingMethod = GET_TIME_OF_DAY_TIMING_METHOD;
		}
		else if (strcmp( pcTimingMethod, "gettime" ) == 0)
		{
			g_eTimingMethod = GET_TIME_TIMING_METHOD;
		}
		else
		{
			FV_WARNING_MSG( "CalcStampsPerSecond: "
						"Unknown time method '%s', using rdtsc.\n",
					pcTimingMethod );
			g_eTimingMethod = RDTSC_TIMING_METHOD;
		}

		return CalcStampsPerSecond();
	}
#endif // FV_USE_RDTSC
}


FvUInt64 StampsPerSecondRDTSC()
{
	static FvUInt64 stampsPerSecondCache = CalcStampsPerSecondRDTSC();
	return stampsPerSecondCache;
}

double StampsPerSecondDRDTSC()
{
	static double stampsPerSecondCacheD = double(StampsPerSecondRDTSC());
	return stampsPerSecondCacheD;
}

FvUInt64 StampsPerSecondGetTimeofday()
{
	static FvUInt64 stampsPerSecondCache = CalcStampsPerSecondGetTimeofday();
	return stampsPerSecondCache;
}

double stampsPerSecondD_gettimeofday()
{
	static double stampsPerSecondCacheD = double(StampsPerSecondGetTimeofday());
	return stampsPerSecondCacheD;
}


#elif defined(_WIN32)

#ifndef _XBOX360
#include <windows.h>
#endif // !_XBOX360
#include "FvProcessorAffinity.h"

#ifdef FV_USE_RDTSC

FvUInt64 g_uiBusyIdleCounter = 0;
volatile static bool ms_bContinueBusyIdle;
static DWORD WINAPI BusyIdleThread( LPVOID pkArg )
{
	FvProcessorAffinity::Update();

	SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_LOWEST );

	while (ms_bContinueBusyIdle) ++g_uiBusyIdleCounter;

	return 0;
}

static FvUInt64 CalcStampsPerSecond()
{
	LARGE_INTEGER	tvBefore,	tvAfter;
	DWORD			tvSleep = 500;
	FvUInt64 stampBefore,	stampAfter;

	FvProcessorAffinity::Update();

	ms_bContinueBusyIdle = true;
	DWORD iBusyIdleThreadID = 0;
	HANDLE thread = CreateThread( NULL, 0, &BusyIdleThread, NULL, 0, &iBusyIdleThreadID );
	Sleep( 100 );

	QueryPerformanceCounter( &tvBefore );
	QueryPerformanceCounter( &tvBefore );

	QueryPerformanceCounter( &tvBefore );
	stampBefore = Timestamp();

	Sleep(tvSleep);

	QueryPerformanceCounter( &tvAfter );
	QueryPerformanceCounter( &tvAfter );

	QueryPerformanceCounter( &tvAfter );
	stampAfter = Timestamp();

	FvUInt64 countDelta = tvAfter.QuadPart - tvBefore.QuadPart;
	FvUInt64 stampDelta = stampAfter - stampBefore;

	LARGE_INTEGER	frequency;
	QueryPerformanceFrequency( &frequency );

	ms_bContinueBusyIdle = false;
	CloseHandle( thread );

	return (FvUInt64)( ( stampDelta * FvUInt64(frequency.QuadPart) ) / countDelta );
}

#else // FV_USE_RDTSC

static FvUInt64 CalcStampsPerSecond()
{
	LARGE_INTEGER rate;
	FV_VERIFY( QueryPerformanceFrequency( &rate ) );
	return rate.QuadPart;
}

#endif // FV_USE_RDTSC

#endif // unix


#if defined( PLAYSTATION3 )

static FvUInt64 CalcStampsPerSecond()
{
	return sys_time_get_timebase_frequency();
}

#endif // PLAYSTATION3

FvUInt64 StampsPerSecond()
{
	static FvUInt64 uiStampsPerSecondCache = CalcStampsPerSecond();
	return uiStampsPerSecondCache;
}

double StampsPerSecondD()
{
	static double iStampsPerSecondCacheD = double(StampsPerSecond());
	return iStampsPerSecondCacheD;
}
