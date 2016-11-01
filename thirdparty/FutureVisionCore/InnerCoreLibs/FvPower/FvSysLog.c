#include "FvSysLog.h"

#include <stdio.h>

const char *s_pcSvcName = NULL;
#define MAX_MES_SIZE 1024

void FV_POWER_API FvOpenLog( const char *pcIdent, int iOption, int iFacility )
{
#ifndef _WIN32
	openlog(pcIdent,iOption,iFacility);
#else // _WIN32
	s_pcSvcName = pcIdent;
#endif // !_WIN32
}


//! modify by Uman, 2010/11/28
void FV_POWER_API FvSysLog( int iPriority, const char *pcFormat, ... )
{
#ifndef _WIN32
	va_list ap;
	va_start( ap, pcFormat );
	syslog(iPriority,pcFormat,ap);
	va_end( ap )
#else // _WIN32
	HANDLE hHandle;
	va_list kArgList;
	static char s_acMsgBuf[2048];

	hHandle = RegisterEventSource(NULL,
		s_pcSvcName);
	if (hHandle == NULL)
		return;

	va_start( kArgList, pcFormat );
	vsnprintf_s(s_acMsgBuf, sizeof(s_acMsgBuf), _TRUNCATE, pcFormat, kArgList);
	va_end( kArgList );

	puts(s_acMsgBuf);

	OutputDebugString( s_acMsgBuf );

	if (!ReportEvent(hHandle,     
		iPriority,
		0,
		iPriority,
		NULL,
		1,
		0,
		(LPCSTR*)&s_acMsgBuf,
		NULL))
	{
		// DO ERROR
	}

	DeregisterEventSource(hHandle);
#endif // !_WIN32
}
/** old code
void FV_POWER_API FvSysLog( int iPriority, const char *pcFormat, ... )
{
#ifndef _WIN32
	va_list ap;
	va_start( ap, pcFormat );
	syslog(iPriority,pcFormat,ap);
	va_end( ap )
#else // _WIN32
	HANDLE hHandle;
	va_list kArgList;
	int iMsgSize;
	char *pcMsg = malloc(MAX_MES_SIZE);

	hHandle = RegisterEventSource(NULL,
		s_pcSvcName);
	if (hHandle == NULL)
		return;

	va_start( kArgList, pcFormat );
	iMsgSize = _vscprintf( pcFormat, kArgList ) + 1;
	va_end( kArgList );
	va_start( kArgList, pcFormat );
	_vsnprintf_s( pcMsg, iMsgSize, MAX_MES_SIZE, pcFormat, kArgList );
	va_end( kArgList );

	puts(pcMsg);

	OutputDebugString( pcMsg );

	if (!ReportEvent(hHandle,     
		iPriority,
		0,
		iPriority,
		NULL,
		1,
		0,
		(LPCSTR*)&pcMsg,
		NULL))
	{
		// DO ERROR
	}

	DeregisterEventSource(hHandle);
	free(pcMsg);
#endif // !_WIN32
}
**/