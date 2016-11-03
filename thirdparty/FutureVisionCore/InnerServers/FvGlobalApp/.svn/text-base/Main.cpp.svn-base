#include <../FvGlobal/FvGlobalEntityManager.h>
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


#define LOGICDLLNAME	"GlobalScript.dll"


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

	FvEntityManager kGlobalApp(kNub);
	FvEntityManager* p = FvEntityManager::pInstance();

	if (!kGlobalApp.Init( iArgc, pcArgv ))
	{
		FV_ERROR_MSG( "main: init failed.\n" );
		return 1;
	}

	FV_INFO_MSG( "---- GlobalApp is running ----\n" );

	do
	{
		try
		{
			kGlobalApp.Nub().ProcessContinuously();
			break;
		}
		catch (FvNetNubException & ne)
		{
			FvNetAddress addr = FvNetAddress::NONE;
			bool hasAddr = ne.GetAddress( addr );

			if (hasAddr)
			{
				char buf[ 256 ];
				_snprintf( buf, sizeof( buf ),
					"processContinuously( %s )", addr.c_str() );

				kGlobalApp.Nub().ReportException( ne , buf );
			}
			else
			{
				kGlobalApp.Nub().ReportException(
					ne, "processContinuously" );
			}
		}
	}
	while (!kGlobalApp.Nub().ProcessingBroken());

	kGlobalApp.Nub().ReportPendingExceptions( true /* reportBelowThreshold */ );

	return 0;
}

int FUTUREVISION_SERVICE_MAIN( int iArgc, char *pcArgv[] )
{
	FV_WRITE_TO_CONSOLE(GlobalApp);
	FV_OPEN_LOG(GlobalApp);

	FvUInt16 uiUserID = 0;
	FvNetNub kNub(FV_MACHINE_PORT, uiUserID, 0, FV_INTERNAL_INTERFACE(GlobalApp));

	new FvLogicDllManager();
	if(!FvLogicDllManager::Instance().LoadDll(LOGICDLLNAME))
	{
		FV_ERROR_MSG( "main: Load dll(%s) failed.\n", LOGICDLLNAME);
		goto ERR;
	}

	//FV_NET_MESSAGE_FORWARDER( FvGlobalApp, baseApp, kNub );
	START_MSG_WITH_USERID( "GlobalApp", uiUserID );

	int iResult = doMain( kNub, iArgc, pcArgv );

	//{
	//	std::string result;
	//	std::string desc;
	//	FvWatcher::Mode mode;
	//	FvWatcher::RootWatcher().GetAsString(NULL, "network/unackedPacketAllocator/numInPoolUsed", result, desc, mode);
	//}

	FV_INFO_MSG( "GlobalApp has shut down.\n" );

ERR:
	if(FvLogicDllManager::pInstance())
		delete FvLogicDllManager::pInstance();

	FvDataType::ClearStaticsForReload();
	FvMetaDataType::Fini();

	FV_CLOSE_LOG();

	FvDebugMsgHelper::Fini();

	return iResult;
}



