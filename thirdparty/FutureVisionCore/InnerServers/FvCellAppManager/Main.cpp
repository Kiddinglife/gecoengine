#include "FvCellAppManager.h"
#include "FvCellAppManagerInterface.h"
#include <FvNetLoggerMessageForwarder.h>
#include <FvNetPortMap.h>
#include <FvXMLSection.h>
#include <FvServerConfig.h>
#include <FvObj.h>
#include <FvDataDescription.h>
#include <FvFileLog.h>


FV_DECLARE_DEBUG_COMPONENT( 0 )

FvUInt16 GetUserID( int iArgc, char *pcArgv[] )
{
	FvUInt16 uiUserID = FV_USER_ID;

	for (int i = 1; i < iArgc -1; ++i)
	{
		if (_stricmp( pcArgv[i], "-UserID" ) == 0)
		{
			uiUserID = (FvUInt16)atoi( pcArgv[ i + 1 ] );
		}
	}

	return uiUserID;
}

#ifdef _WIN32
#include <signal.h>
void ServiceStop()
{
    //raise( SIGINT );
	if(FvCellAppMgr::pInstance())
		FvCellAppMgr::Instance().ShutDown(false);
}

char szServiceDependencies[] = "machined";
#endif

#include <FvService.h>

int doMain( FvNetNub &kNub, int iArgc, char *pcArgv[] )
{
	::StampsPerSecond();

	FvCellAppMgr cellAppMgr( kNub );

	FV_SERVICE_CHECK_POINT( 3000 );


	if (!cellAppMgr.Init( iArgc, pcArgv ))
	{
		FV_ERROR_MSG( "main: init failed.\n" );
		return 1;
	}

	FV_INFO_MSG( "---- CellAppMgr is running ----\n" );

	FV_SERVICE_UPDATE_STATUS( SERVICE_RUNNING, 0, 0 );

	cellAppMgr.Nub().ProcessUntilBreak();

	return 0;
}

int FUTUREVISION_SERVICE_MAIN( int iArgc, char *pcArgv[] )
{
	FV_WRITE_TO_CONSOLE(cellAppMgr);
	FV_OPEN_LOG(cellAppMgr);

	FvUInt16 uiUserID = GetUserID(iArgc, pcArgv);
	FvNetNub kNub(FV_MACHINE_PORT, uiUserID, 0, FV_INTERNAL_INTERFACE(cellAppMgr));

	if(kNub.GetUserID()==0xFFFF)
	{
		FV_ERROR_MSG( "UserID:%d is Invalid\n", kNub.GetUserID());
		goto ERR;
	}

	//FvNetNub kNub( 0, "cellAppMgr" );
	//FV_NET_MESSAGE_FORWARDER( FvCellAppMgr, cellAppMgr, kNub );
	START_MSG_WITH_USERID( "CellAppMgr", uiUserID );

	int iResult = doMain( kNub, iArgc, pcArgv );

	FV_INFO_MSG( "CellAppMgr has shut down.\n" );

ERR:
	FV_CLOSE_LOG();

	FvDebugMsgHelper::Fini();

	return iResult;
}

