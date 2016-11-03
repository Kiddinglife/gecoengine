#include "FvPrintf.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdio.h>
#include <time.h>

#ifndef FV_ENABLE_DPRINTF
#define FV_ENABLE_DPRINTF 1
#endif // FV_ENABLE_DPRINTF
#if FV_ENABLE_DPRINTF

void FV_POWER_API FvDPrintfNormal( const char *pcFormat, ... )
{
	va_list kArgPtr;
	va_start( kArgPtr, pcFormat );
	FvVDPrintfNormal( pcFormat, kArgPtr, NULL );
	va_end(kArgPtr);
}

void FV_POWER_API FvDPrintfPriority( int iComponentLevel, int iSeverity, const char *pcFormat, ... )
{
	va_list kArgPtr;
	va_start( kArgPtr, pcFormat );
	FvVDPrintfPriority( iComponentLevel, iSeverity, pcFormat, kArgPtr, NULL );
	va_end( kArgPtr );
}

#endif // FV_ENABLE_DPRINTF

int g_bShouldWriteTimePrefix = 0;
FV_POWER_EXTERN_VAR int g_iShouldWriteToConsole = 1;

#if FV_ENABLE_DPRINTF
//! modify by Uman, 2010/11/28
void FV_POWER_API FvVDPrintfNormal( const char *pcFormat, va_list kArgPtr, const char *pcPrefix )
{
#ifdef _WIN32
	static char acBuf[ 2048 ];
	char *pcBuf = acBuf;
	int iLen = 0;
	if (pcPrefix)
	{
		iLen = strlen( pcPrefix );
		if(sizeof(acBuf)-1 < iLen)
			return;
		memcpy( pcBuf, pcPrefix, iLen );
		pcBuf += iLen;
		if (g_iShouldWriteToConsole)
		{
			fprintf( stderr, "%s", pcPrefix );
		}
	}

	vsnprintf_s( pcBuf,(sizeof(acBuf) - iLen),_TRUNCATE, pcFormat, kArgPtr );

	OutputDebugString( acBuf );
	if (g_iShouldWriteToConsole)
	{
		vfprintf( stderr, pcFormat, kArgPtr );
	}
#else // _WIN32
	if (g_iShouldWriteToConsole)
	{
		if (g_bShouldWriteTimePrefix)
		{
			time_t ttime = time( NULL );
			char acTimebuff[32];

			if (0 != strftime( acTimebuff, sizeof(acTimebuff),
				"%F %T: ", localtime( &ttime ) ))
			{
				fprintf( stderr, acTimebuff );
			}
		}

		if (pcPrefix)
		{
			fprintf( stderr, "%s", pcPrefix );
		}
		vfprintf( stderr, pcFormat, kArgPtr );
	}
#endif // !_WIN32
}
/**
void FV_POWER_API FvVDPrintfNormal( const char *pcFormat, va_list kArgPtr, const char *pcPrefix )
{
#ifdef _WIN32
	char acBuf[ 2048 ];
	char *pcBuf = acBuf;
	int iLen = 0;
	if (pcPrefix)
	{
		iLen = strlen( pcPrefix );
		memcpy( pcBuf, pcPrefix, iLen );
		pcBuf += iLen;
		if (g_iShouldWriteToConsole)
		{
			fprintf( stderr, "%s", pcPrefix );
		}
	}

	_vsnprintf_s( pcBuf,(sizeof(acBuf) - iLen),(sizeof(acBuf) - iLen), pcFormat, kArgPtr );
	acBuf[sizeof(acBuf)-1]=0;
	OutputDebugString( acBuf );
	if (g_iShouldWriteToConsole)
	{
		vfprintf( stderr, pcFormat, kArgPtr );
	}
#else // _WIN32
	if (g_iShouldWriteToConsole)
	{
		if (g_bShouldWriteTimePrefix)
		{
			time_t ttime = time( NULL );
			char acTimebuff[32];

			if (0 != strftime( acTimebuff, sizeof(acTimebuff),
				"%F %T: ", localtime( &ttime ) ))
			{
				fprintf( stderr, acTimebuff );
			}
		}

		if (pcPrefix)
		{
			fprintf( stderr, "%s", pcPrefix );
		}
		vfprintf( stderr, pcFormat, kArgPtr );
	}
#endif // !_WIN32
}
**/

void FV_POWER_API FvVDPrintfPriority( int iComponentPriority, int iMessagePriority,
			const char *pcFormat, va_list kArgPtr,
			const char *pcPrefix )
{
	FvVDPrintfNormal( pcFormat, kArgPtr, pcPrefix );
}

#endif // FV_ENABLE_DPRINTF