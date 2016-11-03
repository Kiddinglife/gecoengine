#if _WIN32
#include "FvWindowsMachineGuard.h"
#else // _WIN32
#include "FvLinuxMachineGuard.h"
#endif // unix

#include "FvMachined.h"

#ifdef _WIN32
char szServiceDependencies[] = "+TDI~+Network~";
void ServiceStop()
{
	if(FvMachined::pInstance())
	{
		return FvMachined::Instance().Stop();
	}
}
#endif // _WIN32

#include <FvSysLog.h>
#include <FvService.h>
#include <FvNetPortMap.h>
#ifndef FV_SHIPPING
#include <vld.h>
#endif // !FV_SHIPPING

static char s_pcUsage[] =
	"Usage: %s [args]\n"
	"-f/--foreground   Run machined in the foreground (i.e. not as a daemon)\n";

int FUTUREVISION_SERVICE_MAIN_NO_RESMGR( int iArgc, char *pcArgv[] )
{
	bool bDaemon = true;
	for (int i = 1; i < iArgc; ++i)
	{
		if (!strcmp( pcArgv[i], "-f" ) || !strcmp( pcArgv[i], "--foreground" ))
			bDaemon = false;

		else if (strcmp( pcArgv[i], "--help" ) == 0)
		{
			printf( s_pcUsage, pcArgv[0] );
			return 0;
		}

		else
		{
			fprintf( stderr, "Invalid argument: '%s'\n", pcArgv[i] );
			return 1;
		}
	}

	FvOpenLog( pcArgv[0], 0, FV_SYSLOG_DAEMON );

	FvMachined kMachined;

	InitProcessState( bDaemon );
	srand( (int)Timestamp() );

#ifndef _WIN32 
	rlimit rlimitData = { RLIM_INFINITY, RLIM_INFINITY };
	setrlimit( RLIMIT_CORE, &rlimitData );
#endif // !_WIN32

	FV_SERVICE_UPDATE_STATUS( SERVICE_RUNNING, 0, 0 );

	if (FvMachined::pInstance())
		kMachined.Run();

	return 1;
}