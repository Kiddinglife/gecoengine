#include <../FvCell/FvCellEntityManager.h>
#include <FvTimestamp.h>
#include <FvNetLoggerMessageForwarder.h>
#include <FvXMLSection.h>
#include <FvServerConfig.h>
#include <FvNetWatcherGlue.h>
#include <FvNetPortMap.h>
#include <FvLogicDllManager.h>
#include <FvObj.h>
#include <FvDataDescription.h>
#include <FvFileLog.h>

#define LOGICDLLNAME	"CellScript.dll"


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
	raise( SIGINT );
}

char szServiceDependencies[] = "machined";
#endif

#include <FvService.h>

int doMain( FvNetNub &kNub, int iArgc, char *pcArgv[] )
{
	FvEntityManager cellApp( kNub );

	StampsPerSecond();

	if (!cellApp.Init( iArgc, pcArgv ))
	{
		FV_ERROR_MSG( "main: init failed.\n" );
		return 1;
	}

	FV_INFO_MSG( "---- CellApp is running ----\n" );

	do
	{
		try
		{
			cellApp.Nub().ProcessContinuously();
			break;
		}
		catch (FvNetNubException & ne)
		{
			FvNetAddress addr = FvNetAddress::NONE;
			bool hasAddr = ne.GetAddress( addr );

			//switch (ne.Reason())
			//{
			//default:
				if (hasAddr)
				{
					char buf[ 256 ];
					_snprintf( buf, sizeof( buf ),
						"processContinuously( %s )", addr.c_str() );

					cellApp.Nub().ReportException( ne , buf );
				}
				else
				{
					cellApp.Nub().ReportException(
						ne, "processContinuously" );
				}
				break;
			//}
		}
	}
	while (!cellApp.Nub().ProcessingBroken());

	cellApp.Nub().ReportPendingExceptions( true /* reportBelowThreshold */ );
	cellApp.Nub().ReportPendingExceptions( true /* reportBelowThreshold */ );

	return 0;
}

int FUTUREVISION_SERVICE_MAIN( int iArgc, char *pcArgv[] )
{
	FV_WRITE_TO_CONSOLE(cellApp);
	FV_OPEN_LOG(cellApp);

	FvUInt16 uiUserID = GetUserID(iArgc, pcArgv);
	FvNetNub kNub(FV_MACHINE_PORT, uiUserID, 0, FV_INTERNAL_INTERFACE(cellApp));

	if(kNub.GetUserID()==0xFFFF)
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

	//FvNetNub kNub( 0, "cellApp" );
	//FV_NET_MESSAGE_FORWARDER( FvCellApp, cellApp, kNub );
	START_MSG_WITH_USERID( "CellApp", uiUserID );

	int iResult = doMain( kNub, iArgc, pcArgv );

	//{
	//	std::string result;
	//	std::string desc;
	//	FvWatcher::Mode mode;
	//	FvWatcher::RootWatcher().GetAsString(NULL, "network/unackedPacketAllocator/numInPoolUsed", result, desc, mode);
	//}

	FV_INFO_MSG( "CellApp has shut down.\n" );

ERR:
	if(FvLogicDllManager::pInstance())
		delete FvLogicDllManager::pInstance();

	FvDataType::ClearStaticsForReload();
	FvMetaDataType::Fini();

	FV_CLOSE_LOG();

	FvDebugMsgHelper::Fini();

	return iResult;
}



