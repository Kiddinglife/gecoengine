#include <FvPrintf.h>
#include <FvWatcher.h>
#include "FvServerCommon.h"

void FV_SERVERCOMMON_API ServiceParseCommandLine( int argc, char **argv )
{
	FV_WATCH( "debug/shouldWriteToConsole", g_iShouldWriteToConsole );

	for (int i=1; i < argc; i++)
	{
		if (strcmp( argv[i], "-machined" ) == 0)
		{
			g_iShouldWriteToConsole = 0;
		}
	}
}