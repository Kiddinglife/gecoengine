#include "FvReviver.h"

#include <FvDebug.h>
#include <FvNetBundle.h>
#include <FvNetLoggerMessageForwarder.h>
#include <FvServerResource.h>
#include <FvServerConfig.h>

FV_DECLARE_DEBUG_COMPONENT2( "Reviver", 0 )

#ifdef _WIN32
void ServiceStop()
{
	FvReviver * pReviver = FvReviver::pInstance();

	if (pReviver != NULL)
	{
		pReviver->ShutDown();
	}
}

char szServiceDependencies[] = "machined";
#endif

#include <FvService.h>

void printHelp( const char * commandName )
{
	printf( "\n\n" );
	printf( "Usage: %s [OPTION]\n", commandName );
	printf(
"Monitors FutureVision server components and spawns a new process if a component\n"
"fails.\n"
"\n"
"  --add {BaseAppManager|CellAppManager|DBManager|LoginApp}\n"
"  --del {BaseAppManager|CellAppManager|DBManager|LoginApp}\n"
"\n" );

	printf(
"For example, the following monitors the DBManager process and starts a new\n"
"instance if that one fails.\n"
"  %s --add dbMgr\n\n",
	 commandName );
}

int doMain( FvNetNub & nub, int argc, char * argv[] )
{
	FvReviver reviver( nub );

	if (!reviver.Init( argc, argv ))
	{
		FV_ERROR_MSG( "Failed to initialise the reviver\n" );
		return 1;
	}

	FV_SERVICE_UPDATE_STATUS( SERVICE_RUNNING, 0, 0 );

	reviver.Run();

	return 0;
}

int FUTUREVISION_SERVICE_MAIN( int argc, char * argv[] )
{
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--help" ) == 0)
		{
			printHelp( argv[0] );
			return 0;
		}
	}

	FvNetNub nub( 0, FV_INTERNAL_INTERFACE( reviver ) );
	FV_NET_MESSAGE_FORWARDER( FvReviver, reviver, nub );
	START_MSG( "Reviver" );

	int result = doMain( nub, argc, argv );

	FV_INFO_MSG( "Reviver has shut down.\n" );

	return result;
}