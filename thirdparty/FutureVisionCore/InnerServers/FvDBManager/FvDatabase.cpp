#include "FvDatabase.h"
#include "FvDBConfig.h"
#include "FvDBEntitydefs.h"
#include "FvDBInterface.h"
#include "FvDBInterfaceUtils.h"
#include "FvDBEntityRecoverer.h"

#include <../FvBaseAppManager/FvBaseAppManagerInterface.h>
#include <../FvBase/FvBaseAppIntInterface.h>

#include <FvMemoryStream.h>

#include <FvServerResource.h>

#include <FvEntitydefConstants.h>

#include <FvProcess.h>
#include <FvServerConfig.h>
#include <FvReviverSubject.h>
#include <FvWriteDB.h>

#include <FvNetWatcherGlue.h>
#include <FvNetPortMap.h>
#include <FvNetBlockingReplyHandler.h>

#ifdef FV_USE_ORACLE
#include "FvOracleDatabase.h"
#endif

#ifdef FV_USE_MYSQL
#include "FvMysqlDatabase.h"
#endif

#ifdef FV_USE_XML
#include "FvXMLDatabase.h"
#endif

#include <signal.h>
//#include <sys/wait.h>
#include <time.h>
//#include <pwd.h>

#include "FvDBCustom.h"

FV_DECLARE_DEBUG_COMPONENT( 0 )

FV_SINGLETON_STORAGE( FvDatabase )

namespace
{

const char * DEFAULT_ENTITY_TYPE_STR = "Avatar";
const char * DEFAULT_NAME_PROPERTY_STR = "playerName";
const char * UNSPECIFIED_ERROR_STR = "Unspecified error";

#define CONSOLIDATE_DBS_FILENAME_STR 	"consolidate_dbs"
#define CONSOLIDATE_DBS_RELPATH_STR 	"commands/"CONSOLIDATE_DBS_FILENAME_STR
const int CONSOLIDATE_DBS_EXEC_FAILED_EXIT_CODE = 100;

} 

//namespace
//{
//
//FvString getOurExePath()
//{
//	pid_t pid = getpid();
//
//	char linkPath[64];
//	snprintf( linkPath, sizeof(linkPath), "/proc/%i/exe", pid );
//
//	char fullExePath[1024];
//	int ret = readlink( linkPath, fullExePath, sizeof(fullExePath) );
//
//	if ((ret == -1) || (ret >= int(sizeof(fullExePath))))
//	{
//		return FvString();
//	}
//
//	return FvString( fullExePath, ret );
//}
//
//FvString getOurExeDir()
//{
//	FvString ourExePath = getOurExePath();
//	FvString::size_type slashPos = ourExePath.find_last_of( '/' );
//	return FvString( ourExePath, 0, slashPos );
//}
//
//}

namespace
{

static void signalHandler( int sigNum )
{
	FvDatabase * pDB = FvDatabase::pInstance();

	if (pDB != NULL)
	{
		pDB->OnSignalled( sigNum );
	}
}

bool commandShutDown( FvString & output, FvString & value )
{
	FvDatabase * pDB = FvDatabase::pInstance();
	if (pDB != NULL)
	{
		pDB->ShutDown();
	}

	return true;
}

}

FvDatabase::FvDatabase( FvNetNub & nub ) :
	m_kNub( nub ),
	m_kWorkerThreadManager( m_kNub ),
	m_pkEntityDefs( NULL ),
	m_pkDatabase( NULL ),
	//signals_(),
	m_kStatus(),
	m_kBaseAppManager( m_kNub ),
	m_bShouldLoadUnknown( true ),
	m_bShouldCreateUnknown( true ),
	m_bShouldRememberUnknown( true ),
	m_spServerConfig(),
	m_bAllowEmptyDigest( true ),
	m_bShouldSendInitData( false ),
	m_bShouldConsolidate( true ),
	m_uiDesiredBaseApps( 1 ),
	m_uiDesiredCellApps( 1 ),
	m_kStatusCheckTimerID( FV_NET_TIMER_ID_NONE),
	m_bClearRecoveryDataOnStartUp( true ),
	m_kWriteEntityTimer( 5 ),
	m_fCurLoad( 1.f ),
	m_fMaxLoad( 1.f ),
	m_bAnyCellAppOverloaded( true ),
	m_uiOverloadStartTime( 0 ),
	m_iMailboxRemapCheckCount( 0 ),
	m_kSecondaryDBPrefix(),
	m_uiSecondaryDBIndex( 0 ),
	//consolidatePid_( 0 ),
	m_bIsProduction( FvServerConfig::Get( "production", false ) )
{
	m_kBaseAppManager.Channel().IsIrregular( true );
}

FvDatabase::~FvDatabase()
{
	delete m_pkDatabase;
	delete m_pkEntityDefs;
	FvDataType::ClearStaticsForReload();
	m_pkDatabase = NULL;
}

bool ReplaceEnvirVariableInStr(FvString& kStr)
{
	char acSrcStr[256] = {0};
	strcpy_s(acSrcStr, sizeof(acSrcStr), kStr.c_str());
	char* pcFirst = strchr(acSrcStr, '%');
	if(pcFirst)
	{
		char* pcSecond = strchr(pcFirst+1, '%');
		if(pcSecond)
		{
			if(pcFirst+1 == pcSecond)
			{
				//Log::Error("[%s]使用了空环境变量", acSrcStr);
				return false;
			}

			char acEnvirVar[256] = {0};
			memcpy_s(acEnvirVar, sizeof(acEnvirVar), pcFirst+1, pcSecond-pcFirst-1);
			char acEnvirVal[256] = {0};
			if(!GetEnvironmentVariable(acEnvirVar, acEnvirVal, sizeof(acEnvirVal)))
			{
				//Log::Error("找不到[%s]中的环境变量[%s]", acSrcStr, acEnvirVar);
				return false;
			}

			*pcFirst = 0;
			kStr = "";
			kStr = kStr + acSrcStr + acEnvirVal + (pcSecond+1);
		}
	}
	return true;
}

FvDatabase::InitResult FvDatabase::Init( bool isUpgrade, bool isSyncTablesToDefs )
{
	if (m_kNub.Socket() == -1)
	{
		FV_ERROR_MSG( "FvDatabase::Init: Failed to create Nub on internal "
			"interface.\n");
		return InitResultFailure;
	}

	if (m_bIsProduction)
	{
		FV_INFO_MSG( "FvDatabase::Init: Production mode enabled.\n" );
	}

	FvReviverSubject::Instance().Init( &m_kNub, "DBManager" );

	//if (!Script::init( EntityDef::Constants::databasePath(), "database" ))
	//{
	//	return InitResultFailure;
	//}

	FvString defaultTypeName = DEFAULT_ENTITY_TYPE_STR;
	FvString nameProperty;

	int dumpLevel = 0;

	FvServerConfig::Update( "DBManager/allowEmptyDigest", m_bAllowEmptyDigest );
	if ( (m_bAllowEmptyDigest) && (m_bIsProduction) )
	{
		FV_ERROR_MSG( "FvDatabase::Init: Production Mode: Allowing client "
			"connections with empty entity definition digests! This is "
			"a potential security risk.\n" );
	}
	FvServerConfig::Update( "DBManager/loadUnknown", m_bShouldLoadUnknown );
	FvServerConfig::Update( "DBManager/createUnknown", m_bShouldCreateUnknown );
	FvServerConfig::Update( "DBManager/rememberUnknown", m_bShouldRememberUnknown );

	FvServerConfig::Update( "DBManager/entityType", defaultTypeName );
	FvServerConfig::Update( "DBManager/nameProperty", nameProperty );

	m_spServerConfig.reset( new FvDBConfigServer() );

	if (nameProperty.empty())
	{
		nameProperty = DEFAULT_NAME_PROPERTY_STR;
	}
	else
	{
		FV_INFO_MSG( "DBManager/nameProperty has been deprecated. Please add the "
					"attribute <Identifier> true </Identifier> to the name "
					"property of the entity\n" );
	}

	FvServerConfig::Update( "DBManager/dumpEntityDescription", dumpLevel );

	FvServerConfig::Update( "desiredBaseApps", m_uiDesiredBaseApps );
	FvServerConfig::Update( "desiredCellApps", m_uiDesiredCellApps );

	FvServerConfig::Update( "DBManager/clearRecoveryData", m_bClearRecoveryDataOnStartUp );

	FvServerConfig::Update( "DBManager/overloadLevel", m_fMaxLoad );
	m_uiAllowOverloadPeriod = FvUInt64( StampsPerSecondD() *
			FvServerConfig::Get( "DBManager/overloadTolerancePeriod", 5.f ) );

	//PyOutputWriter::overrideSysMembers(
	//	FvServerConfig::Get( "DBManager/writePythonLog", false ) );

	//time_t epochTime = time( NULL );
	//tm timeAndDate;
	//localtime_r( &epochTime, &timeAndDate );

	//uid_t 		uid = getuid();
	//passwd* 	pUserDetail = getpwuid( uid );
	//FvString username;
	//if (pUserDetail)
	//{
	//	username = pUserDetail->pw_name;
	//}
	//else
	//{
	//	FV_WARNING_MSG( "FvDatabase::Init: Using '%hd' as the username due to uid "
	//			"to name lookup failure\n", uid );
	//	std::stringstream ss;
	//	ss << uid;
	//	username = ss.str();
	//}

	//FV_INIT_WATCHER_DOC( "DBManager" );

	//FV_WATCH( "allowEmptyDigest",	m_bAllowEmptyDigest );
	//FV_WATCH( "createUnknown",		m_bShouldCreateUnknown );
	//FV_WATCH( "rememberUnknown",	m_bShouldRememberUnknown );
	//FV_WATCH( "loadUnknown",		m_bShouldLoadUnknown );
	//FV_WATCH( "IsReady", *this, &FvDatabase::IsReady );
	//m_kStatus.RegisterWatchers();

	//FV_WATCH( "HasStartBegun",
	//		*this, FV_ACCESSORS( bool, FvDatabase, HasStartBegun ) );

	//FV_WATCH( "desiredBaseApps", m_uiDesiredBaseApps );
	//FV_WATCH( "desiredCellApps", m_uiDesiredCellApps );

	//FV_WATCH( "clearRecoveryDataOnStartUp", m_bClearRecoveryDataOnStartUp );

	//FV_WATCH( "performance/WriteEntity/performance", m_kWriteEntityTimer,
	//	FvWatcher::WT_READ_ONLY );
	//FV_WATCH( "performance/WriteEntity/rate", m_kWriteEntityTimer,
	//	&FvNetNub::TransientMiniTimer::GetCountPerSec );
	//FV_WATCH( "performance/WriteEntity/duration", (FvNetNub::MiniTimer&) m_kWriteEntityTimer,
	//	&FvNetNub::MiniTimer::GetAvgDurationSecs );

	//FV_WATCH( "load", m_fCurLoad, FvWatcher::WT_READ_ONLY );
	//FV_WATCH( "overloadLevel", m_fMaxLoad );

	//FV_WATCH( "anyCellAppOverloaded", m_bAnyCellAppOverloaded );

	//FvWatcher::RootWatcher().AddChild( "command/shutDown",
	//		new FvNoArgCallableWatcher( commandShutDown,
	//			FvCallableWatcher::LOCAL_ONLY,
	//			"Shuts down DBManager" ) );

	FvXMLSectionPtr spSection =
		FvServerResource::OpenSection( FvEntityDef::Constants::EntitiesFile() );

	if (!spSection)
	{
		FV_ERROR_MSG( "FvDatabase::Init: Failed to open "
				"<res>/%s\n", FvEntityDef::Constants::EntitiesFile() );
		return InitResultFailure;
	}

	m_kStatus.Set( FvDBStatus::STARTING, "Loading entity definitions" );

	FvXMLSectionPtr spOpts = FvServerResource::OpenSection("EntityDef/options.xml");
	extern FV_ENTITYDEF_API FvString g_kFatherDefsPath;
	g_kFatherDefsPath = spOpts->ReadString("FatherEntityDefPath");
	//! 替换环境变量
	if(!ReplaceEnvirVariableInStr(g_kFatherDefsPath))
	{
		FV_ERROR_MSG( "FvDatabase::Init: Failed to replace envir variable in FatherEntityDefPath[%s]\n", g_kFatherDefsPath.c_str() );
		return InitResultFailure;
	}
	FvXMLSectionPtr spFatherEntitiesFile;
	if(!g_kFatherDefsPath.empty())
	{
		FvString kFullPath = g_kFatherDefsPath + "/entities.xml";

		FILE *pkSrcFile;
		int iError;
		iError = fopen_s(&pkSrcFile, kFullPath.c_str(), "rb");
		if(iError == 0)
		{
			Ogre::DataStreamPtr spDataStream(OGRE_NEW Ogre::FileHandleDataStream(pkSrcFile));
			spFatherEntitiesFile = FvXMLSection::OpenSection(spDataStream);
			fclose(pkSrcFile);
		}
	}

	m_pkEntityDefs = new FvEntityDefs();
	if ( !m_pkEntityDefs->Init(spFatherEntitiesFile, spSection, defaultTypeName, nameProperty ) )
	{
		return InitResultFailure;
	}

	if(!m_pkEntityDefs->XMLToMD5())
	{
		return InitResultFailure;
	}

	if ( !m_pkEntityDefs->IsValidEntityType( m_pkEntityDefs->GetDefaultType() ) &&
	     (m_bShouldLoadUnknown || m_bShouldCreateUnknown) )
	{
		FV_ERROR_MSG( "FvDatabase::Init: Invalid DBManager/entityType '%s'. "
				"Consider changing DBManager/entityType in FutureVision.xml\n",
				defaultTypeName.c_str() );
		return InitResultFailure;
	}

	m_pkEntityDefs->DebugDump( dumpLevel );

	FV_REGISTER_WATCHER( 0, "DBManager", "DBManager", "DBManager", m_kNub );

	//FvWatcher::RootWatcher().AddChild( "nub", FvNetNub::GetWatcher(),
	//	&this->m_kNub );

	FvString databaseType = FvServerConfig::Get( "DBManager/type", "xml" );

#ifdef FV_USE_XML
	if (databaseType == "xml")
	{
		m_pkDatabase = new XMLDatabase();

		m_bShouldConsolidate = false;
	} else
#endif
#ifdef FV_USE_ORACLE
	if (databaseType == "oracle")
	{
		char * oracle_home = getenv( "ORACLE_HOME" );

		if (oracle_home == NULL)
		{
			FV_INFO_MSG( "ORACLE_HOME not set. Setting to /home/local/oracle\n" );
			putenv( "ORACLE_HOME=/home/local/oracle" );
		}

		m_pkDatabase = new OracleDatabase();
	}
	else
#endif
#ifdef FV_USE_MYSQL
	if (databaseType == "mysql")
	{
		// m_pkDatabase = new MySqlDatabase();
		m_pkDatabase = MySqlDatabase::Create();

		if (m_pkDatabase == NULL)
		{
			return InitResultFailure;
		}
	}
	else
#endif
	{
		FV_ERROR_MSG("Unknown database type: %s\n", databaseType.c_str());
#ifndef FV_USE_MYSQL
		if (databaseType == "mysql")
		{
			FV_INFO_MSG( "DBManager needs to be rebuilt with MySQL support. See "
					"the Server Installation Guide for more information\n" );
		}
#endif
		return InitResultFailure;
	}

	FV_INFO_MSG( "\tDatabase layer      = %s\n", databaseType.c_str() );
	if ((databaseType == "xml") && (m_bIsProduction))
	{
		FV_ERROR_MSG(
			"The XML database is suitable for demonstrations and "
			"evaluations only.\n"
			"Please use the MySQL database for serious development and "
			"production systems.\n"
			"See the Server Operations Guide for instructions on how to switch "
		   	"to the MySQL database.\n" );
	}

	m_kStatus.Set( FvDBStatus::STARTING, "Initialising database layer" );

	bool isRecover = false;

	if (isUpgrade || isSyncTablesToDefs)
	{
		if (!m_pkDatabase->Startup( this->GetEntityDefs(),
									isRecover, isUpgrade, isSyncTablesToDefs ))
		{
			return InitResultFailure;
		}

		return InitResultAutoShutdown;
	}

	//signal( SIGCHLD, ::signalHandler );
	signal( SIGINT, ::signalHandler );
#ifndef _WIN32  // WIN32PORT
	signal( SIGHUP, ::signalHandler );
#endif // _WIN32  // WIN32PORT

	{
		m_kNub.RegisterBirthListener( DBInterface::HandleBaseAppMgrBirth,
									"BaseAppMgrInterface" );

		FvNetAddress baseAppMgrAddr;
		FvNetReason reason =
			m_kNub.FindInterface( "BaseAppMgrInterface", 0, baseAppMgrAddr );

		if (reason == FV_NET_REASON_SUCCESS)
		{
			m_kBaseAppManager.Addr( baseAppMgrAddr );

			FV_INFO_MSG( "FvDatabase::Init: BaseAppMgr at %s\n",
				m_kBaseAppManager.c_str() );
		}
		else if (reason == FV_NET_REASON_TIMER_EXPIRED)
		{
			FV_INFO_MSG( "FvDatabase::Init: BaseAppMgr is not ready yet.\n" );
		}
		else
		{
			FV_CRITICAL_MSG("FvDatabase::Init: "
				"findInterface( BaseAppMgrInterface ) failed! (%s)\n",
				NetReasonToString( (FvNetReason)reason ) );

			return InitResultFailure;
		}
	}

	DBInterface::RegisterWithNub( m_kNub );

	FvNetReason reason =
		DBInterface::RegisterWithMachined( m_kNub, 0 );

	if (reason != FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG( "FvDatabase::Init: Unable to register with nub. "
				"Is machined running?\n" );
		return InitResultFailure;
	}

	m_kNub.RegisterBirthListener( DBInterface::HandleDatabaseBirth,
								"DBInterface" );

	if (m_kBaseAppManager.Addr() != FvNetAddress::NONE)
	{
		FvNetBlockingReplyHandlerWithResult <bool> handler( m_kNub );
		FvNetBundle & bundle = m_kBaseAppManager.Bundle();

		bundle.StartRequest( BaseAppMgrInterface::RequestHasStarted, &handler );
		m_kBaseAppManager.Send();

		if ( handler.WaitForReply( & m_kBaseAppManager.Channel() ) ==
				FV_NET_REASON_SUCCESS )
		{
			isRecover = handler.get();
		}

		m_bShouldSendInitData = !isRecover;
	}

	if (!m_pkDatabase->Startup( this->GetEntityDefs(),
								isRecover, isUpgrade, isSyncTablesToDefs ))
	{
		return InitResultFailure;
	}

	//if (m_bShouldConsolidate)
	//{
	//	char runIDBuf[ BUFSIZ ];
	//	snprintf( runIDBuf, sizeof(runIDBuf),
	//				"%s_%04d-%02d-%02d_%02d:%02d:%02d",
	//				username.c_str(),
	//				timeAndDate.tm_year + 1900, timeAndDate.tm_mon + 1,
	//				timeAndDate.tm_mday,
	//				timeAndDate.tm_hour, timeAndDate.tm_min, timeAndDate.tm_sec );
	//	m_kSecondaryDBPrefix = runIDBuf;
	//}

	FV_INFO_MSG( "FvDatabase::Init: m_kSecondaryDBPrefix = \"%s\"\n",
			m_kSecondaryDBPrefix.c_str() );


	if (isRecover)
	{
		this->StartServerBegin( true );
	}
	else
	{
		if (m_bShouldConsolidate)
		{
			this->ConsolidateData();
		}
		else
		{
			m_kStatus.Set( FvDBStatus::WAITING_FOR_APPS, "Waiting for other "
					"components to become ready" );
		}
	}

	m_kStatusCheckTimerID = m_kNub.RegisterTimer( 1000000, this );

	#ifdef FV_DBMGR_SELFTEST
		this->RunSelfTest();
	#endif

	FV_INFO_MSG( "\tNub address         = %s\n", (char *)m_kNub.Address() );
	FV_INFO_MSG( "\tAllow empty digest  = %s\n",
		m_bAllowEmptyDigest ? "True" : "False" );
	FV_INFO_MSG( "\tLoad unknown user = %s\n",
		m_bShouldLoadUnknown ? "True" : "False" );
	FV_INFO_MSG( "\tCreate unknown user = %s\n",
		m_bShouldCreateUnknown ? "True" : "False" );
	FV_INFO_MSG( "\tRemember unknown user = %s\n",
		m_bShouldRememberUnknown ? "True" : "False" );
	FV_INFO_MSG( "\tRecover database = %s\n",
		isRecover ? "True" : "False" );
	FV_INFO_MSG( "\tClear recovery data = %s\n",
		m_bClearRecoveryDataOnStartUp ? "True" : "False" );

	return InitResultSuccess;
}

void FvDatabase::Run()
{
	FV_INFO_MSG( "---- DBManager is running ----\n" );
	do
	{
		m_kNub.ProcessUntilBreak();
	} while (/*!signals_.isClear() && */this->ProcessSignals());

	this->Finalise();
}

void FvDatabase::Finalise()
{
	if (m_pkDatabase)
	{
		m_pkDatabase->ShutDown();
	}
}

void FvDatabase::OnSignalled( int sigNum )
{
	//signals_.set( sigNum );

	m_kNub.BreakProcessing();
}

bool FvDatabase::ProcessSignals()
{
	//static Signal::Set allSignals( Signal::Set::FULL );

	bool shouldContinueProcess = true;
	//do
	//{
	//	{
	//		Signal::Blocker signalBlocker( allSignals );

	//		m_kNub.breakProcessing( false );

	//		if (signals_.isSet( SIGCHLD ))
	//		{
	//			int		status;
	//			pid_t 	childPid = waitpid( -1, &status, WNOHANG );
	//			if (childPid != 0)
	//			{
	//				FvDatabase::Instance().onChildProcessExit( childPid, status );
	//			}
	//		}

	//		if (signals_.isSet( SIGINT ) || signals_.isSet( SIGHUP ))
	//		{
	//			this->ShutDown();
	//		}

	//		signals_.clear();

	//		shouldContinueProcess = !m_kNub.processingBroken();
	//	}
	//} while (shouldContinueProcess && m_kNub.processingBroken());

	//return shouldContinueProcess;
	return false;
}

//void FvDatabase::onChildProcessExit( pid_t pid, int status )
//{
//	//FV_ASSERT( pid == this->consolidatePid_ );
//
//	bool isOK = true;
//	if (WIFEXITED( status ))
//	{
//		int exitCode = WEXITSTATUS( status );
//		if (exitCode != 0)
//		{
//			if (exitCode == CONSOLIDATE_DBS_EXEC_FAILED_EXIT_CODE)
//			{
//				FvString fullPath = getOurExeDir() +
//										"/"CONSOLIDATE_DBS_RELPATH_STR;
//				FV_ERROR_MSG( "FvDatabase::onChildProcessExit: "
//							"Failed to execute %s.\n"
//							"Please ensure that the "
//							CONSOLIDATE_DBS_FILENAME_STR " executable exists "
//							"and is runnable. You may need to build it "
//							"manually as it not part of the standard "
//							"package.\n",
//						fullPath.c_str() );
//			}
//			else
//			{
//				FV_ERROR_MSG( "FvDatabase::onChildProcessExit: "
//						"Consolidate process exited with code %d\n"
//						"Please examine the logs for ConsolidateDBs or run "
//						CONSOLIDATE_DBS_FILENAME_STR " manually to determine "
//						"the cause of the error\n",
//						exitCode );
//			}
//			isOK = false;
//		}
//	}
//	else if (WIFSIGNALED( status ))
//	{
//		FV_ERROR_MSG( "FvDatabase::onChildProcessExit: "
//				"Consolidate process was terminated by signal %d\n",
//				int( WTERMSIG( status ) ) );
//		isOK = false;
//	}
//
//	if (isOK)
//	{
//		FV_TRACE_MSG( "Finished data consolidation\n" );
//	}
//
//	//this->consolidatePid_ = 0;
//
//	while (!m_pkDatabase->lockDB())
//	{
//		FV_WARNING_MSG( "FvDatabase::onChildProcessExit: "
//				"Failed to re-lock database. Retrying...\n" );
//		sleep(1);
//	}
//
//	this->OnConsolidateProcessEnd( isOK );
//}

void FvDatabase::ConsolidateData()
{
	if (m_kStatus.GetStatus() <= FvDBStatus::STARTING)
	{
		m_kStatus.Set( FvDBStatus::STARTUP_CONSOLIDATING, "Consolidating data" );
	}
	else if ( m_kStatus.GetStatus() >= FvDBStatus::SHUTTING_DOWN)
	{
		m_kStatus.Set( FvDBStatus::SHUTDOWN_CONSOLIDATING, "Consolidating data" );
	}
	else
	{
		FV_CRITICAL_MSG( "FvDatabase::ConsolidateData: Not a valid state to be "
				"running data consolidation!" );
		return;
	}

	FvUInt32 numSecondaryDBs = m_pkDatabase->GetNumSecondaryDBs();
	if (numSecondaryDBs > 0)
	{
		FV_TRACE_MSG( "Starting data consolidation\n" );
		this->StartConsolidationProcess();
	}
	else
	{
		this->OnConsolidateProcessEnd( true );
	}
}

bool FvDatabase::StartConsolidationProcess()
{
	//if (this->IsConsolidating())
	//{
	//	FV_TRACE_MSG( "FvDatabase::startConsolidationProcess: Ignoring second "
	//			"attempt to consolidate data while data consolidation is "
	//			"already in progress\n" );
	//	return false;
	//}

	//m_pkDatabase->UnlockDB();

	//std::vector< FvString > cmdArgs;

	//{
	//	int numPaths = BWResource::getPathNum();
	//	if (numPaths > 0)
	//	{
	//		cmdArgs.push_back( "--res" );

	//		std::stringstream ss;
	//		ss << BWResource::getPath( 0 );
	//		for (int i = 1; i < numPaths; ++i)
	//		{
	//			ss << BW_RES_PATH_SEPARATOR << BWResource::getPath( i );
	//		}
	//		cmdArgs.push_back( ss.str() );
	//	}
	//}

	//if ((consolidatePid_ = fork()) == 0)
	//{
	//	// Find path
	//	FvString path = getOurExeDir();

	//	// Change to it
	//	if (chdir( path.c_str() ) == -1)
	//	{
	//		printf( "Failed to change directory to %s\n", path.c_str() );
	//		exit(1);
	//	}

	//	// Add the exe name
	//	path += "/"CONSOLIDATE_DBS_RELPATH_STR;

	//	// Close parent sockets
	//	close( this->m_kNub.socket() );

	//	// Make arguments into const char * array.
	//	const char ** argv = new const char *[cmdArgs.size() + 2];
	//	int i = 0;
	//	argv[i++] = path.c_str();
	//	for (std::vector< FvString >::iterator pCurArg = cmdArgs.begin();
	//			pCurArg != cmdArgs.end(); ++pCurArg, ++i)
	//	{
	//		argv[i] = pCurArg->c_str();
	//	}
	//	argv[i] = NULL;

	//	// __kyl__ (21/4/2008) This is a rather worrying const cast. If execv()
	//	// modifies any of the arguments, we're screwed since we're pointing
	//	// to constant strings.
	//	int result = execv( path.c_str(),
	//			const_cast< char * const * >( argv ) );

	//	if (result == -1)
	//	{
	//		exit( CONSOLIDATE_DBS_EXEC_FAILED_EXIT_CODE );
	//	}

	//	exit(1);
	//}

	return true;
}

void FvDatabase::OnConsolidateProcessEnd( bool isOK )
{
	if (m_kStatus.GetStatus() == FvDBStatus::STARTUP_CONSOLIDATING)
	{
		if (isOK)
		{
			m_kStatus.Set( FvDBStatus::WAITING_FOR_APPS, "Waiting for other "
					"components to become ready" );
		}
		else
		{
			m_bShouldConsolidate = false;

			this->StartSystemControlledShutdown();
		}
	}
	else if (m_kStatus.GetStatus() == FvDBStatus::SHUTDOWN_CONSOLIDATING)
	{
		this->ShutDown();
	}
	else
	{
		FV_CRITICAL_MSG( "FvDatabase::onChildProcessExit: Invalid state %d at "
				"the end of data consolidation\n", m_kStatus.GetStatus() );
	}
}

class LoginAppCheckStatusReplyHandler : public FvNetReplyMessageHandler
{
public:
	LoginAppCheckStatusReplyHandler( const FvNetAddress & srcAddr,
			FvNetReplyID replyID ) :
		m_kSrcAddr( srcAddr ),
		m_iReplyID ( replyID )
	{
	}

private:
	virtual void HandleMessage( const FvNetAddress & /*srcAddr*/,
			FvNetUnpackedMessageHeader & /*header*/,
			FvBinaryIStream & data, void * /*arg*/ )
	{
		FvNetChannelSender sender( FvDatabase::GetChannel( m_kSrcAddr ) );
		FvNetBundle & bundle = sender.Bundle();

		bundle.StartReply( m_iReplyID );

		bool isOkay;
		FvInt32 numBaseApps;
		FvInt32 numCellApps;

		data >> isOkay >> numBaseApps >> numCellApps;

		bundle << FvUInt8( isOkay && (numBaseApps > 0) && (numCellApps > 0));

		bundle.Transfer( data, data.RemainingLength() );

		if (numBaseApps <= 0)
		{
			bundle << "No BaseApps";
		}

		if (numBaseApps <= 0)
		{
			bundle << "No CellApps";
		}

		delete this;
	}

	virtual void HandleException( const FvNetNubException & /*ne*/,
		void * /*arg*/ )
	{
		FvNetChannelSender sender( FvDatabase::GetChannel( m_kSrcAddr ) );
		FvNetBundle & bundle = sender.Bundle();

		bundle.StartReply( m_iReplyID );
		bundle << FvUInt8( false );
		bundle << "No reply from BaseAppMgr";

		delete this;
	}

	FvNetAddress m_kSrcAddr;
	FvNetReplyID m_iReplyID;
};

void FvDatabase::CheckStatus( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
{
	FvNetChannelSender sender(
		FvDatabase::Instance().GetBaseAppManager().Channel() );

	sender.Bundle().StartRequest( BaseAppMgrInterface::CheckStatus,
		   new LoginAppCheckStatusReplyHandler( srcAddr, header.m_iReplyID ) );
}

void FvDatabase::HandleStatusCheck( FvBinaryIStream & data )
{
	bool isOkay;
	FvUInt32 numBaseApps = 0;
	FvUInt32 numCellApps = 0;
	data >> isOkay >> numBaseApps >> numCellApps;
	FV_INFO_MSG( "FvDatabase::HandleStatusCheck: bOk:%d "
				"baseApps = %u/%u. cellApps = %u/%u\n",
			  isOkay,
			  std::max( FvUInt32(0), numBaseApps ), m_uiDesiredBaseApps,
			  std::max( FvUInt32(0), numCellApps ), m_uiDesiredCellApps );

	data.Finish();

	if ((m_kStatus.GetStatus() <= FvDBStatus::WAITING_FOR_APPS) &&
			isOkay &&//! add by Uman, 20100425
			!data.Error() &&
			(numBaseApps >= m_uiDesiredBaseApps) &&
			(numCellApps >= m_uiDesiredCellApps))
	{
		this->StartServerBegin();
	}
}

class CheckStatusReplyHandler : public FvNetReplyMessageHandler
{
private:
	virtual void HandleMessage( const FvNetAddress & /*srcAddr*/,
			FvNetUnpackedMessageHeader & /*header*/,
			FvBinaryIStream & data, void * /*arg*/ )
	{
		FvDatabase::Instance().HandleStatusCheck( data );
		delete this;
	}

	virtual void HandleException( const FvNetNubException & /*ne*/,
		void * /*arg*/ )
	{
		delete this;
	}
};

void FvDatabase::SecondaryDBRegistration( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FvIDatabase::SecondaryDBEntry	secondaryDBEntry;

	data >> secondaryDBEntry.m_kAddr >> secondaryDBEntry.m_iAppID;

	data >> secondaryDBEntry.m_kLocation;
	m_pkDatabase->AddSecondaryDB( secondaryDBEntry );
}

void FvDatabase::UpdateSecondaryDBs( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FvUInt32 size;
	data >> size;

	FvBaseAppIDs ids;

	for (FvUInt32 i = 0; i < size; ++i)
	{
		int id;
		data >> id;
		ids.push_back( id );
	}

	m_pkDatabase->UpdateSecondaryDBs( ids, *this );
}

void FvDatabase::GetSecondaryDBDetails( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FvNetChannelSender sender( FvDatabase::GetChannel( srcAddr ) );
	FvNetBundle & bundle = sender.Bundle();
	bundle.StartReply( header.m_iReplyID );

	if (!m_kSecondaryDBPrefix.empty())
	{
		char buf[ BUFSIZ ];
		++m_uiSecondaryDBIndex;
		FvSNPrintf( buf, sizeof( buf ), "%s-%d.db",
				m_kSecondaryDBPrefix.c_str(),
				m_uiSecondaryDBIndex );

		bundle << buf;
	}
	else
	{
		bundle << "";
	}
}

void FvDatabase::OnUpdateSecondaryDBsComplete(
		const FvIDatabase::SecondaryDBEntries& removedEntries )
{
	for (FvIDatabase::SecondaryDBEntries::const_iterator pEntry =
			removedEntries.begin(); pEntry != removedEntries.end(); ++pEntry )
	{
		if (this->SendRemoveDBCmd( pEntry->m_kAddr.m_uiIP, pEntry->m_kLocation ))
		{
			FV_TRACE_MSG( "FvDatabase::OnUpdateSecondaryDBsComplete: "
					"Deleting secondary database file %s on %s\n",
					pEntry->m_kLocation.c_str(), pEntry->m_kAddr.IPAsString() );
		}
		else
		{
			FV_ERROR_MSG( "FvDatabase::OnUpdateSecondaryDBsComplete: Failed to "
					"delete secondary database file %s on %s. It should be "
					"manually deleted to prevent disk space exhaustion.\n",
					pEntry->m_kLocation.c_str(), pEntry->m_kAddr.IPAsString() );
		}
	}
}

bool FvDatabase::SendRemoveDBCmd( FvUInt32 destIP, const FvString& dbLocation )
{
	FvNetCreateWithArgsMessage cm;
	cm.m_uiUID = m_kNub.GetUserID();
#if defined FV_DEBUG
#if defined FV_AS_STATIC_LIBS
	cm.m_kConfig = "DebugLib";
#else
	cm.m_kConfig = "DebugDll";
#endif // FV_AS_STATIC_LIBS
#elif defined FV_RELEASE
#if defined FV_AS_STATIC_LIBS
	cm.m_kConfig = "ReleaseLib";
#else
	cm.m_kConfig = "ReleaseDll";
#endif // FV_AS_STATIC_LIBS
#else 
	cm.m_kConfig = "";
#endif

	cm.m_uiRecover = 0;
	cm.m_kName = "commands/remove_db";
	cm.m_uiFWDIP = 0;
	cm.m_uiFWDPort = 0;

	cm.m_kArgs.push_back( dbLocation );

	FvNetEndpoint ep;
	ep.Socket( SOCK_DGRAM );

	return (ep.Good() && (ep.Bind() == 0) &&
			cm.SendTo( ep, htons( FV_MACHINE_PORT ), destIP ));
}

int FvDatabase::HandleTimeout( FvNetTimerID id, void * arg )
{
	if (m_bShouldSendInitData)
	{
		m_bShouldSendInitData = false;
		this->SendInitData();
	}

	if (m_kBaseAppManager.Channel().IsEstablished() &&
			(m_kStatus.GetStatus() == FvDBStatus::WAITING_FOR_APPS))
	{
		FvNetBundle & bundle = m_kBaseAppManager.Bundle();

		bundle.StartRequest( BaseAppMgrInterface::CheckStatus,
			   new CheckStatusReplyHandler() );

		m_kBaseAppManager.Send();

		m_kNub.ClearSpareTime();
	}

	if (m_kStatus.GetStatus() > FvDBStatus::WAITING_FOR_APPS)
	{
		FvUInt64 spareTime = m_kNub.GetSpareTime();
		m_kNub.ClearSpareTime();

		m_fCurLoad = 1.f - float( double(spareTime) / StampsPerSecondD() );
	}

	if (--m_iMailboxRemapCheckCount == 0)
		this->EndMailboxRemapping();

	return 0;
}


void FvDatabase::HandleBaseAppMgrBirth(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);
	m_kBaseAppManager.Addr( kAddr );

	FV_INFO_MSG( "FvDatabase::HandleBaseAppMgrBirth: BaseAppMgr is at %s\n",
		m_kBaseAppManager.c_str() );

	if (m_kStatus.GetStatus() < FvDBStatus::SHUTTING_DOWN)
	{
		m_bShouldSendInitData = true;
	}
}

void FvDatabase::HandleDatabaseBirth(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);
	if (kAddr != m_kNub.Address())
	{
		FV_WARNING_MSG( "FvDatabase::HandleDatabaseBirth: %s\n", kAddr.c_str() );
		this->ShutDown();
	}
}

void FvDatabase::ShutDown( DBInterface::ShutDownArgs & /*args*/ )
{
	this->ShutDown();
}

void FvDatabase::StartSystemControlledShutdown()
{
	if (m_kBaseAppManager.Channel().IsEstablished())
	{
		BaseAppMgrInterface::ControlledShutDownArgs args;
		args.stage = SHUTDOWN_TRIGGER;
		args.shutDownTime = 0;
		m_kBaseAppManager.Bundle() << args;
		m_kBaseAppManager.Send();
	}
	else
	{
		FV_WARNING_MSG( "FvDatabase::StartSystemControlledShutdown: "
				"No known BaseAppMgr, only shutting down self\n" );
		this->ShutDownNicely();
	}
}

void FvDatabase::ShutDownNicely()
{
	if (m_kStatus.GetStatus() >= FvDBStatus::SHUTTING_DOWN)
	{
		FV_WARNING_MSG( "FvDatabase::ShutDownNicely: Ignoring second shutdown\n" );
		return;
	}

	FV_TRACE_MSG( "FvDatabase::ShutDownNicely: Shutting down\n" );

	m_kStatus.Set( FvDBStatus::SHUTTING_DOWN, "Shutting down" );

	m_kNub.ProcessUntilChannelsEmpty();

	if (m_bShouldConsolidate)
	{
		this->ConsolidateData();
	}
	else
	{
		this->ShutDown();
	}
}

void FvDatabase::ShutDown()
{
	FV_TRACE_MSG( "FvDatabase::ShutDown\n" );

	//if (consolidatePid_ != 0)
	//{
	//	FV_WARNING_MSG( "FvDatabase::ShutDown: Stopping ongoing consolidation "
	//			"process %d\n", consolidatePid_ );
	//	::kill( consolidatePid_, SIGINT );
	//}

	m_kNub.BreakProcessing();
}

void FvDatabase::ControlledShutDown( DBInterface::ControlledShutDownArgs & args )
{
	FV_DEBUG_MSG( "FvDatabase::ControlledShutDown: stage = %d\n", args.stage );

	switch (args.stage)
	{
	case SHUTDOWN_REQUEST:
	{
		m_kNub.PrepareToShutDown();

		if (m_kBaseAppManager.Channel().IsEstablished())
		{
			BaseAppMgrInterface::ControlledShutDownArgs args;
			args.stage = SHUTDOWN_REQUEST;
			args.shutDownTime = 0;
			m_kBaseAppManager.Bundle() << args;
			m_kBaseAppManager.Send();
		}
		else
		{
			FV_WARNING_MSG( "FvDatabase::ControlledShutDown: "
					"No BaseAppMgr. Proceeding to shutdown immediately\n" );
			this->ShutDownNicely();
		}
	}
	break;

	case SHUTDOWN_PERFORM:
		this->ShutDownNicely();
		break;

	default:
		FV_ERROR_MSG( "FvDatabase::ControlledShutDown: Stage %d not handled.\n",
				args.stage );
		break;
	}
}

void FvDatabase::CellAppOverloadStatus(
	DBInterface::CellAppOverloadStatusArgs & args )
{
	m_bAnyCellAppOverloaded = args.anyOverloaded;
}

void FvDatabase::GetEntityHandler::OnGetEntityComplete( bool isOK )
{
	if (FvDatabase::Instance().HasMailboxRemapping() &&
			this->OutRec().IsBaseMBProvided() &&
			this->OutRec().GetBaseMB())
	{
		FvDatabase::Instance().RemapMailbox( *this->OutRec().GetBaseMB() );
	}

	this->OnGetEntityCompleted( isOK );
}

void FvDatabase::GetEntity( GetEntityHandler& handler )
{
	m_pkDatabase->GetEntity( handler );
}

void FvDatabase::PutEntity( const FvEntityDBKey& ekey, FvEntityDBRecordIn& erec,
		FvIDatabase::IPutEntityHandler& handler )
{
	if (this->HasMailboxRemapping() && erec.IsBaseMBProvided() &&
			erec.GetBaseMB())
		this->RemapMailbox( *erec.GetBaseMB() );

	m_pkDatabase->PutEntity( ekey, erec, handler );
}

FV_INLINE void FvDatabase::DelEntity( const FvEntityDBKey & ekey,
		FvIDatabase::IDelEntityHandler& handler )
{
	m_pkDatabase->DelEntity( ekey, handler );
}

FV_INLINE void FvDatabase::SetLoginMapping( const FvString & username,
		const FvString & password, const FvEntityDBKey& ekey,
		FvIDatabase::ISetLoginMappingHandler& handler )
{
	m_pkDatabase->SetLoginMapping( username, password, ekey, handler );
}

class FvLoginHandler : public FvNetReplyMessageHandler,
                     public FvIDatabase::IMapLoginToEntityDBKeyHandler,
					 public FvIDatabase::ISetLoginMappingHandler,
                     public FvDatabase::GetEntityHandler,
                     public FvIDatabase::IPutEntityHandler
{
	enum State
	{
		StateInit,
		StateWaitingForLoadUnknown,
		StateWaitingForLoad,
		StateWaitingForPutNewEntity,
		StateWaitingForSetLoginMappingForLoadUnknown,
		StateWaitingForSetLoginMappingForCreateUnknown,
		StateWaitingForSetBaseToLoggingOn,
		StateWaitingForSetBaseToFinal,
	};

public:
	FvLoginHandler(
			LogOnParamsPtr pParams,
			const FvNetAddress& clientAddr,
			const FvNetAddress & replyAddr,
			bool offChannel,
			FvNetReplyID replyID ) :
		m_eState( StateInit ),
		m_kEntityDBKey( 0, 0 ),
		m_spParams( pParams ),
		m_kClientAddr( clientAddr ),
		m_kReplyAddr( replyAddr ),
		m_bOffChannel( offChannel ),
		m_iReplyID( replyID ),
		m_pkStrmDbID( 0 )
	{
	}
	virtual ~FvLoginHandler() {}

	void Login();

	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data, void * arg );

	virtual void HandleException( const FvNetNubException & ne,
		void * arg );

	virtual void OnMapLoginToEntityDBKeyComplete( FvDatabaseLoginStatus status,
		const FvEntityDBKey& ekey );

	virtual void OnSetLoginMappingComplete();

	virtual FvEntityDBKey& GetKey()					{	return m_kEntityDBKey;	}
	virtual FvEntityDBRecordOut& OutRec()			{	return m_kOutRec;	}

	virtual const FvString * GetPasswordOverride() const
	{
		return FvDatabase::Instance().GetEntityDefs().
			EntityTypeHasPassword( m_kEntityDBKey.m_uiTypeID ) ? &m_spParams->Password() : 0;
	}

	virtual void OnGetEntityCompleted( bool isOK );

	virtual void OnPutEntityComplete( bool isOK, FvDatabaseID dbID );

private:
	void HandleFailure( FvBinaryIStream * pData, FvDatabaseLoginStatus reason );
	void CheckOutEntity();
	void CreateNewEntity( bool isBundlePrepared = false );
	void SendCreateEntityMsg();
	void SendReply();
	void SendFailureReply( FvDatabaseLoginStatus status, const char * msg = NULL );

	State m_eState;
	FvEntityDBKey m_kEntityDBKey;
	LogOnParamsPtr m_spParams;
	FvNetAddress m_kClientAddr;
	FvNetAddress m_kReplyAddr;
	bool m_bOffChannel;
	FvNetReplyID m_iReplyID;
	FvNetBundle m_kBundle;
	FvEntityMailBoxRef m_kBaseRef;
	FvEntityMailBoxRef *m_pkBaseRef;
	FvEntityDBRecordOut	m_kOutRec;
	FvDatabaseID *m_pkStrmDbID;
};

void FvLoginHandler::Login()
{
	FvDatabase::Instance().GetIDatabase().MapLoginToEntityDBKey(
		m_spParams->Username(), m_spParams->Password(), *this );
}

void FvLoginHandler::OnMapLoginToEntityDBKeyComplete( FvDatabaseLoginStatus status,
												   const FvEntityDBKey& ekey )
{
	bool shouldLoadEntity = false;
	bool shouldCreateEntity = false;

	if (status == FvDatabaseLoginStatus::LOGGED_ON)
	{
		m_kEntityDBKey = ekey;
		shouldLoadEntity = true;
		m_eState = StateWaitingForLoad;
	}
	else if (status == FvDatabaseLoginStatus::LOGIN_REJECTED_NO_SUCH_USER)
	{
		if (FvDatabase::Instance().ShouldLoadUnknown())
		{
			m_kEntityDBKey.m_uiTypeID = FvDatabase::Instance().GetEntityDefs().GetDefaultType();
			m_kEntityDBKey.m_kName = m_spParams->Username();
			shouldLoadEntity = true;
			m_eState = StateWaitingForLoadUnknown;
		}
		else if (FvDatabase::Instance().ShouldCreateUnknown())
		{
			shouldCreateEntity = true;
		}
	}

	if (shouldLoadEntity)
	{
		m_pkStrmDbID = FvDatabase::PrepareCreateEntityBundle( m_kEntityDBKey.m_uiTypeID,
			m_kEntityDBKey.m_iDBID, m_kClientAddr, this, m_kBundle, m_spParams );

		m_pkBaseRef = &m_kBaseRef;
		m_kOutRec.ProvideBaseMB( m_pkBaseRef );	
		m_kOutRec.ProvideStrm( m_kBundle );

		FvDatabase::Instance().GetEntity( *this );
	}
	else if (shouldCreateEntity)
	{
		this->CreateNewEntity();
	}
	else
	{
		const char * msg;
		bool isError = false;
		switch (status)
		{
			case FvDatabaseLoginStatus::LOGIN_REJECTED_NO_SUCH_USER:
				msg = "Unknown user."; break;
			case FvDatabaseLoginStatus::LOGIN_REJECTED_INVALID_PASSWORD:
				msg = "Invalid password."; break;
			case FvDatabaseLoginStatus::LOGIN_REJECTED_DB_GENERAL_FAILURE:
				msg = "Unexpected database failure.";
				isError = true;
				break;
			default:
				msg = UNSPECIFIED_ERROR_STR;
				isError = true;
				break;
		}
		if (isError)
		{
			FV_ERROR_MSG( "FvDatabase::LogOn: MapLoginToEntityDBKey for %s failed: "
				"(%d) %s\n", m_spParams->Username().c_str(), int(status), msg );
		}
		else
		{
			FV_NOTICE_MSG( "FvDatabase::LogOn: MapLoginToEntityDBKey for %s failed: "
				"(%d) %s\n", m_spParams->Username().c_str(), int(status), msg );
		}
		FvDatabase::Instance().SendFailure( m_iReplyID, m_kReplyAddr, m_bOffChannel,
			status, msg );
		delete this;
	}

}

void FvLoginHandler::OnGetEntityCompleted( bool isOK )
{
	if (!isOK)
	{
		if ( (m_eState == StateWaitingForLoadUnknown) &&
			 FvDatabase::Instance().ShouldCreateUnknown() )
		{
			this->CreateNewEntity( true );
		}
		else
		{
			FV_ERROR_MSG( "FvDatabase::LogOn: Entity %s does not exist\n",
				m_kEntityDBKey.m_kName.c_str() );

			this->SendFailureReply(
				FvDatabaseLoginStatus::LOGIN_REJECTED_NO_SUCH_USER,
				"Failed to load entity." );
		}
	}
	else
	{
		if (m_pkStrmDbID)
		{
			*m_pkStrmDbID = m_kEntityDBKey.m_iDBID;
		}

		if ( (m_eState == StateWaitingForLoadUnknown) &&
			 (FvDatabase::Instance().ShouldRememberUnknown()) )
		{
			m_eState = StateWaitingForSetLoginMappingForLoadUnknown;

			FvDatabase::Instance().SetLoginMapping(
				m_spParams->Username(), m_spParams->Password(), m_kEntityDBKey, *this );
		}
		else
		{
			this->CheckOutEntity();
		}
	}
}

void FvLoginHandler::CheckOutEntity()
{
	if ( !m_kOutRec.GetBaseMB() &&
		FvDatabase::Instance().OnStartEntityCheckout( m_kEntityDBKey ) )
	{
		m_eState = StateWaitingForSetBaseToLoggingOn;
		FvDatabase::SetBaseRefToLoggingOn( m_kBaseRef, m_kEntityDBKey.m_uiTypeID );
		m_pkBaseRef = &m_kBaseRef;
		FvEntityDBRecordIn	erec;
		erec.ProvideBaseMB( m_pkBaseRef );
		FvDatabase::Instance().PutEntity( m_kEntityDBKey, erec, *this );
	}
	else
	{
		FvDatabase::Instance().OnLogOnLoggedOnUser( m_kEntityDBKey.m_uiTypeID, m_kEntityDBKey.m_iDBID,
			m_spParams, m_kClientAddr, m_kReplyAddr, m_bOffChannel, m_iReplyID,
			m_kOutRec.GetBaseMB() );

		delete this;
	}
}

void FvLoginHandler::OnPutEntityComplete( bool isOK, FvDatabaseID dbID )
{
	switch (m_eState)
	{
		case StateWaitingForPutNewEntity:
			FV_ASSERT( m_pkStrmDbID );
			*m_pkStrmDbID = dbID;
			if (isOK)
			{
				m_kEntityDBKey.m_iDBID = dbID;

				m_eState = StateWaitingForSetLoginMappingForCreateUnknown;
				FvDatabase::Instance().SetLoginMapping(
					m_spParams->Username(), m_spParams->Password(),	m_kEntityDBKey, *this );

				break;
			}
			else
			{
				FV_ERROR_MSG( "FvDatabase::LogOn: Failed to write default entity "
					"for %s\n", m_spParams->Username().c_str());
			}
		case StateWaitingForSetBaseToLoggingOn:
			if (isOK)
				this->SendCreateEntityMsg();
			else
			{
				FvDatabase::Instance().OnCompleteEntityCheckout( m_kEntityDBKey, NULL );
				this->SendFailureReply(
						FvDatabaseLoginStatus::LOGIN_REJECTED_DB_GENERAL_FAILURE,
						"Unexpected database failure." );
			}
			break;
		case StateWaitingForSetBaseToFinal:
			FvDatabase::Instance().OnCompleteEntityCheckout( m_kEntityDBKey,
					(isOK) ? &m_kBaseRef : NULL );
			if (isOK)
				this->SendReply();
			else
				this->SendFailureReply(
						FvDatabaseLoginStatus::LOGIN_REJECTED_DB_GENERAL_FAILURE,
						"Unexpected database failure." );
			break;
		default:
			FV_ASSERT(false);
			delete this;
	}
}

FV_INLINE void FvLoginHandler::SendCreateEntityMsg()
{
	FV_INFO_MSG( "FvDatabase::LogOn: %s\n", m_spParams->Username().c_str() );

	FvDatabase::Instance().GetBaseAppManager().Send( &m_kBundle );
}

FV_INLINE void FvLoginHandler::SendReply()
{
	if (m_bOffChannel)
	{
		FvDatabase::Instance().nub().Send( m_kReplyAddr, m_kBundle );
	}
	else
	{
		FvDatabase::GetChannel( m_kReplyAddr ).Send( &m_kBundle );
	}
	delete this;
}

FV_INLINE void FvLoginHandler::SendFailureReply( FvDatabaseLoginStatus status,
		const char * msg )
{
	FvDatabase::Instance().SendFailure( m_iReplyID, m_kReplyAddr, m_bOffChannel,
		status, msg );
	delete this;
}

void FvLoginHandler::OnSetLoginMappingComplete()
{
	if (m_eState == StateWaitingForSetLoginMappingForLoadUnknown)
	{
		this->CheckOutEntity();
	}
	else
	{
		FV_ASSERT( m_eState == StateWaitingForSetLoginMappingForCreateUnknown );
		this->SendCreateEntityMsg();
	}
}

void FvLoginHandler::CreateNewEntity( bool isBundlePrepared )
{
	//! add by Uman, 20101120, 接口废弃, 因为属性的遍历顺序已改变
	FV_ASSERT(0 && "Code is abandoned!");

	m_kEntityDBKey.m_uiTypeID = FvDatabase::Instance().GetEntityDefs().GetDefaultType();
	m_kEntityDBKey.m_kName = m_spParams->Username();

	if (!isBundlePrepared)
	{
		m_pkStrmDbID = FvDatabase::PrepareCreateEntityBundle( m_kEntityDBKey.m_uiTypeID,
			0, m_kClientAddr, this, m_kBundle, m_spParams );
	}

	bool isDefaultEntityOK;

	if (FvDatabase::Instance().ShouldRememberUnknown())
	{
		FvMemoryOStream strm;
		isDefaultEntityOK = FvDatabase::Instance().DefaultEntityToStrm(
			m_kEntityDBKey.m_uiTypeID, m_spParams->Username(), strm, &m_spParams->Password() );

		if (isDefaultEntityOK)
		{
			m_kBundle.Transfer( strm, strm.Size() );
			strm.Rewind();

			m_eState = StateWaitingForPutNewEntity;
			FvDatabase::SetBaseRefToLoggingOn( m_kBaseRef, m_kEntityDBKey.m_uiTypeID );
			m_pkBaseRef = &m_kBaseRef;
			FvEntityDBRecordIn	erec;
			erec.ProvideBaseMB( m_pkBaseRef );
			erec.ProvideStrm( strm );
			FvDatabase::Instance().PutEntity( m_kEntityDBKey, erec, *this );
		}
	}
	else
	{
		*m_pkStrmDbID = 0;

		isDefaultEntityOK = FvDatabase::Instance().DefaultEntityToStrm(
			m_kEntityDBKey.m_uiTypeID, m_spParams->Username(), m_kBundle,
			&m_spParams->Password() );

		if (isDefaultEntityOK)
			this->SendCreateEntityMsg();
	}

	if (!isDefaultEntityOK)
	{
		FV_ERROR_MSG( "FvDatabase::LogOn: Failed to create default entity for %s\n",
			m_spParams->Username().c_str());

		this->SendFailureReply( FvDatabaseLoginStatus::LOGIN_CUSTOM_DEFINED_ERROR,
				"Failed to create default entity" );
	}
}

void FvLoginHandler::HandleMessage( const FvNetAddress & source,
	FvNetUnpackedMessageHeader & header,
	FvBinaryIStream & data,
	void * arg )
{
	FvNetAddress proxyAddr;

	data >> proxyAddr;

	if (proxyAddr.m_uiIP == 0)
	{
		FvDatabaseLoginStatus::Status status;
		switch (proxyAddr.m_uiPort)
		{
			case BaseAppMgrInterface::CREATE_ENTITY_ERROR_NO_BASEAPPS:
				status = FvDatabaseLoginStatus::LOGIN_REJECTED_NO_BASEAPPS;
				break;
			case BaseAppMgrInterface::CREATE_ENTITY_ERROR_BASEAPPS_OVERLOADED:
				status = FvDatabaseLoginStatus::LOGIN_REJECTED_BASEAPP_OVERLOAD;
				break;
			default:
				status = FvDatabaseLoginStatus::LOGIN_CUSTOM_DEFINED_ERROR;
				break;
		}

		this->HandleFailure( &data, status );
	}
	else
	{
		data >> m_kBaseRef;

		m_kBundle.Clear();
		m_kBundle.StartReply( m_iReplyID );

		m_kBundle << (FvUInt8)FvDatabaseLoginStatus::LOGGED_ON;
		m_kBundle << proxyAddr;
		m_kBundle.Transfer( data, data.RemainingLength() );

		if ( m_kEntityDBKey.m_iDBID )
		{
			m_eState = StateWaitingForSetBaseToFinal;
			m_pkBaseRef = &m_kBaseRef;
			FvEntityDBRecordIn erec;
			erec.ProvideBaseMB( m_pkBaseRef );
			FvDatabase::Instance().PutEntity( m_kEntityDBKey, erec, *this );
		}
		else
		{
			this->SendReply();
		}
	}
}

void FvLoginHandler::HandleException( const FvNetNubException & ne,
	void * arg )
{
	FvMemoryOStream mos;
	mos << "BaseAppMgr timed out creating entity.";
	this->HandleFailure( &mos,
			FvDatabaseLoginStatus::LOGIN_REJECTED_BASEAPPMGR_TIMEOUT );
}

void FvLoginHandler::HandleFailure( FvBinaryIStream * pData,
		FvDatabaseLoginStatus reason )
{
	m_kBundle.Clear();
	m_kBundle.StartReply( m_iReplyID );

	m_kBundle << (FvUInt8)reason;

	m_kBundle.Transfer( *pData, pData->RemainingLength() );

	if ( m_kEntityDBKey.m_iDBID )
	{
		m_eState = StateWaitingForSetBaseToFinal;
		m_pkBaseRef = 0;
		FvEntityDBRecordIn erec;
		erec.ProvideBaseMB( m_pkBaseRef );
		FvDatabase::Instance().PutEntity( m_kEntityDBKey, erec, *this );
	}
	else
	{
		this->SendReply();
	}
}


class RelogonAttemptHandler : public FvNetReplyMessageHandler,
                            public FvIDatabase::IPutEntityHandler
{
	enum State
	{
		StateWaitingForOnLogOnAttempt,
		StateWaitingForSetBaseToFinal,
		StateWaitingForSetBaseToNull,
		StateAborted
	};

public:
	RelogonAttemptHandler( FvEntityTypeID entityTypeID,
			FvDatabaseID dbID,
			const FvNetAddress & replyAddr,
			bool offChannel,
			FvNetReplyID replyID,
			LogOnParamsPtr pParams,
			const FvNetAddress & addrForProxy ) :
		m_eState( StateWaitingForOnLogOnAttempt ),
		m_kEntityDBKey( entityTypeID, dbID ),
		m_kReplyAddr( replyAddr ),
		m_bOffChannel( offChannel ),
		m_iReplyID( replyID ),
		m_spParams( pParams ),
		m_kAddrForProxy( addrForProxy ),
		m_kReplyBundle()
	{
		FvDatabase::Instance().OnStartRelogonAttempt( entityTypeID, dbID, this );
	}

	virtual ~RelogonAttemptHandler()
	{
		if (m_eState != StateAborted)
		{
			FvDatabase::Instance().OnCompleteRelogonAttempt(
					m_kEntityDBKey.m_uiTypeID, m_kEntityDBKey.m_iDBID );
		}
	}

	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		FvUInt8 result;
		data >> result;

		if (m_eState != StateAborted)
		{
			switch (result)
			{
				case BaseAppIntInterface::LOG_ON_ATTEMPT_TOOK_CONTROL:
				{
					FV_INFO_MSG( "RelogonAttemptHandler: It's taken over.\n" );
					FvNetAddress proxyAddr;
					data >> proxyAddr;

					FvEntityMailBoxRef baseRef;
					data >> baseRef;

					m_kReplyBundle.StartReply( m_iReplyID );

					m_kReplyBundle << (FvUInt8)FvDatabaseLoginStatus::LOGGED_ON;
					m_kReplyBundle << proxyAddr;
					m_kReplyBundle.Transfer( data, data.RemainingLength() );

					m_eState = StateWaitingForSetBaseToFinal;
					FvEntityMailBoxRef*	pBaseRef = &baseRef;
					FvEntityDBRecordIn	erec;
					erec.ProvideBaseMB( pBaseRef );
					FvDatabase::Instance().PutEntity( m_kEntityDBKey, erec, *this );

					return;
				}
				case BaseAppIntInterface::LOG_ON_ATTEMPT_NOT_EXIST:
				{
					FV_INFO_MSG( "RelogonAttemptHandler: Entity does not exist. "
						"Attempting to log on normally.\n" );

					m_eState = StateWaitingForSetBaseToNull;
					FvEntityDBRecordIn	erec;
					FvEntityMailBoxRef* 	pBaseRef = 0;
					erec.ProvideBaseMB( pBaseRef );
					FvDatabase::Instance().PutEntity( m_kEntityDBKey, erec, *this );

					return;
				}
				case BaseAppIntInterface::LOG_ON_ATTEMPT_REJECTED:
				{
					FV_INFO_MSG( "RelogonAttemptHandler: "
							"Re-login not allowed for %s.\n",
						m_spParams->Username().c_str() );

					FvDatabase::Instance().SendFailure( m_iReplyID, m_kReplyAddr,
						false, /* offChannel */
						FvDatabaseLoginStatus::LOGIN_REJECTED_ALREADY_LOGGED_IN,
						"Relogin denied." );

					break;
				}
				default:
					FV_CRITICAL_MSG( "RelogonAttemptHandler: Invalid result %d\n",
							int(result) );
					break;
			}
		}

		delete this;
	}

	virtual void HandleException( const FvNetNubException & exception,
		void * arg )
	{
		if (m_eState != StateAborted)
		{
			const char * errorMsg = NetReasonToString( exception.Reason() );
			FV_ERROR_MSG( "RelogonAttemptHandler: %s.\n", errorMsg );
			FvDatabase::Instance().SendFailure( m_iReplyID, m_kReplyAddr,
					m_bOffChannel,
					FvDatabaseLoginStatus::LOGIN_REJECTED_BASEAPP_TIMEOUT,
					errorMsg );
		}

		delete this;
	}

	virtual void OnPutEntityComplete( bool isOK, FvDatabaseID )
	{
		switch (m_eState)
		{
			case StateWaitingForSetBaseToFinal:
				if (isOK)
				{
					if (!m_bOffChannel)
					{
						FvDatabase::GetChannel( m_kReplyAddr ).Send(
							&m_kReplyBundle );
					}
					else
					{
						FvDatabase::Instance().nub().Send( m_kReplyAddr,
							m_kReplyBundle );
					}
				}
				else
				{
					this->sendEntityDeletedFailure();
				}
				break;
			case StateWaitingForSetBaseToNull:
				if (isOK)
				{
					this->onEntityLogOff();
				}
				else
				{
					this->sendEntityDeletedFailure();
				}
				break;
			case StateAborted:
				break;
			default:
				FV_CRITICAL_MSG( "RelogonHandler::OnPutEntityComplete: "
						"Invalid state %d\n",
					m_eState );
				break;
		}

		delete this;
	}

	void sendEntityDeletedFailure()
	{
		FV_ERROR_MSG( "FvDatabase::LogOn: Entity %s was deleted during logon.\n",
			m_kEntityDBKey.m_kName.c_str() );

		FvDatabase::Instance().SendFailure( m_iReplyID, m_kReplyAddr,
				m_bOffChannel,
				FvDatabaseLoginStatus::LOGIN_REJECTED_NO_SUCH_USER,
				"Entity deleted during login." );
	}

	void onEntityLogOff()
	{
		if (m_eState != StateAborted)
		{
			m_eState = StateAborted;
			FvDatabase::Instance().OnCompleteRelogonAttempt(
					m_kEntityDBKey.m_uiTypeID, m_kEntityDBKey.m_iDBID );

			FvDatabase::Instance().LogOn( m_kReplyAddr, m_iReplyID, m_spParams,
				m_kAddrForProxy, m_bOffChannel );
		}
	}

private:
	State m_eState;
	FvEntityDBKey m_kEntityDBKey;
	FvNetAddress m_kReplyAddr;
	bool m_bOffChannel;
	FvNetReplyID m_iReplyID;
	LogOnParamsPtr m_spParams;
	FvNetAddress m_kAddrForProxy;
	FvNetBundle m_kReplyBundle;
};

void FvDatabase::LogOn( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
{
	FvNetAddress addrForProxy;
	bool offChannel;
	LogOnParamsPtr pParams = new FvLogOnParams();

	data >> addrForProxy >> offChannel >> *pParams;

	const FvMD5::Digest & digest = pParams->Digest();
	bool goodDigest = (digest == this->GetEntityDefs().GetDigest());

	if (!goodDigest && m_bAllowEmptyDigest)
	{
		goodDigest = true;

		for (FvUInt32 i = 0; i < sizeof( digest ); i++)
		{
			if (digest.m_acBytes[i] != '\0')
			{
				goodDigest = false;
				break;
			}
		}

		if (goodDigest)
		{
			FV_WARNING_MSG( "FvDatabase::LogOn: %s logged on with empty digest.\n",
				pParams->Username().c_str() );
		}
	}

	if (!goodDigest)
	{
		FV_ERROR_MSG( "FvDatabase::LogOn: Incorrect digest\n" );
		this->SendFailure( header.m_iReplyID, srcAddr, offChannel,
			FvDatabaseLoginStatus::LOGIN_REJECTED_BAD_DIGEST,
			"Defs digest mismatch." );
		return;
	}

	this->LogOn( srcAddr, header.m_iReplyID, pParams, addrForProxy, offChannel );
}

void FvDatabase::LogOn( const FvNetAddress & srcAddr,
		FvNetReplyID replyID,
		LogOnParamsPtr pParams,
		const FvNetAddress & addrForProxy,
		bool offChannel )
{
	if (m_kStatus.GetStatus() != FvDBStatus::RUNNING)
	{
		FV_INFO_MSG( "FvDatabase::LogOn: "
			"Login failed for %s. Server not ready.\n",
			pParams->Username().c_str() );

		this->SendFailure( replyID, srcAddr, offChannel,
			FvDatabaseLoginStatus::LOGIN_REJECTED_SERVER_NOT_READY,
			"Server not ready." );

		return;
	}

	bool isOverloaded = m_fCurLoad > m_fMaxLoad;

	if (this->CalculateOverloaded( isOverloaded ))
	{
		FV_INFO_MSG( "FvDatabase::LogOn: "
				"Login failed for %s. We are overloaded "
				"(load=%.02f > max=%.02f)\n",
			pParams->Username().c_str(), m_fCurLoad, m_fMaxLoad );

		this->SendFailure( replyID, srcAddr, offChannel,
			FvDatabaseLoginStatus::LOGIN_REJECTED_DBMGR_OVERLOAD,
			"DBManager is overloaded." );

		return;
	}

	if (m_bAnyCellAppOverloaded)
	{
		FV_INFO_MSG( "FvDatabase::LogOn: "
			"Login failed for %s. At least one CellApp is overloaded.\n",
			pParams->Username().c_str() );

		this->SendFailure( replyID, srcAddr, offChannel,
			FvDatabaseLoginStatus::LOGIN_REJECTED_CELLAPP_OVERLOAD,
			"At least one CellApp is overloaded." );

		return;
	}

	FvLoginHandler * pHandler =
		new FvLoginHandler( pParams, addrForProxy, srcAddr, offChannel, replyID );

	pHandler->Login();
}

bool FvDatabase::CalculateOverloaded( bool isOverloaded )
{
	if (isOverloaded)
	{
		FvUInt64 overloadTime;

		if (m_uiOverloadStartTime == 0)
			m_uiOverloadStartTime = Timestamp();

		overloadTime = Timestamp() - m_uiOverloadStartTime;
		FV_INFO_MSG( "DBManager::Overloaded for "PRIu64"ms\n", overloadTime/(StampsPerSecond()/1000) );

		return (overloadTime >= m_uiAllowOverloadPeriod);
	}
	else
	{
		m_uiOverloadStartTime = 0;
		return false;
	}
}

void FvDatabase::OnLogOnLoggedOnUser( FvEntityTypeID typeID, FvDatabaseID dbID,
	LogOnParamsPtr pParams,
	const FvNetAddress & clientAddr, const FvNetAddress & replyAddr,
	bool offChannel, FvNetReplyID replyID,
	const FvEntityMailBoxRef* pExistingBase )
{
	//! add by Uman, 20101213, 重复登录暂时先挡回
	FV_INFO_MSG( "FvDatabase::LogOn: %s already logged on\n",
		pParams->Username().c_str() );

	this->SendFailure( replyID, replyAddr, offChannel,
		FvDatabaseLoginStatus::LOGIN_REJECTED_ALREADY_LOGGED_IN,
		"A relogin of same name still in progress." );
	return;
	//! add end

	bool bShouldAttemptRelogon = true;

	if (bShouldAttemptRelogon &&
		(FvDatabase::Instance().GetInProgRelogonAttempt( typeID, dbID ) == NULL))
	{
		if (FvDatabase::IsValidMailBox( pExistingBase ))
		{
			FvNetChannelSender sender(
				FvDatabase::GetChannel( pExistingBase->m_kAddr ) );

			FvNetBundle & bundle = sender.Bundle();
			bundle.StartRequest( BaseAppIntInterface::LogOnAttempt,
				new RelogonAttemptHandler( pExistingBase->GetType(), dbID,
					replyAddr, offChannel, replyID, pParams, clientAddr )
				);

			bundle << pExistingBase->m_iID;
			bundle << clientAddr;
			bundle << pParams->EncryptionKey();

			bool hasPassword =
				this->GetEntityDefs().EntityTypeHasPassword( typeID );

			bundle << hasPassword;

			if (hasPassword)
			{
				bundle << pParams->Password();
			}
		}
		else
		{
			FV_WARNING_MSG( "FvDatabase::LogOn: %s already logging in\n",
				pParams->Username().c_str() );

			this->SendFailure( replyID, replyAddr, offChannel,
				FvDatabaseLoginStatus::LOGIN_REJECTED_ALREADY_LOGGED_IN,
			   "Another login of same name still in progress." );
		}
	}
	else
	{
		FV_INFO_MSG( "FvDatabase::LogOn: %s already logged on\n",
			pParams->Username().c_str() );

		this->SendFailure( replyID, replyAddr, offChannel,
				FvDatabaseLoginStatus::LOGIN_REJECTED_ALREADY_LOGGED_IN,
				"A relogin of same name still in progress." );
	}
}

bool FvDatabase::DefaultEntityToStrm( FvEntityTypeID typeID,
	const FvString& name, FvBinaryOStream& strm,
	const FvString* pPassword ) const
{
	FvXMLSectionPtr pSection = CreateNewEntity( typeID, name );
	bool isCreated = pSection.Exists();
	if (isCreated)
	{
		if (pPassword)
		{
			if (this->GetEntityDefs().GetPropertyType( typeID, "password" ) == "BLOB")
				pSection->WriteBlob( "password", *pPassword );
			else
				pSection->WriteString( "password", *pPassword );
		}

		const FvEntityDescription& desc =
			this->GetEntityDefs().GetEntityDescription( typeID );
		desc.AddSectionToStream( pSection, strm,
			FvEntityDescription::FV_BASE_DATA | FvEntityDescription::FV_CELL_DATA |
			FvEntityDescription::FV_ONLY_PERSISTENT_DATA );
		if (desc.HasCellScript())
		{
			FvVector3 defaultVec( 0, 0, 0 );

			strm << defaultVec;
			strm << defaultVec;
			strm << FvSpaceID(0);
		}

		strm << FvTimeStamp(0);
	}

	return isCreated;
}

FvDatabaseID* FvDatabase::PrepareCreateEntityBundle( FvEntityTypeID typeID,
		FvDatabaseID dbID, const FvNetAddress& addrForProxy,
		FvNetReplyMessageHandler* pHandler, FvNetBundle& bundle,
		LogOnParamsPtr pParams )
{
	bundle.StartRequest( BaseAppMgrInterface::CreateEntity, pHandler, 0,
		FV_NET_DEFAULT_REQUEST_TIMEOUT + 1000000 ); 

	bundle	<< FvEntityID( 0 )
			<< typeID;

	FvDatabaseID*	pDbID = 0;
	if (dbID)
		bundle << dbID;
	else
		pDbID = reinterpret_cast<FvDatabaseID*>(bundle.Reserve( sizeof(*pDbID) ));

	bundle << addrForProxy;

	bundle << ((pParams != NULL) ? pParams->EncryptionKey() : "");

	bundle << true;

	return pDbID;
}

void FvDatabase::SendFailure( FvNetReplyID replyID,
		const FvNetAddress & dstAddr, bool offChannel,
		FvDatabaseLoginStatus reason, const char * pDescription )
{
	FV_ASSERT( reason != FvDatabaseLoginStatus::LOGGED_ON );

	FvNetBundle * pBundle;

	if (offChannel)
	{
		pBundle = new FvNetBundle();
	}
	else
	{
		FvNetChannelSender sender( FvDatabase::GetChannel( dstAddr ) );
		pBundle = &sender.Bundle();
	}
	FvNetBundle & bundle = *pBundle;

	bundle.StartReply( replyID );
	bundle << FvUInt8( reason );

	if (pDescription == NULL)
		pDescription = UNSPECIFIED_ERROR_STR;

	bundle << pDescription;

	if (offChannel)
	{
		FvDatabase::Instance().nub().Send( dstAddr, bundle );
		delete pBundle;
	}
}

class WriteEntityHandler : public FvIDatabase::IPutEntityHandler,
                           public FvIDatabase::IDelEntityHandler
{
	FvEntityDBKey m_kEntityDBKey;
	FvInt8 m_iFlags;
	bool m_bShouldReply;
	FvNetReplyID m_iReplyID;
	const FvNetAddress m_kSrcAddr;

public:
	WriteEntityHandler( const FvEntityDBKey ekey, FvInt8 flags, bool shouldReply,
			FvNetReplyID replyID, const FvNetAddress & srcAddr )
		: m_kEntityDBKey(ekey), m_iFlags(flags), m_bShouldReply(shouldReply),
		m_iReplyID(replyID), m_kSrcAddr(srcAddr)
	{}
	virtual ~WriteEntityHandler() {}

	void WriteEntity( FvBinaryIStream & data, FvEntityID entityID );

	void DeleteEntity();

	virtual void OnPutEntityComplete( bool isOK, FvDatabaseID );

	virtual void OnDelEntityComplete( bool isOK );

private:
	void PutEntity( FvEntityDBRecordIn& erec )
	{
		FvDatabase::Instance().PutEntity( m_kEntityDBKey, erec, *this );
	}
	void Finalise(bool isOK);
};


void WriteEntityHandler::WriteEntity( FvBinaryIStream & data, FvEntityID entityID )
{
	FvEntityDBRecordIn erec;
	if (m_iFlags & WRITE_ALL_DATA)
		erec.ProvideStrm( data );

	if (m_iFlags & WRITE_LOG_OFF)
	{
		FvEntityMailBoxRef* pBaseRef = 0;
		erec.ProvideBaseMB( pBaseRef );
		this->PutEntity( erec );
	}
	else if (!m_kEntityDBKey.m_iDBID)
	{
		FvEntityMailBoxRef	baseRef;
		baseRef.Init( entityID, m_kSrcAddr, FvEntityMailBoxRef::BASE,
			m_kEntityDBKey.m_uiTypeID );
		FvEntityMailBoxRef* pBaseRef = &baseRef;
		erec.ProvideBaseMB( pBaseRef );
		this->PutEntity( erec );
	}
	else
	{
		this->PutEntity( erec );
	}
}

void WriteEntityHandler::OnPutEntityComplete( bool isOK, FvDatabaseID dbID )
{
	m_kEntityDBKey.m_iDBID = dbID;
	if (!isOK)
	{
		FV_ERROR_MSG( "FvDatabase::WriteEntity: Failed to update entity %"FMT_DBID
			" of type %d.\n", dbID, m_kEntityDBKey.m_uiTypeID );
	}

	this->Finalise(isOK);
}

void WriteEntityHandler::DeleteEntity()
{
	FV_ASSERT( m_iFlags & WRITE_DELETE_FROM_DB );
	FvDatabase::Instance().DelEntity( m_kEntityDBKey, *this );
}

void WriteEntityHandler::OnDelEntityComplete( bool isOK )
{
	if (!isOK)
	{
		FV_ERROR_MSG( "FvDatabase::WriteEntity: Failed to delete entity %"FMT_DBID" of type %d.\n",
			m_kEntityDBKey.m_iDBID, m_kEntityDBKey.m_uiTypeID );
	}

	this->Finalise(isOK);
}

void WriteEntityHandler::Finalise( bool isOK )
{
	if (m_bShouldReply)
	{
		FvNetChannelSender sender( FvDatabase::GetChannel( m_kSrcAddr ) );
		sender.Bundle().StartReply( m_iReplyID );
		sender.Bundle() << isOK << m_kEntityDBKey.m_iDBID;
	}

	if (isOK && (m_iFlags & WRITE_LOG_OFF))
	{
		FvDatabase::Instance().OnEntityLogOff( m_kEntityDBKey.m_uiTypeID, m_kEntityDBKey.m_iDBID );
	}

	delete this;
}

void FvDatabase::WriteEntity( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
{
	FvNetNub::TransientMiniTimer::Op< FvNetNub::TransientMiniTimer >
		writeEntityTimerOp(m_kWriteEntityTimer);

	FvInt8 flags;
	data >> flags;

	FV_ASSERT( flags & (WRITE_ALL_DATA|WRITE_LOG_OFF) );

	FvEntityDBKey	ekey( 0, 0 );
	data >> ekey.m_uiTypeID >> ekey.m_iDBID;

	 FV_TRACE_MSG( "FvDatabase::WriteEntity: %lld flags=%i\n",
			   ekey.m_iDBID, flags );

	bool isOkay = this->GetEntityDefs().IsValidEntityType( ekey.m_uiTypeID );
	if (!isOkay)
	{
		FV_ERROR_MSG( "FvDatabase::WriteEntity: Invalid entity type %d\n",
			ekey.m_uiTypeID );

		if (header.m_cFlags & FvNetPacket::FLAG_HAS_REQUESTS)
		{
			FvNetChannelSender sender( FvDatabase::GetChannel( srcAddr ) );
			sender.Bundle().StartReply( header.m_iReplyID );
			sender.Bundle() << isOkay << ekey.m_iDBID;
		}
	}
	else
	{
		WriteEntityHandler* pHandler =
			new WriteEntityHandler( ekey, flags,
				(header.m_cFlags & FvNetPacket::FLAG_HAS_REQUESTS),
				header.m_iReplyID, srcAddr );
		if (flags & WRITE_DELETE_FROM_DB)
		{
			pHandler->DeleteEntity();
		}
		else
		{
			FvEntityID entityID;
			data >> entityID;

			pHandler->WriteEntity( data, entityID );
		}
	}
}

void FvDatabase::OnEntityLogOff( FvEntityTypeID typeID, FvDatabaseID dbID )
{
	RelogonAttemptHandler* pHandler =
			this->GetInProgRelogonAttempt( typeID, dbID );
	if (pHandler)
		pHandler->onEntityLogOff();
}

class LoadEntityHandler : public FvDatabase::GetEntityHandler,
                          public FvIDatabase::IPutEntityHandler,
                          public FvDatabase::ICheckoutCompletionListener
{
	FvEntityDBKey m_kEntityDBKey;
	FvEntityMailBoxRef m_kBaseRef;
	FvEntityMailBoxRef *m_pkBaseRef;
	FvEntityDBRecordOut	m_kOutRec;
	FvNetAddress m_kSrcAddr;
	FvEntityID m_iEntityID;
	FvNetReplyID m_iReplyID;
	FvNetBundle m_kReplyBundle;
	FvDatabaseID *m_pkStrmDbID;

public:
	LoadEntityHandler( const FvEntityDBKey& ekey,
			const FvNetAddress& srcAddr, FvEntityID entityID,
			FvNetReplyID replyID ) :
		m_kEntityDBKey(ekey), m_kBaseRef(), m_pkBaseRef(0), m_kOutRec(), m_kSrcAddr(srcAddr),
		m_iEntityID(entityID), m_iReplyID(replyID), m_pkStrmDbID(0)
	{}
	virtual ~LoadEntityHandler() {}

	void LoadEntity();

	virtual FvEntityDBKey& GetKey()					{	return m_kEntityDBKey;	}
	virtual FvEntityDBRecordOut& OutRec() 		{	return m_kOutRec;	}
	virtual void OnGetEntityCompleted( bool isOK );

	virtual void OnPutEntityComplete( bool isOK, FvDatabaseID );

	virtual void OnCheckoutCompleted( const FvEntityMailBoxRef* pBaseRef );

private:
	void SendAlreadyCheckedOutReply( const FvEntityMailBoxRef& baseRef );
};



void LoadEntityHandler::LoadEntity()
{
	m_kReplyBundle.StartReply( m_iReplyID );
	m_kReplyBundle << (FvUInt8) FvDatabaseLoginStatus::LOGGED_ON;

	if (m_kEntityDBKey.m_iDBID)
	{
		m_kReplyBundle << m_kEntityDBKey.m_iDBID;
	}
	else
	{
		m_pkStrmDbID = reinterpret_cast<FvDatabaseID*>(m_kReplyBundle.Reserve(sizeof(*m_pkStrmDbID)));
	}

	m_pkBaseRef = &m_kBaseRef;
	m_kOutRec.ProvideBaseMB( m_pkBaseRef );
	m_kOutRec.ProvideStrm( m_kReplyBundle );
	FvDatabase::Instance().GetEntity( *this );
}

void LoadEntityHandler::OnGetEntityCompleted( bool isOK )
{
	if (isOK)
	{
		if ( !m_pkBaseRef &&
			 FvDatabase::Instance().OnStartEntityCheckout( m_kEntityDBKey ) )
		{
			if (m_pkStrmDbID)
			{
				*m_pkStrmDbID = m_kEntityDBKey.m_iDBID;
			}

			m_kBaseRef.Init( m_iEntityID, m_kSrcAddr, FvEntityMailBoxRef::BASE, m_kEntityDBKey.m_uiTypeID );

			m_pkBaseRef = &m_kBaseRef;
			FvEntityDBRecordIn inrec;
			inrec.ProvideBaseMB( m_pkBaseRef );
			FvDatabase::Instance().PutEntity( m_kEntityDBKey, inrec, *this );
			return;	
		}
		else if (m_pkBaseRef)
		{
			this->SendAlreadyCheckedOutReply( *m_pkBaseRef );
		}
		else
		{
			FV_VERIFY( FvDatabase::Instance().RegisterCheckoutCompletionListener(
					m_kEntityDBKey.m_uiTypeID, m_kEntityDBKey.m_iDBID, *this ) );
			return;	
		}
	}
	else
	{
		if (m_kEntityDBKey.m_iDBID)
			FV_ERROR_MSG( "FvDatabase::LoadEntity: No such entity %"FMT_DBID" of type %d.\n",
					m_kEntityDBKey.m_iDBID, m_kEntityDBKey.m_uiTypeID );
		else
			FV_ERROR_MSG( "FvDatabase::LoadEntity: No such entity %s of type %d.\n",
					m_kEntityDBKey.m_kName.c_str(), m_kEntityDBKey.m_uiTypeID );
		FvDatabase::Instance().SendFailure( m_iReplyID, m_kSrcAddr, false,
			FvDatabaseLoginStatus::LOGIN_REJECTED_NO_SUCH_USER,
			"No such user." );
	}
	delete this;
}

void LoadEntityHandler::OnPutEntityComplete( bool isOK, FvDatabaseID )
{
	if (isOK)
	{
		FvDatabase::GetChannel( m_kSrcAddr ).Send( &m_kReplyBundle );
	}
	else
	{
		FvDatabase::Instance().SendFailure( m_iReplyID, m_kSrcAddr, false,
			FvDatabaseLoginStatus::LOGIN_REJECTED_DB_GENERAL_FAILURE,
			"Unexpected failure from database layer." );
	}
	FvDatabase::Instance().OnCompleteEntityCheckout( m_kEntityDBKey,
			(isOK) ? &m_kBaseRef : NULL );

	delete this;
}

void LoadEntityHandler::OnCheckoutCompleted( const FvEntityMailBoxRef* pBaseRef )
{
	if (pBaseRef)
	{
		this->SendAlreadyCheckedOutReply( *pBaseRef );
	}
	else
	{
		FvDatabase::Instance().SendFailure( m_iReplyID, m_kSrcAddr, false,
				FvDatabaseLoginStatus::LOGIN_REJECTED_DB_GENERAL_FAILURE,
				"Unexpected failure from database layer." );
	}

	delete this;
}

void LoadEntityHandler::SendAlreadyCheckedOutReply(
		const FvEntityMailBoxRef& baseRef )
{
	FvNetChannelSender sender( FvDatabase::GetChannel( m_kSrcAddr ) );
	FvNetBundle & bundle = sender.Bundle();

	bundle.StartReply( m_iReplyID );
	bundle << FvUInt8( FvDatabaseLoginStatus::LOGIN_REJECTED_ALREADY_LOGGED_IN );
	bundle << m_kEntityDBKey.m_iDBID;
	bundle << baseRef;
}

void FvDatabase::LoadEntity( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream& input )
{
	FvEntityDBKey ekey( 0, 0 );
	FvXMLSectionPtr pSection;
	bool byName;
	FvEntityID entityID;
	input >> ekey.m_uiTypeID >> entityID >> byName;

	if (!this->GetEntityDefs().IsValidEntityType( ekey.m_uiTypeID ))
	{
		FV_ERROR_MSG( "FvDatabase::LoadEntity: Invalid entity type %d\n",
			ekey.m_uiTypeID );
		this->SendFailure( header.m_iReplyID, srcAddr, false,
			FvDatabaseLoginStatus::LOGIN_CUSTOM_DEFINED_ERROR,
			"Invalid entity type" );
		return;
	}

	if (byName)
		input >> ekey.m_kName;
	else
		input >> ekey.m_iDBID;

	LoadEntityHandler* pHandler = new LoadEntityHandler( ekey, srcAddr, entityID,
		header.m_iReplyID );
	pHandler->LoadEntity();
}

class DeleteEntityHandler : public FvDatabase::GetEntityHandler
                          , public FvIDatabase::IDelEntityHandler
{
	FvNetBundle m_kReplyBundle;
	FvNetAddress m_kSrcAddr;
	FvEntityDBKey m_kEntityDBKey;
	FvEntityMailBoxRef m_kBaseRef;
	FvEntityMailBoxRef *m_pkBaseRef;
	FvEntityDBRecordOut	m_kOutRec;

public:
	DeleteEntityHandler( FvEntityTypeID typeID, FvDatabaseID dbID,
		const FvNetAddress& srcAddr, FvNetReplyID replyID );
	DeleteEntityHandler( FvEntityTypeID typeID, const FvString& name,
		const FvNetAddress& srcAddr, FvNetReplyID replyID );
	virtual ~DeleteEntityHandler() {}

	void DeleteEntity();

	virtual FvEntityDBKey& GetKey()					{	return m_kEntityDBKey;	}
	virtual FvEntityDBRecordOut& OutRec()			{	return m_kOutRec;	}
	virtual void OnGetEntityCompleted( bool isOK );

	virtual void OnDelEntityComplete( bool isOK );
};

DeleteEntityHandler::DeleteEntityHandler( FvEntityTypeID typeID,
	FvDatabaseID dbID, const FvNetAddress& srcAddr, FvNetReplyID replyID )
	: m_kReplyBundle(), m_kSrcAddr(srcAddr), m_kEntityDBKey(typeID, dbID),
	m_kBaseRef(), m_pkBaseRef(0), m_kOutRec()
{
	m_kReplyBundle.StartReply(replyID);
}

DeleteEntityHandler::DeleteEntityHandler( FvEntityTypeID typeID,
		const FvString& name, const FvNetAddress& srcAddr,
		FvNetReplyID replyID )
	: m_kReplyBundle(), m_kSrcAddr(srcAddr), m_kEntityDBKey(typeID, 0, name),
	m_kBaseRef(), m_pkBaseRef(0), m_kOutRec()
{
	m_kReplyBundle.StartReply(replyID);
}

void DeleteEntityHandler::DeleteEntity()
{
	if (FvDatabase::Instance().GetEntityDefs().IsValidEntityType( m_kEntityDBKey.m_uiTypeID ))
	{
		m_pkBaseRef = &m_kBaseRef;
		m_kOutRec.ProvideBaseMB(m_pkBaseRef);
		FvDatabase::Instance().GetEntity( *this );
	}
	else
	{
		FV_ERROR_MSG( "DeleteEntityHandler::DeleteEntity: Invalid entity type "
				"%d\n", int(m_kEntityDBKey.m_uiTypeID) );
		m_kReplyBundle << FvInt32(-1);

		FvDatabase::GetChannel( m_kSrcAddr ).Send( &m_kReplyBundle );
		delete this;
	}
}

void DeleteEntityHandler::OnGetEntityCompleted( bool isOK )
{
	if (isOK)
	{
		if (FvDatabase::IsValidMailBox(m_kOutRec.GetBaseMB()))
		{
			FV_TRACE_MSG( "FvDatabase::DeleteEntity: entity checked out\n" );
			m_kReplyBundle << *m_kOutRec.GetBaseMB();
		}
		else
		{
			FvDatabase::Instance().DelEntity( m_kEntityDBKey, *this );
			return;
		}
	}
	else
	{
		FV_TRACE_MSG( "FvDatabase::DeleteEntity: no such entity\n" );
		m_kReplyBundle << FvInt32(-1);
	}

	FvDatabase::GetChannel( m_kSrcAddr ).Send( &m_kReplyBundle );

	delete this;
}

void DeleteEntityHandler::OnDelEntityComplete( bool isOK )
{
	if ( isOK )
	{
		FV_TRACE_MSG( "FvDatabase::DeleteEntity: succeeded\n" );
	}
	else
	{
		FV_ERROR_MSG( "FvDatabase::DeleteEntity: Failed to delete entity '%s' "
					"(%"FMT_DBID") of type %d\n",
					m_kEntityDBKey.m_kName.c_str(), m_kEntityDBKey.m_iDBID, m_kEntityDBKey.m_uiTypeID );
		m_kReplyBundle << FvInt32(-1);
	}

	FvDatabase::GetChannel( m_kSrcAddr ).Send( &m_kReplyBundle );

	delete this;
}

void FvDatabase::DeleteEntity( const FvNetAddress & srcAddr,
	FvNetUnpackedMessageHeader & header,
	DBInterface::DeleteEntityArgs & args )
{
	DeleteEntityHandler* pHandler = new DeleteEntityHandler( args.entityTypeID,
			args.dbid, srcAddr, header.m_iReplyID );
	pHandler->DeleteEntity();
}

void FvDatabase::DeleteEntityByName( const FvNetAddress & srcAddr,
	FvNetUnpackedMessageHeader & header,
	FvBinaryIStream & data )
{
	FvEntityTypeID	entityTypeID;
	FvString		name;
	data >> entityTypeID >> name;

	DeleteEntityHandler* pHandler = new DeleteEntityHandler( entityTypeID,
			name, srcAddr, header.m_iReplyID );
	pHandler->DeleteEntity();
}

class LookupEntityHandler : public FvDatabase::GetEntityHandler
{
	FvNetBundle m_kReplyBundle;
	FvNetAddress m_kSrcAddr;
	FvEntityDBKey m_kEntityDBKey;
	FvEntityMailBoxRef m_kBaseRef;
	FvEntityMailBoxRef *m_pkBaseRef;
	FvEntityDBRecordOut	m_kOutRec;
	bool m_bOffChannel;

public:
	LookupEntityHandler( FvEntityTypeID typeID, FvDatabaseID dbID,
		const FvNetAddress& srcAddr, FvNetReplyID replyID,
		bool offChannel );
	LookupEntityHandler( FvEntityTypeID typeID, const FvString& name,
		const FvNetAddress& srcAddr, FvNetReplyID replyID,
		bool offChannel );
	virtual ~LookupEntityHandler() {}

	void LookupEntity();

	virtual FvEntityDBKey& GetKey()					{	return m_kEntityDBKey;	}
	virtual FvEntityDBRecordOut& OutRec()			{	return m_kOutRec;	}
	virtual void OnGetEntityCompleted( bool isOK );

};

LookupEntityHandler::LookupEntityHandler( FvEntityTypeID typeID,
		FvDatabaseID dbID, const FvNetAddress& srcAddr,
		FvNetReplyID replyID, bool offChannel ) :
	m_kReplyBundle(), m_kSrcAddr(srcAddr), m_kEntityDBKey(typeID, dbID),
	m_kBaseRef(), m_pkBaseRef(0), m_kOutRec(), m_bOffChannel( offChannel )
{
	m_kReplyBundle.StartReply(replyID);
}

LookupEntityHandler::LookupEntityHandler( FvEntityTypeID typeID,
		const FvString& name, const FvNetAddress& srcAddr,
		FvNetReplyID replyID, bool offChannel ) :
	m_kReplyBundle(), m_kSrcAddr(srcAddr), m_kEntityDBKey(typeID, 0, name),
	m_kBaseRef(), m_pkBaseRef(0), m_kOutRec(), m_bOffChannel( offChannel )
{
	m_kReplyBundle.StartReply(replyID);
}

void LookupEntityHandler::LookupEntity()
{
	if (FvDatabase::Instance().GetEntityDefs().IsValidEntityType( m_kEntityDBKey.m_uiTypeID ))
	{
		m_pkBaseRef = &m_kBaseRef;
		m_kOutRec.ProvideBaseMB(m_pkBaseRef);
		FvDatabase::Instance().GetEntity( *this );
	}
	else
	{
		FV_ERROR_MSG( "LookupEntityHandler::LookupEntity: Invalid entity type "
				"%d\n", m_kEntityDBKey.m_uiTypeID );
		m_kReplyBundle << FvInt32(-1);

		FvDatabase::GetChannel( m_kSrcAddr ).Send( &m_kReplyBundle );

		delete this;
	}
}

void LookupEntityHandler::OnGetEntityCompleted( bool isOK )
{
	if (isOK)
	{
		if (FvDatabase::IsValidMailBox(m_kOutRec.GetBaseMB()))
		{
			m_kReplyBundle << *m_kOutRec.GetBaseMB();
		}
		else
		{

		}
	}
	else
	{
		m_kReplyBundle << FvInt32(-1);
	}

	if (m_bOffChannel)
	{
		FvDatabase::Instance().nub().Send( m_kSrcAddr, m_kReplyBundle );
	}
	else
	{
		FvDatabase::GetChannel( m_kSrcAddr ).Send( &m_kReplyBundle );
	}

	delete this;
}

void FvDatabase::LookupEntity( const FvNetAddress & srcAddr,
	FvNetUnpackedMessageHeader & header,
	DBInterface::LookupEntityArgs & args )
{
	LookupEntityHandler* pHandler = new LookupEntityHandler( args.entityTypeID,
		args.dbid, srcAddr, header.m_iReplyID, args.offChannel );
	pHandler->LookupEntity();
}

void FvDatabase::LookupEntityByName( const FvNetAddress & srcAddr,
	FvNetUnpackedMessageHeader & header,
	FvBinaryIStream & data )
{
	FvEntityTypeID m_uiEntityTypeID;
	FvString m_kName;
	bool m_bOffChannel;
	data >> m_uiEntityTypeID >> m_kName >> m_bOffChannel;
	LookupEntityHandler* pHandler = new LookupEntityHandler( m_uiEntityTypeID,
		m_kName, srcAddr, header.m_iReplyID, m_bOffChannel );
	pHandler->LookupEntity();
}

class LookupDBIDHandler : public FvDatabase::GetEntityHandler
{
	FvNetBundle m_kReplyBundle;
	FvNetAddress m_kSrcAddr;
	FvEntityDBKey m_kEntityDBKey;
	FvEntityDBRecordOut	m_kOutRec;

public:
	LookupDBIDHandler( FvEntityTypeID typeID, const FvString& name,
			const FvNetAddress& srcAddr, FvNetReplyID replyID ) :
		m_kReplyBundle(), m_kSrcAddr( srcAddr ), m_kEntityDBKey( typeID, 0, name ), m_kOutRec()
	{
		m_kReplyBundle.StartReply( replyID );
	}
	virtual ~LookupDBIDHandler() {}

	void LookupDBID();

	virtual FvEntityDBKey& GetKey()					{	return m_kEntityDBKey;	}
	virtual FvEntityDBRecordOut& OutRec()			{	return m_kOutRec;	}
	virtual void OnGetEntityCompleted( bool isOK );

};

void LookupDBIDHandler::LookupDBID()
{
	if (FvDatabase::Instance().GetEntityDefs().IsValidEntityType( m_kEntityDBKey.m_uiTypeID ))
	{
		FvDatabase::Instance().GetEntity( *this );
	}
	else
	{
		FV_ERROR_MSG( "LookupDBIDHandler::LookupDBID: Invalid entity type "
				"%d\n", m_kEntityDBKey.m_uiTypeID );
		m_kReplyBundle << FvDatabaseID( 0 );
		FvDatabase::GetChannel( m_kSrcAddr ).Send( &m_kReplyBundle );
		delete this;
	}
}

void LookupDBIDHandler::OnGetEntityCompleted( bool isOK )
{
	m_kReplyBundle << m_kEntityDBKey.m_iDBID;
	FvDatabase::GetChannel( m_kSrcAddr ).Send( &m_kReplyBundle );

	delete this;
}


void FvDatabase::LookupDBIDByName( const FvNetAddress & srcAddr,
	FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FvEntityTypeID entityTypeID;
	FvString name;
	data >> entityTypeID >> name;

	LookupDBIDHandler* pHandler = new LookupDBIDHandler( entityTypeID,
		name, srcAddr, header.m_iReplyID );
	pHandler->LookupDBID();
}


class ExecuteRawCommandHandler : public FvIDatabase::IExecuteRawCommandHandler
{
	FvNetBundle m_kReplyBundle;
	FvNetAddress m_kSrcAddr;

public:
	ExecuteRawCommandHandler( const FvNetAddress srcAddr,
			FvNetReplyID replyID ) :
		m_kReplyBundle(), m_kSrcAddr(srcAddr)
	{
		m_kReplyBundle.StartReply(replyID);
	}
	virtual ~ExecuteRawCommandHandler() {}

	void ExecuteRawCommand( const FvString& command )
	{
		FvDatabase::Instance().GetIDatabase().ExecuteRawCommand( command, *this );
	}

	virtual FvBinaryOStream& Response()	{	return m_kReplyBundle;	}
	virtual void OnExecuteRawCommandComplete()
	{
		FvDatabase::GetChannel( m_kSrcAddr ).Send( &m_kReplyBundle );

		delete this;
	}
};

void FvDatabase::ExecuteRawCommand( const FvNetAddress & srcAddr,
	FvNetUnpackedMessageHeader & header,
	FvBinaryIStream & data )
{
	FvString command( (char*)data.Retrieve( header.m_iLength ), header.m_iLength );
	ExecuteRawCommandHandler* pHandler =
		new ExecuteRawCommandHandler( srcAddr, header.m_iReplyID );
	pHandler->ExecuteRawCommand( command );
}

void FvDatabase::PutIDs( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream& input )
{
	int numIDs = input.RemainingLength() / sizeof(FvEntityID);
	FV_INFO_MSG( "FvDatabase::putIDs: storing %d id's\n", numIDs);
	m_pkDatabase->PutIDs( numIDs,
			static_cast<const FvEntityID*>( input.Retrieve( input.RemainingLength() ) ) );
}

class GetIDsHandler : public FvIDatabase::IGetIDsHandler
{
	FvNetAddress m_kSrcAddr;
	FvNetReplyID m_iReplyID;
	FvNetBundle m_kReplyBundle;

public:
	GetIDsHandler( const FvNetAddress& srcAddr, FvNetReplyID replyID ) :
		m_kSrcAddr(srcAddr), m_iReplyID( replyID ), m_kReplyBundle()
	{
		m_kReplyBundle.StartReply( replyID );
	}
	virtual ~GetIDsHandler() {}

	void GetIDs( int numIDs )
	{
		FvDatabase::Instance().GetIDatabase().GetIDs( numIDs, *this );
	}

	virtual FvBinaryOStream& GetIDStrm()	{ return m_kReplyBundle; }
	virtual void ResetStrm()
	{
		m_kReplyBundle.Clear();
		m_kReplyBundle.StartReply( m_iReplyID );
	}
	virtual void OnGetIDsComplete()
	{
		FvDatabase::GetChannel( m_kSrcAddr ).Send( &m_kReplyBundle );
		delete this;
	}
};

void FvDatabase::GetIDs( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream& input )
{
	int numIDs;
	input >> numIDs;
	FV_INFO_MSG( "FvDatabase::GetIDs: fetching %d id's\n", numIDs);

	GetIDsHandler* pHandler = new GetIDsHandler( srcAddr, header.m_iReplyID );
	pHandler->GetIDs( numIDs );
}

void FvDatabase::WriteSpaces( const FvNetAddress & /*srcAddr*/,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data )
{
	m_pkDatabase->WriteSpaceData( data );
}

void FvDatabase::HandleBaseAppDeath( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
{
	if (this->HasMailboxRemapping())
	{
		FV_ERROR_MSG( "FvDatabase::HandleBaseAppDeath: Multiple BaseApp deaths "
				"not supported. Some mailboxes may not be updated\n" );
		this->EndMailboxRemapping();
	}

	data >> m_kRemappingSrcAddr >> m_kRemappingDestAddrs;

	FV_INFO_MSG( "FvDatabase::HandleBaseAppDeath: %s\n", (char *)m_kRemappingSrcAddr );

	m_pkDatabase->RemapEntityMailboxes( m_kRemappingSrcAddr, m_kRemappingDestAddrs );

	m_iMailboxRemapCheckCount = 5;
}


void FvDatabase::EndMailboxRemapping()
{
//	FV_DEBUG_MSG( "FvDatabase::EndMailboxRemapping: End of HandleBaseAppDeath\n" );
	m_kRemappingDestAddrs.clear();
}


void FvDatabase::RemapMailbox( FvEntityMailBoxRef& mailbox ) const
{
	if (mailbox.m_kAddr == m_kRemappingSrcAddr)
	{
		const FvNetAddress& newAddr =
				m_kRemappingDestAddrs.AddressFor( mailbox.m_iID );
		mailbox.m_kAddr.m_uiIP = newAddr.m_uiIP;
		mailbox.m_kAddr.m_uiPort = newAddr.m_uiPort;
	}
}

void FvDatabase::WriteGameTime( DBInterface::WriteGameTimeArgs & args )
{
	m_pkDatabase->SetGameTime( args.gameTime );
}

void FvDatabase::SendInitData()
{
	m_pkDatabase->GetBaseAppMgrInitData( *this );
}

void FvDatabase::OnGetBaseAppMgrInitDataComplete( FvTimeStamp gameTime,
		FvInt32 maxSecondaryDBAppID )
{
	if (m_kStatus.GetStatus() < FvDBStatus::RUNNING)
	{
		maxSecondaryDBAppID = 0;
	}

	FvNetBundle& bundle = m_kBaseAppManager.Bundle();
	bundle.StartMessage( BaseAppMgrInterface::InitData );
	bundle << gameTime;
	bundle << maxSecondaryDBAppID;

	m_kBaseAppManager.Send();
}

void FvDatabase::HasStartBegun( bool hasStartBegun )
{
	if (hasStartBegun)
	{
		if (m_kStatus.GetStatus() >= FvDBStatus::WAITING_FOR_APPS)
		{
			this->StartServerBegin();
		}
		else
		{
			FV_NOTICE_MSG( "FvDatabase::HasStartBegun: Server is not ready "
					"to start yet\n" );
		}
	}
}

void FvDatabase::StartServerBegin( bool isRecover )
{
	if (m_kStatus.GetStatus() > FvDBStatus::WAITING_FOR_APPS)
	{
		FV_ERROR_MSG( "FvDatabase::StartServerBegin: Server already started. Cannot "
				"start again.\n" );
		return;
	}

	if (isRecover)
	{
		this->StartServerEnd( isRecover );
	}
	else
	{
		m_kStatus.Set( FvDBStatus::RESTORING_STATE, "Restoring game state" );

		FvNetBundle& bundle = m_kBaseAppManager.Bundle();
		bundle.StartMessage( BaseAppMgrInterface::SpaceDataRestore );
		if (m_pkDatabase->GetSpacesData( bundle ))
		{
			m_kBaseAppManager.Send();

			FvDBEntityRecoverer* pRecoverer = new FvDBEntityRecoverer;
			m_pkDatabase->RestoreEntities( *pRecoverer );
		}
		else
		{
			FV_CRITICAL_MSG( "FvDatabase::StartServerBegin: Failed to read game "
					"time and space data from database!" );
		}
	}
}

void FvDatabase::StartServerEnd( bool isRecover )
{
	if (m_kStatus.GetStatus() < FvDBStatus::RUNNING)
	{
		m_kStatus.Set( FvDBStatus::RUNNING, "Running" );

		if (!isRecover)
		{
			FV_TRACE_MSG( "FvDatabase::StartServerEnd: Sending startup message\n" );
			FvNetChannelSender sender(
				FvDatabase::Instance().GetBaseAppManager().Channel() );

			sender.Bundle().StartMessage( BaseAppMgrInterface::Startup );
		}
	}
	else
	{
		FV_ERROR_MSG( "FvDatabase::StartServerEnd: Already started.\n" );
	}
}

void FvDatabase::StartServerError()
{
	FV_ASSERT( m_kStatus.GetStatus() < FvDBStatus::RUNNING );
	this->StartSystemControlledShutdown();
}

bool FvDatabase::OnCompleteEntityCheckout( const FvEntityKey& entityID,
	const FvEntityMailBoxRef* pBaseRef )
{
	bool isErased = (m_kInProgCheckouts.erase( entityID ) > 0);
	if (isErased && (m_kCheckoutCompletionListeners.size() > 0))
	{
		std::pair< CheckoutCompletionListeners::iterator,
				CheckoutCompletionListeners::iterator > listeners =
			m_kCheckoutCompletionListeners.equal_range( entityID );
		for ( CheckoutCompletionListeners::const_iterator iter =
				listeners.first; iter != listeners.second; ++iter )
		{
			iter->second->OnCheckoutCompleted( pBaseRef );
		}
		m_kCheckoutCompletionListeners.erase( listeners.first,
				listeners.second );
	}

	return isErased;
}

bool FvDatabase::RegisterCheckoutCompletionListener( FvEntityTypeID typeID,
		FvDatabaseID dbID, FvDatabase::ICheckoutCompletionListener& listener )
{
	EntityKeySet::key_type key( typeID, dbID );
	bool isFound = (m_kInProgCheckouts.find( key ) != m_kInProgCheckouts.end());
	if (isFound)
	{
		CheckoutCompletionListeners::value_type item( key, &listener );
		m_kCheckoutCompletionListeners.insert( item );
	}
	return isFound;
}

template <class ARGS_TYPE>
class SimpleDBMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvDatabase::*Handler)( ARGS_TYPE & args );

	SimpleDBMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		if (sizeof(args) != 0)
			data >> args;
		(FvDatabase::Instance().*handler_)( args );
	}

	Handler handler_;
};

template <class ARGS_TYPE>
class ReturnDBMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvDatabase::*Handler)( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		ARGS_TYPE & args );

	ReturnDBMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		ARGS_TYPE & args = *(ARGS_TYPE*)data.Retrieve( sizeof(ARGS_TYPE) );
		(FvDatabase::Instance().*handler_)( srcAddr, header, args );
	}

	Handler handler_;
};

class DBListenerMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvDatabase::*Handler)(const InterfaceListenerMsg& kMsg);

	DBListenerMessageHandler(Handler kHandler):m_kHandler(kHandler) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		InterfaceListenerMsg kMsg;
		data >> kMsg;
		FV_ASSERT(data.RemainingLength()==0 && !data.Error());
		(FvDatabase::Instance().*m_kHandler)(kMsg);
	}

	Handler m_kHandler;
};

class DBVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvDatabase::*Handler)( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );

	DBVarLenMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		(FvDatabase::Instance().*handler_)( srcAddr, header, data );
	}

	Handler handler_;
};

#ifdef FV_DBMGR_SELFTEST
#include <FvMemoryStream.h>

class SelfTest : public FvIDatabase::IGetEntityHandler,
	public FvIDatabase::IPutEntityHandler,
	public FvIDatabase::IDelEntityHandler
{
	int				stepNum_;
	FvIDatabase&		db_;

	std::string		entityName_;
	std::string		badEntityName_;
	FvEntityTypeID	entityTypeID_;
	FvDatabaseID		newEntityID_;
	FvDatabaseID		badEntityID_;
	FvMemoryOStream	entityData_;
	FvEntityMailBoxRef entityBaseMB_;

	FvEntityDBKey		ekey_;
	FvEntityDBRecordOut outRec_;
	FvMemoryOStream	tmpEntityData_;
	FvEntityMailBoxRef tmpEntityBaseMB_;
	FvEntityMailBoxRef* pTmpEntityBaseMB_;

public:
	SelfTest( FvIDatabase& db ) :
	  stepNum_(0), db_(db),
		  entityName_("test_entity"), badEntityName_("Ben"), entityTypeID_(0),
		  newEntityID_(0), badEntityID_(0), entityData_(), entityBaseMB_(),
		  ekey_( 0, 0 ), outRec_(), tmpEntityData_(), tmpEntityBaseMB_(),
		  pTmpEntityBaseMB_(0)
	  {
		  entityBaseMB_.Init( 123, FvNetAddress( 7654321, 1234 ), FvEntityMailBoxRef::BASE, 1 );
	  }
	  virtual ~SelfTest() {}

	  void NextStep();

	  virtual FvEntityDBKey& GetKey()					{	return ekey_;	}
	  virtual FvEntityDBRecordOut& OutRec()			{	return outRec_;	}
	  virtual void OnGetEntityComplete( bool isOK );

	  virtual void OnPutEntityComplete( bool isOK, FvDatabaseID );

	  virtual void OnDelEntityComplete( bool isOK );
};

void SelfTest::NextStep()
{
	FV_TRACE_MSG( "SelfTest::NextStep - step %d\n", stepNum_ + 1);
	switch (++stepNum_)
	{
	case 1:
		{	// Create new entity
			FvMemoryOStream strm;
			bool isDefaultEntityOK =
				FvDatabase::Instance().DefaultEntityToStrm( entityTypeID_, entityName_, strm );
			FV_ASSERT( isDefaultEntityOK );

			FvEntityDBRecordIn erec;
			erec.ProvideStrm( strm );
			FvEntityDBKey	ekey( entityTypeID_, 0);
			db_.PutEntity( ekey, erec, *this );
			break;
		}
	case 2:
		{	// Check entity exists by name
			outRec_.UnprovideBaseMB();
			outRec_.UnprovideStrm();
			ekey_ = FvEntityDBKey( entityTypeID_, 0, entityName_ );
			db_.GetEntity( *this );
			break;
		}
	case 3:
		{	// Check entity not exists by name
			outRec_.UnprovideBaseMB();
			outRec_.UnprovideStrm();
			ekey_ = FvEntityDBKey( entityTypeID_, 0, badEntityName_ );
			db_.GetEntity( *this );
			break;
		}
	case 4:
		{	// Check entity exists by ID
			outRec_.UnprovideBaseMB();
			outRec_.UnprovideStrm();
			ekey_ = FvEntityDBKey( entityTypeID_, newEntityID_ );
			db_.GetEntity( *this );
			break;
		}
	case 5:
		{	// Check entity not exists by ID
			outRec_.UnprovideBaseMB();
			outRec_.UnprovideStrm();
			ekey_ = FvEntityDBKey( entityTypeID_, badEntityID_ );
			db_.GetEntity( *this );
			break;
		}
	case 6:
		{	// Get entity data by ID.
			outRec_.UnprovideBaseMB();
			outRec_.ProvideStrm( entityData_ );
			ekey_ = FvEntityDBKey( entityTypeID_, newEntityID_ );
			db_.GetEntity( *this );
			break;
		}
	case 7:
		{	// Get entity data by non-existent ID.
			outRec_.UnprovideBaseMB();
			tmpEntityData_.Reset();
			outRec_.ProvideStrm( tmpEntityData_ );
			ekey_ = FvEntityDBKey( entityTypeID_, badEntityID_ );
			db_.GetEntity( *this );
			break;
		}
	case 8:
		{	// Overwrite entity data
			FvEntityDBRecordIn erec;
			erec.ProvideStrm( entityData_ );
			FvEntityDBKey	ekey( entityTypeID_, newEntityID_ );
			db_.PutEntity( ekey, erec, *this );
			break;
		}
	case 9:
		{	// Get entity data by name.
			outRec_.UnprovideBaseMB();
			tmpEntityData_.Reset();
			outRec_.ProvideStrm( tmpEntityData_ );
			ekey_ = FvEntityDBKey( entityTypeID_, 0, entityName_ );
			db_.GetEntity( *this );
			break;
		}
	case 10:
		{	// Overwrite non-existent entity with new data
			FvEntityDBRecordIn erec;
			erec.ProvideStrm( entityData_ );
			FvEntityDBKey	ekey( entityTypeID_, badEntityID_ );
			db_.PutEntity( ekey, erec, *this );
			break;
		}
	case 11:
		{	// Get entity data with bad name.
			outRec_.UnprovideBaseMB();
			tmpEntityData_.Reset();
			outRec_.ProvideStrm( tmpEntityData_ );
			ekey_ = FvEntityDBKey( entityTypeID_, 0, badEntityName_ );
			db_.GetEntity( *this );
			break;
		}
	case 12:
		{	// Get NULL base MB by ID
			pTmpEntityBaseMB_ = &tmpEntityBaseMB_;
			outRec_.ProvideBaseMB( pTmpEntityBaseMB_ );
			outRec_.UnprovideStrm();
			ekey_ = FvEntityDBKey( entityTypeID_, newEntityID_ );
			db_.GetEntity( *this );
			break;
		}
	case 13:
		{	// Get NULL base MB by bad ID
			pTmpEntityBaseMB_ = &tmpEntityBaseMB_;
			outRec_.ProvideBaseMB( pTmpEntityBaseMB_ );
			outRec_.UnprovideStrm();
			ekey_ = FvEntityDBKey( entityTypeID_, badEntityID_ );
			db_.GetEntity( *this );
			break;
		}
	case 14:
		{	// Put base MB with ID
			FvEntityMailBoxRef* pBaseRef = &entityBaseMB_;
			FvEntityDBRecordIn erec;
			erec.ProvideBaseMB( pBaseRef );
			FvEntityDBKey	ekey( entityTypeID_, newEntityID_ );
			db_.PutEntity( ekey, erec, *this );
			break;
		}
	case 15:
		{	// Put base MB with bad ID
			FvEntityMailBoxRef baseMB;
			FvEntityMailBoxRef* pBaseRef = &baseMB;
			FvEntityDBRecordIn erec;
			erec.ProvideBaseMB( pBaseRef );
			FvEntityDBKey	ekey( entityTypeID_, badEntityID_ );
			db_.PutEntity( ekey, erec, *this );
			break;
		}
	case 16:
		{	// Get base MB by ID
			tmpEntityBaseMB_.Init( 666, FvNetAddress( 66666666, 666 ), FvEntityMailBoxRef::CELL, 1 );
			pTmpEntityBaseMB_ = &tmpEntityBaseMB_;
			outRec_.ProvideBaseMB( pTmpEntityBaseMB_ );
			outRec_.UnprovideStrm();
			ekey_ = FvEntityDBKey( entityTypeID_, newEntityID_ );
			db_.GetEntity( *this );
			break;
		}
	case 17:
		{	// Update base MB by ID
			entityBaseMB_.m_iID = 999;
			FvEntityMailBoxRef* pBaseRef = &entityBaseMB_;
			FvEntityDBRecordIn erec;
			erec.ProvideBaseMB( pBaseRef );
			FvEntityDBKey	ekey( entityTypeID_, newEntityID_ );
			db_.PutEntity( ekey, erec, *this );
			break;
		}
	case 18:
		{	// Get entity data and base MB by ID.
			tmpEntityBaseMB_.Init( 666, FvNetAddress( 66666666, 666 ), FvEntityMailBoxRef::GLOBAL, 1 );
			pTmpEntityBaseMB_ = &tmpEntityBaseMB_;
			outRec_.ProvideBaseMB( pTmpEntityBaseMB_ );
			tmpEntityData_.Reset();
			outRec_.ProvideStrm( tmpEntityData_ );
			ekey_ = FvEntityDBKey( entityTypeID_, newEntityID_ );
			db_.GetEntity( *this );
			break;
		}
	case 19:
		{	// Put NULL base MB with ID
			FvEntityMailBoxRef* pBaseRef = 0;
			FvEntityDBRecordIn erec;
			erec.ProvideBaseMB( pBaseRef );
			FvEntityDBKey	ekey( entityTypeID_, newEntityID_ );
			db_.PutEntity( ekey, erec, *this );
			break;
		}
	case 20:
		{	// Get NULL base MB by name
			pTmpEntityBaseMB_ = &tmpEntityBaseMB_;
			outRec_.ProvideBaseMB( pTmpEntityBaseMB_ );
			outRec_.UnprovideStrm();
			ekey_ = FvEntityDBKey( entityTypeID_, 0, entityName_ );
			db_.GetEntity( *this );
			break;
		}
	case 21:
		{	// Get NULL base MB by bad name
			pTmpEntityBaseMB_ = &tmpEntityBaseMB_;
			outRec_.ProvideBaseMB( pTmpEntityBaseMB_ );
			outRec_.UnprovideStrm();
			ekey_ = FvEntityDBKey( entityTypeID_, 0, badEntityName_ );
			db_.GetEntity( *this );
			break;
		}
	case 22:
		{	// Delete entity by name
			FvEntityDBKey	ekey( entityTypeID_, 0, entityName_ );
			db_.DelEntity( ekey, *this );
			break;
		}
	case 23:
		{	// Create new entity by stream.
			FvEntityDBRecordIn erec;
			erec.ProvideStrm( entityData_ );
			FvEntityDBKey	ekey( entityTypeID_, 0 );
			db_.PutEntity( ekey, erec, *this );
			break;
		}
	case 24:
		{	// Get entity data by name (again)
			outRec_.UnprovideBaseMB();
			tmpEntityData_.Reset();
			outRec_.ProvideStrm( tmpEntityData_ );
			ekey_ = FvEntityDBKey( entityTypeID_, 0, entityName_ );
			db_.GetEntity( *this );
			break;
		}
	case 25:
		{	// Delete entity by ID
			FvEntityDBKey	ekey( entityTypeID_, newEntityID_ );
			db_.DelEntity( ekey, *this );
			break;
		}
	case 26:
		{	// Delete non-existent entity by name
			FvEntityDBKey	ekey( entityTypeID_, 0, entityName_ );
			db_.DelEntity( ekey, *this );
			break;
		}
	case 27:
		{	// Delete non-existent entity by ID
			FvEntityDBKey	ekey( entityTypeID_, newEntityID_ );
			db_.DelEntity( ekey, *this );
			break;
		}
	default:
		FV_TRACE_MSG( "SelfTest::NextStep - completed\n" );
		delete this;
		break;
	}
}

void SelfTest::OnGetEntityComplete( bool isOK )
{
	switch (stepNum_)
	{
	case 2:
		{
			bool checkEntityWithName = isOK;
			FV_ASSERT( checkEntityWithName && (ekey_.m_iDBID == newEntityID_) );
			break;
		}
	case 3:
		{
			bool checkEntityWithBadName = isOK;
			FV_ASSERT( !checkEntityWithBadName );
			break;
		}
	case 4:
		{
			bool checkEntityWithID = isOK;
			FV_ASSERT( checkEntityWithID && (ekey_.m_kName == entityName_) );
			break;
		}
	case 5:
		{
			bool checkEntityWithBadID = isOK;
			FV_ASSERT( !checkEntityWithBadID );
			break;
		}
	case 6:
		{
			bool getEntityDataWithID = isOK;
			FV_ASSERT( getEntityDataWithID );
			break;
		}
	case 7:
		{
			bool getEntityDataWithBadID = isOK;
			FV_ASSERT( !getEntityDataWithBadID );
			break;
		}
	case 9:
		{
			bool getEntityDataWithName = isOK;
			FV_ASSERT( getEntityDataWithName &&
				(entityData_.Size() == tmpEntityData_.Size()) &&
				(memcmp( entityData_.Data(), tmpEntityData_.Data(), entityData_.Size()) == 0) );
			break;
		}
	case 11:
		{
			bool getEntityDataWithBadName = isOK;
			FV_ASSERT( !getEntityDataWithBadName );
			break;
		}
	case 12:
		{
			bool getNullBaseMBWithID = isOK;
			FV_ASSERT( getNullBaseMBWithID && outRec_.GetBaseMB() == 0 );
			break;
		}
	case 13:
		{
			bool getNullBaseMBWithBadID = isOK;
			FV_ASSERT( !getNullBaseMBWithBadID );
			break;
		}
	case 16:
		{
			bool getBaseMBWithID = isOK;
			FV_ASSERT( getBaseMBWithID && outRec_.GetBaseMB() &&
				(tmpEntityBaseMB_.m_iID == entityBaseMB_.m_iID) &&
				(tmpEntityBaseMB_.GetType() == entityBaseMB_.GetType()) &&
				(tmpEntityBaseMB_.GetComponent() == entityBaseMB_.GetComponent()) &&
				(tmpEntityBaseMB_.m_kAddr == entityBaseMB_.m_kAddr) );
			break;
		}
	case 18:
		{
			bool getEntityWithID = isOK;
			FV_ASSERT( getEntityWithID &&
				(entityData_.Size() == tmpEntityData_.Size()) &&
				(memcmp( entityData_.Data(), tmpEntityData_.Data(), entityData_.Size()) == 0) &&
				outRec_.GetBaseMB() &&
				(tmpEntityBaseMB_.m_iID == entityBaseMB_.m_iID) &&
				(tmpEntityBaseMB_.GetType() == entityBaseMB_.GetType()) &&
				(tmpEntityBaseMB_.GetComponent() == entityBaseMB_.GetComponent()) &&
				(tmpEntityBaseMB_.m_kAddr == entityBaseMB_.m_kAddr) );
			break;
		}
	case 20:
		{
			bool getNullBaseMBWithName = isOK;
			FV_ASSERT( getNullBaseMBWithName && outRec_.GetBaseMB() == 0 );
			break;
		}
	case 21:
		{
			bool getNullBaseMBWithBadName = isOK;
			FV_ASSERT( !getNullBaseMBWithBadName );
			break;
		}
	case 24:
		{
			bool getEntityDataWithName = isOK;
			FV_ASSERT( getEntityDataWithName &&
				(entityData_.Size() == tmpEntityData_.Size()) &&
				(memcmp( entityData_.Data(), tmpEntityData_.Data(), entityData_.Size()) == 0) );
			break;
		}
	default:
		FV_ASSERT( false );
		break;
	}

	this->NextStep();
}

void SelfTest::OnPutEntityComplete( bool isOK, FvDatabaseID dbID )
{
	switch (stepNum_)
	{
	case 1:
		{
			bool createNewEntity = isOK;
			FV_ASSERT( createNewEntity );
			newEntityID_ = dbID;
			badEntityID_ = newEntityID_ + 9999;
			break;
		}
	case 8:
		{
			bool overwriteEntityData = isOK;
			FV_ASSERT( overwriteEntityData );
			entityData_.Rewind();
			break;
		}
	case 10:
		{
			bool putNonExistEntityData = isOK;
			FV_ASSERT( !putNonExistEntityData );
			entityData_.Rewind();
			break;
		}
	case 14:
		{
			bool putBaseMBwithID = isOK;;
			FV_ASSERT( putBaseMBwithID );
			break;
		}
	case 15:
		{
			bool putBaseMBwithBadID = isOK;
			FV_ASSERT( !putBaseMBwithBadID );
			break;
		}
	case 17:
		{
			bool updateBaseMBwithID = isOK;
			FV_ASSERT( updateBaseMBwithID );
			break;
		}
	case 19:
		{
			bool putNullBaseMBwithID = isOK;
			FV_ASSERT( putNullBaseMBwithID );
			break;
		}
	case 23:
		{
			bool putEntityData = isOK;
			FV_ASSERT( putEntityData && (dbID != 0) && (newEntityID_ != dbID) );
			newEntityID_ = dbID;

			entityData_.Rewind();
			break;
		}
	default:
		FV_ASSERT( false );
		break;
	}

	this->NextStep();
}

void SelfTest::OnDelEntityComplete( bool isOK )
{
	switch (stepNum_)
	{
	case 22:
		{
			bool delEntityByName = isOK;
			FV_ASSERT( delEntityByName );
			break;
		}
	case 25:
		{
			bool delEntityByID = isOK;
			FV_ASSERT( delEntityByID );
			break;
		}
	case 26:
		{
			bool delEntityByBadName = isOK;
			FV_ASSERT( !delEntityByBadName );
			break;
		}
	case 27:
		{
			bool delNonExistEntityByID = isOK;
			FV_ASSERT( !delNonExistEntityByID );
			break;
		}
	default:
		FV_ASSERT( false );
		break;
	}
	this->NextStep();
}

void FvDatabase::RunSelfTest()
{
	SelfTest* selfTest = new SelfTest(*m_pkDatabase);
	selfTest->NextStep();
}
#endif	// FV_DBMGR_SELFTEST

#define DEFINE_INTERFACE_HERE
#include <../FvBase/FvBaseAppIntInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvBaseAppManager/FvBaseAppManagerInterface.h>

#define DEFINE_SERVER_HERE
#include "FvDBInterface.h"