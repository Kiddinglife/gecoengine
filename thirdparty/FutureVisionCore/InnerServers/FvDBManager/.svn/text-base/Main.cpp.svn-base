#include "FvDBInterface.h"
#include "FvDatabase.h"
#include <FvDebug.h>
#include <FvObj.h>
#include <FvDataDescription.h>
#include <FvNetLoggerMessageForwarder.h>
#include <FvServerConfig.h>
#include <FvNetPortMap.h>
#include <FvFileLog.h>
#ifndef FV_SHIPPING
#include <vld.h>
#endif // !FV_SHIPPING


FV_DECLARE_DEBUG_COMPONENT( 0 )

#ifdef _WIN32  // WIN32PORT
void ServiceStop()
{
	FvDatabase * pDB = FvDatabase::pInstance();

	if (pDB != NULL)
	{
		pDB->ShutDown();
	}
}

char szServiceDependencies[] = "machined";
#endif

#include <FvService.h>

int doMain( FvNetNub &kNub, int iArgc, char *pcArgv[] )
{
	bool bIsUpgrade = false;
	bool bIsSyncTablesToDefs = false;
	for (int i=1; i<iArgc; i++)
	{
		if ((0 == strcmp( pcArgv[i], "--upgrade" )) ||
					(0 == strcmp( pcArgv[i], "-upgrade" )))
		{
			bIsUpgrade = true;
		}
		else if ((0 == strcmp( pcArgv[i], "--sync-tables-to-defs" )) ||
					(0 == strcmp( pcArgv[i], "-syncTablesToDefs" )))
		{
			bIsSyncTablesToDefs = true;
		}
	}

	FvDatabase kDatabase( kNub );

	FvDatabase::InitResult result =
		kDatabase.Init( bIsUpgrade, bIsSyncTablesToDefs );
	switch (result)
	{
		case FvDatabase::InitResultFailure:
			FV_ERROR_MSG( "Failed to initialise the database\n" );
			return 1;
		case FvDatabase::InitResultSuccess:
			FV_SERVICE_UPDATE_STATUS( SERVICE_RUNNING, 0, 0 );
			kDatabase.Run();
			break;
		case FvDatabase::InitResultAutoShutdown:
			kDatabase.Finalise();
			break;
		default:
			FV_ASSERT(false);
			break;
	}

	return 0;
}

int FUTUREVISION_SERVICE_MAIN( int iArgc, char *pcArgv[] )
{
	FV_WRITE_TO_CONSOLE(DBManager);
	FV_OPEN_LOG(DBManager);
	FvNetNub kNub(FV_MACHINE_PORT, FV_USER_ID, 0, FV_INTERNAL_INTERFACE(DBManager));

	if(kNub.GetUserID()==0 || kNub.GetUserID()==0xFFFF)
	{
		FV_ERROR_MSG( "UserID:%d is Invalid\n", kNub.GetUserID());
		goto ERR;
	}

	//FvNetNub kNub( 0, "DBManager" );
	//FV_NET_MESSAGE_FORWARDER( DBManager, DBManager, kNub );
	START_MSG( "DBManager" );

	int iResult = doMain( kNub, iArgc, pcArgv );

	FV_INFO_MSG( "DBManager has shut down.\n" );

ERR:
	FV_CLOSE_LOG();

	FvDebugMsgHelper::Fini();

	return iResult;
}
