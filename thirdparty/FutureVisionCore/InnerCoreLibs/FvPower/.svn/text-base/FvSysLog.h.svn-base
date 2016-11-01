//{future header message}
#ifndef __FvSysLog_H__
#define __FvSysLog_H__

#include "FvPowerDefines.h"

#ifndef _WIN32
#include <syslog.h>
#include <sys/types.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef _WIN32
	#define FV_SYSLOG_EMERG       0
	#define FV_SYSLOG_ALERT       1
	#define FV_SYSLOG_CRIT        2
	#define FV_SYSLOG_ERR         3
	#define FV_SYSLOG_WARNING     4
	#define FV_SYSLOG_NOTICE      5
	#define FV_SYSLOG_INFO        6
	#define FV_SYSLOG_DEBUG       7
#else // !_WIN32

	#define FV_SYSLOG_EMERG       EVENTLOG_SUCCESS
	#define FV_SYSLOG_ALERT       EVENTLOG_ERROR_TYPE
	#define FV_SYSLOG_CRIT        EVENTLOG_ERROR_TYPE
	#define FV_SYSLOG_ERR         EVENTLOG_ERROR_TYPE
	#define FV_SYSLOG_WARNING     EVENTLOG_WARNING_TYPE
	#define FV_SYSLOG_NOTICE      EVENTLOG_WARNING_TYPE
	#define FV_SYSLOG_INFO        EVENTLOG_INFORMATION_TYPE
	#define FV_SYSLOG_DEBUG       EVENTLOG_INFORMATION_TYPE
#endif // _WIN32

	#define FV_SYSLOG_KERN        (0<<3)
	#define FV_SYSLOG_USER        (1<<3)
	#define FV_SYSLOG_MAIL        (2<<3)
	#define FV_SYSLOG_DAEMON      (3<<3)
	#define FV_SYSLOG_AUTH        (4<<3)
	#define FV_SYSLOG_SYSLOG      (5<<3)
	#define FV_SYSLOG_LPR         (6<<3)
	#define FV_SYSLOG_NEWS        (7<<3)
	#define FV_SYSLOG_UUCP        (8<<3)
	#define FV_SYSLOG_CRON        (9<<3)
	#define FV_SYSLOG_AUTHPRIV    (10<<3)
	#define FV_SYSLOG_FTP         (11<<3)

	void FV_POWER_API FvOpenLog( const char *pcIdent, int iOption, int iFacility );
	void FV_POWER_API FvSysLog( int iPriority, const char *pcFormat, ... );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __FvSysLog_H__