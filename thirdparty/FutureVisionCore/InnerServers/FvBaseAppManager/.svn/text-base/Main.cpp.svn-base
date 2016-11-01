#include "FvBaseAppManager.h"
#include "FvBaseAppManagerInterface.h"
#include <FvNetLoggerMessageForwarder.h>
#include <FvNetPortMap.h>
#include <FvXMLSection.h>
#include <FvServerConfig.h>
#include <FvFileLog.h>
//#include <FvObj.h>
//#include <FvDataDescription.h>


FV_DECLARE_DEBUG_COMPONENT( 0 )

#ifdef _WIN32
#include <signal.h>
void ServiceStop()
{
    //raise( SIGINT );
	if(FvBaseAppMgr::pInstance())
		FvBaseAppMgr::Instance().ShutDown(false);
}

char szServiceDependencies[] = "cellappmgr";
#endif

#include <FvService.h>

int doMain( FvNetNub &kNub, int iArgc, char *pcArgv[] )
{
	FvBaseAppMgr baseAppMgr( kNub );

	FV_SERVICE_CHECK_POINT( 3000 );


	if (!baseAppMgr.Init( iArgc, pcArgv ))
	{
		FV_ERROR_MSG( "main: init failed.\n" );
		return 1;
	}

	FV_INFO_MSG( "---- BaseAppMgr is running ----\n" );

	FV_SERVICE_UPDATE_STATUS( SERVICE_RUNNING, 0, 0 );

	baseAppMgr.Nub().ProcessUntilBreak();

	return 0;
}

int FUTUREVISION_SERVICE_MAIN( int iArgc, char *pcArgv[] )
{
	FV_WRITE_TO_CONSOLE(baseAppMgr);
	FV_OPEN_LOG(baseAppMgr);
	FvNetNub kNub(FV_MACHINE_PORT, FV_USER_ID, 0, FV_INTERNAL_INTERFACE(baseAppMgr));

	if(kNub.GetUserID()==0 || kNub.GetUserID()==0xFFFF)
	{
		FV_ERROR_MSG( "UserID:%d is Invalid\n", kNub.GetUserID());
		goto ERR;
	}

	//FvNetNub kNub( 0, "baseAppMgr" );
	//FV_NET_MESSAGE_FORWARDER( FvBaseAppMgr, baseAppMgr, kNub );
	START_MSG( "BaseAppMgr" );

	int iResult = doMain( kNub, iArgc, pcArgv );

	FV_INFO_MSG( "BaseAppMgr has shut down.\n" );

ERR:
	FV_CLOSE_LOG();

	FvDebugMsgHelper::Fini();

	return iResult;
}

