#include <../FvBase/FvBaseEntityManager.h>
#include <FvTimestamp.h>
#include <FvNetLoggerMessageForwarder.h>
#include <FvXMLSection.h>
#include <FvServerConfig.h>
#include <FvNetWatcherGlue.h>
#include <FvLogicDllManager.h>
#include <FvNetPortMap.h>
#include <FvFileLog.h>
#include <FvObj.h>
#include <FvDataDescription.h>


#define LOGICDLLNAME	"BaseScript.dll"


FV_DECLARE_DEBUG_COMPONENT( 0 )

#ifdef _WIN32
#include <signal.h>
void ServiceStop()
{
	raise( SIGINT );
}

char szServiceDependencies[] = "machined";
#endif

#include <FvService.h>


int doMain( FvNetNub &kNub, int iArgc, char *pcArgv[] )
{
	StampsPerSecond();

	FvUInt16 uiExternalPort = FvServerConfig::Get( "baseApp/externalPort", FvUInt16(0) );

	FvEntityManager baseApp( kNub, uiExternalPort );

	if (!baseApp.Init( iArgc, pcArgv, uiExternalPort ))
	{
		FV_ERROR_MSG( "main: init failed.\n" );
		return 1;
	}

	FV_INFO_MSG( "---- BaseApp is running ----\n" );

	do
	{
		try
		{
			baseApp.IntNub().ProcessContinuously();
			break;
		}
		catch (FvNetNubException & ne)
		{
			FvNetAddress addr = FvNetAddress::NONE;
			bool hasAddr = ne.GetAddress( addr );

			switch (ne.Reason())
			{
				case FV_NET_REASON_INACTIVITY:
				case FV_NET_REASON_NO_SUCH_PORT:
					baseApp.AddressDead( addr, ne.Reason() );
					break;
				default:
					if (hasAddr)
					{
						char buf[ 256 ];
						_snprintf( buf, sizeof( buf ),
							"processContinuously( %s )", addr.c_str() );

						baseApp.IntNub().ReportException( ne , buf );
					}
					else
					{
						baseApp.IntNub().ReportException(
							ne, "processContinuously" );
					}
					break;
			}
		}
	}
	while (!baseApp.IntNub().ProcessingBroken());

	baseApp.IntNub().ReportPendingExceptions( true /* reportBelowThreshold */ );
	baseApp.ExtNub().ReportPendingExceptions( true /* reportBelowThreshold */ );

	return 0;
}

int FUTUREVISION_SERVICE_MAIN( int iArgc, char *pcArgv[] )
{
	FV_WRITE_TO_CONSOLE(baseApp);
	FV_OPEN_LOG(baseApp);
	FvNetNub kNub(FV_MACHINE_PORT, FV_USER_ID, 0, FV_INTERNAL_INTERFACE(baseApp));

	if(kNub.GetUserID()==0 || kNub.GetUserID()==0xFFFF)
	{
		FV_ERROR_MSG( "UserID:%d is Invalid\n", kNub.GetUserID());
		goto ERR;
	}

	new FvLogicDllManager();
	if(!FvLogicDllManager::Instance().LoadDll(LOGICDLLNAME))
	{
		FV_ERROR_MSG( "main: Load dll(%s) failed.\n", LOGICDLLNAME);
		goto ERR;
	}

	//FvNetNub kNub( 0, "baseApp" );
	//FV_NET_MESSAGE_FORWARDER( FvBaseApp, baseApp, kNub );
	START_MSG( "BaseApp" );

	int iResult = doMain( kNub, iArgc, pcArgv );

	//{
	//	std::string result;
	//	std::string desc;
	//	FvWatcher::Mode mode;
	//	FvWatcher::RootWatcher().GetAsString(NULL, "network/unackedPacketAllocator/numInPoolUsed", result, desc, mode);
	//}

	FV_INFO_MSG( "BaseApp has shut down.\n" );

ERR:
	if(FvLogicDllManager::pInstance())
		delete FvLogicDllManager::pInstance();

	FvDataType::ClearStaticsForReload();
	FvMetaDataType::Fini();

	FV_CLOSE_LOG();

	FvDebugMsgHelper::Fini();

	return iResult;
}



