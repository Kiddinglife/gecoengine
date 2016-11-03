#include "FvLoginApp.h"

#include <FvPowerDefines.h>
#include <FvDebug.h>
#include <FvNetLoggerMessageForwarder.h>
#include <FvNetPortMap.h>
#include <FvServerConfig.h>
#include <FvFileLog.h>
#ifndef FV_SHIPPING
#include <vld.h>
#endif // ! FV_SHIPPING

FV_DECLARE_DEBUG_COMPONENT( 0 )

static unsigned short LoginPort( int iArgc, char *pcArgv[] )
{
	FvUInt16 loginPort = FvServerConfig::Get( "LoginApp/port", FV_NET_PORT_LOGIN );

	for (int i = 1; i < iArgc - 1; ++i)
	{
		if (strcmp( pcArgv[i], "-loginPort" ) == 0)
		{
			loginPort = atoi( pcArgv[ i + 1 ] );
		}
	}

	return loginPort;
}

#ifdef _WIN32 
#include <signal.h>
void ServiceStop()
{
	//raise( SIGINT );
	if(FvLoginApp::pInstance())
		FvLoginApp::Instance().ControlledShutDown();
}

char szServiceDependencies[] = "machined";
#endif // _WIN32

#include <FvService.h>

int doMain( FvNetNub &kNub, int iArgc, char *pcArgv[] )
{
	FvUInt16 uiLoginPort = ::LoginPort( iArgc, pcArgv );
	FvLoginApp kLoginApp( kNub, uiLoginPort );

	if (!kLoginApp.Init( iArgc, pcArgv, uiLoginPort ))
	{
		FV_ERROR_MSG( "Failed to initialise Login App\n" );
		return 1;
	}

	FV_INFO_MSG( "---- LoginApp is running ----\n" );

	if (!kLoginApp.IsDBReady())
	{
		FV_INFO_MSG( "Database is not ready yet\n" );
	}

	kLoginApp.Run();

	return 0;
}


int FUTUREVISION_SERVICE_MAIN( int iArgc, char *pcArgv[] )
{
	FV_WRITE_TO_CONSOLE(LoginApp);
	FV_OPEN_LOG(LoginApp);
	FvNetNub kNub(FV_MACHINE_PORT, FV_USER_ID, 0, FV_INTERNAL_INTERFACE(LoginApp));

	if(kNub.GetUserID()==0 || kNub.GetUserID()==0xFFFF)
	{
		FV_ERROR_MSG( "UserID:%d is Invalid\n", kNub.GetUserID());
		goto ERR;
	}

	//FvNetNub kNub( 0, "LoginApp" );
	//FV_NET_MESSAGE_FORWARDER( LoginApp, LoginApp, kNub );
	START_MSG( "LoginApp" );

	int iResult = doMain( kNub, iArgc, pcArgv );

	FV_INFO_MSG( "LoginApp has shut down.\n" );

ERR:
	FV_CLOSE_LOG();

	FvDebugMsgHelper::Fini();

	return iResult;
}