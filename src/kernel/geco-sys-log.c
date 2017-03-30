#include "geco-sys-log.h"
#include <stdio.h>

const char *s_pcSvcName = NULL;
#define MAX_MES_SIZE 1024

void GECO_API geco_open_syslog(const char *pcIdent, int iOption, int iFacility)
{
#ifndef _WIN32
	openlog(pcIdent, iOption, iFacility);
#else // _WIN32
	s_pcSvcName = pcIdent;
#endif // !_WIN32
}

void GECO_API geco_syslog(int iPriority, const char *pcFormat, ...)
{
#ifndef _WIN32
	va_list ap;
	va_start(ap, pcFormat);
	syslog(iPriority, pcFormat, ap);
	va_end(ap)
#else // _WIN32
	HANDLE hHandle;
	va_list kArgList;
	static char s_acMsgBuf[2048];

	hHandle = RegisterEventSource(NULL,
		s_pcSvcName);
	if (hHandle == NULL)
		return;

	va_start(kArgList, pcFormat);
	vsnprintf_s(s_acMsgBuf, sizeof(s_acMsgBuf), _TRUNCATE, pcFormat, kArgList);
	va_end(kArgList);

	puts(s_acMsgBuf);

	OutputDebugString(s_acMsgBuf);

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