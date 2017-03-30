//{future header message}
#ifndef __GECO_SYS_LOG_H__
#define __GECO_SYS_LOG_H__

#include "geco-defines.h"

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
#define GECO_SYSLOG_EMERG       0
#define GECO_SYSLOG_ALERT       1
#define GECO_SYSLOG_CRIT        2
#define GECO_SYSLOG_ERR         3
#define GECO_SYSLOG_WARNING     4
#define GECO_SYSLOG_NOTICE      5
#define GECO_SYSLOG_INFO        6
#define GECO_SYSLOG_DEBUG       7
#else // !_WIN32

#define GECO_SYSLOG_EMERG       EVENTLOG_SUCCESS
#define GECO_SYSLOG_ALERT       EVENTLOG_ERROR_TYPE
#define GECO_SYSLOG_CRIT        EVENTLOG_ERROR_TYPE
#define GECO_SYSLOG_ERR         EVENTLOG_ERROR_TYPE
#define GECO_SYSLOG_WARNING     EVENTLOG_WARNING_TYPE
#define GECO_SYSLOG_NOTICE      EVENTLOG_WARNING_TYPE
#define GECO_SYSLOG_INFO        EVENTLOG_INFORMATION_TYPE
#define GECO_SYSLOG_DEBUG       EVENTLOG_INFORMATION_TYPE
#endif // _WIN32

#define GECO_SYSLOG_KERN        (0<<3)
#define GECO_SYSLOG_USER        (1<<3)
#define GECO_SYSLOG_MAIL        (2<<3)
#define GECO_SYSLOG_DAEMON      (3<<3)
#define GECO_SYSLOG_AUTH        (4<<3)
#define GECO_SYSLOG_SYSLOG      (5<<3)
#define GECO_SYSLOG_LPR         (6<<3)
#define GECO_SYSLOG_NEWS        (7<<3)
#define GECO_SYSLOG_UUCP        (8<<3)
#define GECO_SYSLOG_CRON        (9<<3)
#define GECO_SYSLOG_AUTHPRIV    (10<<3)
#define GECO_SYSLOG_FTP         (11<<3)

	void GECO_API geco_open_syslog(const char *pcIdent, int iOption, int iFacility);
	void GECO_API geco_syslog(int iPriority, const char *pcFormat, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __GECO_SYS_LOG_H__