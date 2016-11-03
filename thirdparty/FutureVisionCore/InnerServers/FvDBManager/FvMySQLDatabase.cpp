#include "FvDBEntityDefs.h"
#include "FvMySQLDatabase.h"
#include "FvMySQLThread.h"
#include "FvDBEntityRecoverer.h"
#include "FvMySQLTypeMapping.h"
#include "FvMySQLWrapper.h"
#include "FvMySQLNotPrepared.h"
#include "FvMySQLNamedLock.h"
#include "FvDatabase.h"
#include "FvDBInterfaceUtils.h"
#include "FvDBConfig.h"

#include <../FvBaseAppManager/FvBaseAppManagerInterface.h>
#include <FvDebug.h>
#include <FvWatcher.h>
#include <FvMD5.h>
#include <FvServerConfig.h>

FV_DECLARE_DEBUG_COMPONENT(0)

FvUInt32 InitInfoTable( MySql& connection )
{
	std::vector<FvString> tableNames;
	connection.GetTableNames( tableNames, "FutureVisionEntityTypes" );
	bool brandNewDb = (tableNames.size() == 0);

	MySqlTransaction transaction( connection );
	bool hasInfoTable = !brandNewDb;
	if (hasInfoTable)
	{
		MySqlTableMetadata infoTableMetadata( connection, "FutureVisionInfo" );
		hasInfoTable = infoTableMetadata.IsValid();
	}

	if (!hasInfoTable)
	{
#ifdef FV_ENABLE_TABLE_SCHEMA_ALTERATIONS
		transaction.Execute( "CREATE TABLE IF NOT EXISTS FutureVisionInfo "
				"(version INT UNSIGNED NOT NULL,snapshotTime TIMESTAMP NULL) "
				"ENGINE="MYSQL_ENGINE_TYPE );
#else
		throw std::runtime_error( "Cannot create FutureVisionInfo table because "
				"FV_ENABLE_TABLE_SCHEMA_ALTERATIONS is not enabled" );
#endif
	}

	MySqlStatement	stmtGetVersion( connection,
									"SELECT version FROM FutureVisionInfo" );
	FvUInt32			version = 0;
	MySqlBindings	b;
	b << version;
	stmtGetVersion.BindResult( b );

	transaction.Execute( stmtGetVersion );
	if (stmtGetVersion.ResultRows() > 0)
	{
		stmtGetVersion.Fetch();
	}
	else
	{
		version = brandNewDb ? DBMGR_CURRENT_VERSION : 0;
		std::stringstream ss;
		ss << "INSERT INTO FutureVisionInfo (version) VALUES (" <<
				version << ")";
		transaction.Execute( ss.str() );
	}

	transaction.Commit();

	return version;
}


MySqlDatabase::MySqlDatabase() :
	m_pkThreadResPool( 0 ),
	m_iMaxSpaceDataSize( 2048 ),
	m_iNumConnections( 5 ),
	m_iNumWriteSpaceOpsInProgress( 0 ),
	m_kReconnectTimerID( FV_NET_TIMER_ID_NONE ),
	m_stReconnectCount( 0 )
{
	FV_WATCH( "performance/numBusyThreads", *this,
				&MySqlDatabase::WatcherGetNumBusyThreads );
	FV_WATCH( "performance/busyThreadsMaxElapsed", *this,
				&MySqlDatabase::WatcherGetBusyThreadsMaxElapsedSecs );
	FV_WATCH( "performance/allOperations/rate", *this,
				&MySqlDatabase::WatcherGetAllOpsCountPerSec );
	FV_WATCH( "performance/allOperations/duration", *this,
				&MySqlDatabase::WatcherGetAllOpsAvgDurationSecs );
}

MySqlDatabase * MySqlDatabase::Create()
{
	try
	{
		MySqlDatabase * pDatabase = new MySqlDatabase();
		return pDatabase;
	}
	catch (std::exception & e)
	{
		const char* pMsg = e.what();
		return NULL;
	}
}

MySqlDatabase::~MySqlDatabase()
{
}

bool MySqlDatabase::Startup( const FvEntityDefs& entityDefs,
		bool isFaultRecovery, bool isUpgrade, bool isSyncTablesToDefsCmd )
{
	FV_ASSERT( !(isFaultRecovery && isUpgrade) );

	#ifdef FV_USE_MYSQL_PREPARED_STATEMENTS
		FV_INFO_MSG( "\tMySql: Compiled for prepared statements = True.\n" );
	#else
		FV_INFO_MSG( "\tMySql: Compiled for prepared statements = False.\n" );
	#endif

	bool isSyncTablesToDefsCfg =
							FvServerConfig::Get( "DBManager/syncTablesToDefs", false );

	this->PrintConfigStatus();

	try
	{
		const FvDBConfigConnection& connectionInfo =
				FvDatabase::Instance().GetServerConfig().GetCurServer().m_kConnectionInfo;
		MySql connection( connectionInfo );

		MySQL::NamedLock dbLock( connection,
				connectionInfo.GenerateLockName(), false );
		if (!dbLock.Lock())
		{
			FV_ERROR_MSG( "MySqlDatabase::Startup: Database %s on %s:%d is being "
					"used by another FutureVision process\n",
					connectionInfo.m_kDatabase.c_str(),
					connectionInfo.m_kHost.c_str(),
					connectionInfo.m_uiPort );
			return false;
		}

#ifndef FV_ENABLE_TABLE_SCHEMA_ALTERATIONS
		if (isUpgrade)
		{
			FV_ERROR_MSG( "MySqlDatabase::init: "
						"This build of DBManager does not support the --upgrade "
						"option.\n"
						"Please rebuild DBManager with "
						"FV_ENABLE_TABLE_SCHEMA_ALTERATIONS enabled\n");
			return false;
		}

		if (isSyncTablesToDefsCmd)
		{
			FV_ERROR_MSG( "MySqlDatabase::init: "
						"This build of DBManager does not support the "
						"--sync-tables-to-defs option.\n"
						"Please rebuild DBManager with "
						"FV_ENABLE_TABLE_SCHEMA_ALTERATIONS enabled\n");
			return false;
		}

		if (isSyncTablesToDefsCfg)
		{
			FV_ERROR_MSG( "MySqlDatabase::init: "
						"This build of DBManager does not support the "
						"syncTablesToDefs option.\n"
						"Please disable the DBManager/syncTablesToDefs "
						"configuration or rebuild DBManager with "
						"FV_ENABLE_TABLE_SCHEMA_ALTERATIONS enabled.\n");
			return false;
		}
#endif

		FvUInt32 version = InitInfoTable( connection );
		if (version < DBMGR_OLDEST_SUPPORTED_VERSION)
		{
			FV_ERROR_MSG( "Cannot use database created by an ancient version "
					"of FutureVision\n" );
			return false;
		}
		else if ( (version < DBMGR_CURRENT_VERSION) && !isUpgrade )
		{
			FV_ERROR_MSG( "Cannot use database from previous versions of FutureVision "
						"without upgrade\n" );
			FV_INFO_MSG( "Database can be upgraded by running dbmgr --upgrade\n" );
			return false;
		}
		else if (version > DBMGR_CURRENT_VERSION)
		{
			FV_ERROR_MSG( "Cannot use database from newer version of FutureVision\n" );
			return false;
		}
		else if ( (version == DBMGR_CURRENT_VERSION) && isUpgrade )
		{
			FV_WARNING_MSG( "Database version is current, ignoring --upgrade option\n" );
		}

		m_iMaxSpaceDataSize = std::max( FvServerConfig::Get( "DBManager/maxSpaceDataSize",
										m_iMaxSpaceDataSize ), 1 );

		if (!isFaultRecovery)
		{
			if (!this->CheckSpecialFutureVisionTables( connection ))
			{
#ifdef FV_ENABLE_TABLE_SCHEMA_ALTERATIONS
				this->CreateSpecialFutureVisionTables( connection );
#else
				throw std::runtime_error( "FutureVision internal tables do not "
						"meet requirements. Please re-initialise tables with "
						"a DBManager built with FV_ENABLE_TABLE_SCHEMA_ALTERATIONS "
						"enabled" );
#endif
			}

			bool hasUnconsolidatedDBs =
					(this->GetNumSecondaryDBs( connection ) > 0);
			if (hasUnconsolidatedDBs)
			{
				if (isSyncTablesToDefsCmd)
				{
					FV_ERROR_MSG( "MySqlDatabase::Startup: Cannot "
								"syncTablesToDefs when there are "
								"unconsolidated secondary databases\n" );
					return false;
				}

				isSyncTablesToDefsCfg = false;

			}

			bool shouldSyncTablesToDefs =
					isSyncTablesToDefsCmd || isSyncTablesToDefsCfg || isUpgrade;
			bool isEntityTablesInSync =
					InitEntityTables( connection, entityDefs, version,
							shouldSyncTablesToDefs );

			if (!isEntityTablesInSync)
			{
				if (hasUnconsolidatedDBs)
				{
					FV_ERROR_MSG( "MySqlDatabase::Startup: "
								"Entity definitions were changed while there "
								"are unconsolidated secondary databases.\n"
								"Please revert changes to entity definitions "
								"and run the data consolidation tool.\n"
								"Alternatively, run \"consolidate_dbs --clear\""
								" to allow the server to run without doing "
								"data consolidation. Unconsolidated data "
								"will be lost.\n" );
				}
				else
				{
					FV_ASSERT( !isSyncTablesToDefsCmd );
					FV_ERROR_MSG( "MySqlDatabase::Startup: "
								"Tables not in sync with entity definitions.\n"
								"Please run dbmgr with --sync-tables-to-defs "
								"option to update tables\n" );
				}
				return false;
			}

			this->InitSpecialFutureVisionTables( connection, entityDefs );

			if (FvDatabase::Instance().ClearRecoveryDataOnStartUp())
			{
				MySqlTransaction t( connection );

				t.Execute( "DELETE FROM FutureVisionLogOns" );

				t.Execute( "DELETE FROM FutureVisionSpaces" );
				t.Execute( "DELETE FROM FutureVisionSpaceData" );

				//! TODO: 关服流程未完善,先不清除时间,保证重开服务器后时间ok
				//t.Execute( "UPDATE FutureVisionGameTime SET time=0" );

				t.Commit();
			}
		}

		m_iNumConnections = std::max( FvServerConfig::Get( "DBManager/numConnections",
													m_iNumConnections ), 1 );

		FV_INFO_MSG( "\tMySql: Number of connections = %d.\n", m_iNumConnections );

		FV_VERIFY( dbLock.Unlock() );

		m_pkThreadResPool =
			new MySqlThreadResPool( FvDatabase::Instance().GetWorkerThreadMgr(),
				FvDatabase::Instance().nub(),
				m_iNumConnections, m_iMaxSpaceDataSize, connectionInfo, entityDefs );
	}
	catch (std::exception& e)
	{
		FV_ERROR_MSG( "MySqlDatabase::Startup: %s\n", e.what() );
		return false;
	}

	return true;
}

bool MySqlDatabase::ShutDown()
{
	try
	{
		delete m_pkThreadResPool;
		m_pkThreadResPool = NULL;

		if ( m_kReconnectTimerID != FV_NET_TIMER_ID_NONE )
		{
			FvDatabase::Instance().nub().CancelTimer( m_kReconnectTimerID );
			m_kReconnectTimerID = FV_NET_TIMER_ID_NONE;
		}

		return true;
	}
	catch (std::exception& e)
	{
		FV_ERROR_MSG( "MySqlDatabase::ShutDown: %s\n", e.what() );
		return false;
	}
}

void MySqlDatabase::PrintConfigStatus()
{
	FV_INFO_MSG( "\tMySql: Configured MySQL servers:\n" );
	FvDBConfigServer& config = this->GetServerConfig();
	do
	{
		const FvDBConfigServer::ServerInfo& serverInfo = config.GetCurServer();
		const char * failedString;
		try
		{
			MySql connection( serverInfo.m_kConnectionInfo );
			failedString = "";

			if (serverInfo.m_kConnectionInfo.m_kPassword.length() == 0)
			{
				FV_WARNING_MSG( "Connection to MySQL database '%s:%d/%s' has no "
						"password specified. This is a potential security "
						"risk.\n",
						serverInfo.m_kConnectionInfo.m_kHost.c_str(),
						serverInfo.m_kConnectionInfo.m_uiPort,
						serverInfo.m_kConnectionInfo.m_kDatabase.c_str() );
			}
		}
		catch (std::exception& e)
		{
			const char* pMsg = e.what();
			failedString = " - FAILED!";
		}
		FV_INFO_MSG( "\t\t%s: %s:%d (%s)%s\n",
					serverInfo.m_kConfigName.c_str(),
					serverInfo.m_kConnectionInfo.m_kHost.c_str(),
					serverInfo.m_kConnectionInfo.m_uiPort,
					serverInfo.m_kConnectionInfo.m_kDatabase.c_str(),
					failedString );
	} while (config.GotoNextServer());
}

FV_INLINE MySqlThreadData& MySqlDatabase::GetMainThreadData()
{
	return m_pkThreadResPool->GetMainThreadData();
}


FV_INLINE FvDBConfigServer& MySqlDatabase::GetServerConfig()
{
	return FvDatabase::Instance().GetServerConfig();
}

void MySqlDatabase::OnConnectionFatalError()
{
	if (!this->HasFatalConnectionError())
	{
		if (this->GetServerConfig().GetNumServers() == 1)
		{
			m_kReconnectTimerID =
				FvDatabase::Instance().nub().RegisterTimer( 1000000, this );
		}
		else
		{
			m_kReconnectTimerID =
				FvDatabase::Instance().nub().RegisterTimer( 1, this );
		}
		m_stReconnectCount = 0;
	}
}

bool MySqlDatabase::RestoreConnectionToDb()
{
	FV_ASSERT( this->HasFatalConnectionError() );

	++m_stReconnectCount;

	FvDBConfigServer& config = this->GetServerConfig();
	bool isSuccessful;
	if (config.GetNumServers() == 1)
	{
		isSuccessful = this->GetMainThreadData().m_kConnection.Ping();
	}
	else
	{
		config.GotoNextServer();
		isSuccessful = true;

		if (m_stReconnectCount == config.GetNumServers())
		{
			FvDatabase::Instance().nub().CancelTimer( m_kReconnectTimerID );
			m_kReconnectTimerID =
				FvDatabase::Instance().nub().RegisterTimer( 1000000, this );
		}
	}

	if (isSuccessful)
	{
		MySqlThreadResPool* pOldThreadResPool = m_pkThreadResPool;
		pOldThreadResPool->ThreadPool().WaitForAllTasks();
		const FvDBConfigServer::ServerInfo& curServer =
				this->GetServerConfig().GetCurServer();
		try
		{
			m_pkThreadResPool =
				new MySqlThreadResPool( FvDatabase::Instance().GetWorkerThreadMgr(),
					FvDatabase::Instance().nub(),
					m_iNumConnections, m_iMaxSpaceDataSize,
					curServer.m_kConnectionInfo,
					FvDatabase::Instance().GetEntityDefs(),
					pOldThreadResPool->IsDBLocked() );

			if (isSuccessful)
			{
				FvDatabase::Instance().nub().CancelTimer( m_kReconnectTimerID );
				m_kReconnectTimerID = FV_NET_TIMER_ID_NONE;

				delete pOldThreadResPool;

				FV_INFO_MSG( "MySqlDatabase: %s - Reconnected to database\n",
						 curServer.m_kConfigName.c_str() );
			}
			else
			{
				delete m_pkThreadResPool;
				m_pkThreadResPool = pOldThreadResPool;
			}
		}
		catch (std::exception& e)
		{
			FV_ERROR_MSG( "MySqlDatabase::RestoreConnectionToDb: %s - %s\n",
						curServer.m_kConfigName.c_str(),
						e.what() );

			if (m_pkThreadResPool != pOldThreadResPool)
				delete m_pkThreadResPool;
			m_pkThreadResPool = pOldThreadResPool;

			isSuccessful = false;
		}
	}

	return isSuccessful;
}

int MySqlDatabase::HandleTimeout( FvNetTimerID id, void * arg )
{
	this->RestoreConnectionToDb();

	return 0;
}

unsigned int MySqlDatabase::WatcherGetNumBusyThreads() const
{
	return (m_pkThreadResPool) ?
				(unsigned int) m_pkThreadResPool->ThreadPool().GetNumBusyThreads() : 0;
}

double MySqlDatabase::WatcherGetBusyThreadsMaxElapsedSecs() const
{
	return (m_pkThreadResPool) ?
				m_pkThreadResPool->GetBusyThreadsMaxElapsedSecs() : 0;
}

double MySqlDatabase::WatcherGetAllOpsCountPerSec() const
{
	return (m_pkThreadResPool) ?
				m_pkThreadResPool->GetOpCountPerSec() : 0;
}

double MySqlDatabase::WatcherGetAllOpsAvgDurationSecs() const
{
	return (m_pkThreadResPool) ? m_pkThreadResPool->GetAvgOpDuration() : 0;
}


class MapLoginToEntityDBKeyTask : public MySqlThreadTask
{
	FvString m_kLogOnName;
	FvString m_kPassword;
	FvDatabaseLoginStatus m_kLoginStatus;
	FvIDatabase::IMapLoginToEntityDBKeyHandler &m_kHandler;

public:
	MapLoginToEntityDBKeyTask( MySqlDatabase& owner,
		const FvString& logOnName, const FvString& password,
		FvIDatabase::IMapLoginToEntityDBKeyHandler& handler );

	virtual void Run();
	virtual void OnRunComplete();
};

MapLoginToEntityDBKeyTask::MapLoginToEntityDBKeyTask( MySqlDatabase& owner,
	const FvString& logOnName, const FvString& password,
	FvIDatabase::IMapLoginToEntityDBKeyHandler& handler )
	: MySqlThreadTask(owner), m_kLogOnName(logOnName), m_kPassword(password),
	m_kLoginStatus( FvDatabaseLoginStatus::LOGGED_ON ), m_kHandler(handler)
{
	this->StartThreadTaskTiming();

	MySqlThreadData& threadData = this->GetThreadData();
	threadData.m_kEntityDBKey.m_uiTypeID = 0;
	threadData.m_kEntityDBKey.m_iDBID = 0;
	threadData.m_kEntityDBKey.m_kName.clear();
	threadData.m_kExceptionStr.clear();
}

void MapLoginToEntityDBKeyTask::Run()
{
	bool retry;
	do
	{
		retry = false;
		MySqlThreadData& threadData = this->GetThreadData();
		try
		{
			MySqlTransaction transaction( threadData.m_kConnection );
			FvString actualPassword;
			bool entryExists = threadData.m_kTypeMapping.GetLogOnMapping( transaction,
				m_kLogOnName, actualPassword, threadData.m_kEntityDBKey.m_uiTypeID,
				threadData.m_kEntityDBKey.m_kName );
			if (entryExists)
			{
				if (!actualPassword.empty() && m_kPassword != actualPassword)
					m_kLoginStatus = FvDatabaseLoginStatus::LOGIN_REJECTED_INVALID_PASSWORD;
			}
			else
			{
				m_kLoginStatus = FvDatabaseLoginStatus::LOGIN_REJECTED_NO_SUCH_USER;
			}
			transaction.Commit();
		}
		catch (MySqlRetryTransactionException& e)
		{
			const char* pMsg = e.what();
			retry = true;
		}
		catch (std::exception& e)
		{
			threadData.m_kExceptionStr = e.what();
			m_kLoginStatus = FvDatabaseLoginStatus::LOGIN_REJECTED_DB_GENERAL_FAILURE;
		}
	} while (retry);
}

void MapLoginToEntityDBKeyTask::OnRunComplete()
{
	MySqlThreadData& threadData = this->GetThreadData();
	if (threadData.m_kExceptionStr.length())
	{
		FV_ERROR_MSG( "MySqlDatabase::MapLoginToEntityDBKey: %s\n",
		           threadData.m_kExceptionStr.c_str() );
	}
	else if (threadData.m_kConnection.HasFatalError())
	{
		m_kLoginStatus = FvDatabaseLoginStatus::LOGIN_REJECTED_DB_GENERAL_FAILURE;
	}

	FvUInt64 duration = this->StopThreadTaskTiming();
	if (duration > THREAD_TASK_WARNING_DURATION)
		FV_WARNING_MSG( "MapLoginToEntityDBKeyTask for '%s' took %f seconds\n",
					m_kLogOnName.c_str(), double(duration)/StampsPerSecondD() );

	FvDatabaseLoginStatus	loginStatus = m_kLoginStatus;
	FvEntityDBKey			ekey = threadData.m_kEntityDBKey;
	FvIDatabase::IMapLoginToEntityDBKeyHandler& handler = m_kHandler;
	delete this;

	handler.OnMapLoginToEntityDBKeyComplete( loginStatus, ekey );
}


void MySqlDatabase::MapLoginToEntityDBKey(
		const FvString & logOnName,
		const FvString & password,
		FvIDatabase::IMapLoginToEntityDBKeyHandler& handler )
{
	MapLoginToEntityDBKeyTask* pTask =
		new MapLoginToEntityDBKeyTask( *this, logOnName, password, handler );
	pTask->DoTask();
}

class SetLoginMappingTask : public MySqlThreadTask
{
	FvIDatabase::ISetLoginMappingHandler &m_kHandler;

public:
	SetLoginMappingTask( MySqlDatabase& owner, const FvString& username,
		const FvString & password, const FvEntityDBKey& ekey,
		FvIDatabase::ISetLoginMappingHandler& handler )
		: MySqlThreadTask(owner), m_kHandler(handler)
	{
		this->StartThreadTaskTiming();

		MySqlThreadData& threadData = this->GetThreadData();

		threadData.m_kTypeMapping.LogOnMappingToBound( username, password,
			ekey.m_uiTypeID, ekey.m_kName );

		threadData.m_kExceptionStr.clear();
	}

	virtual void Run();
	virtual void OnRunComplete();
};

void SetLoginMappingTask::Run()
{
	MySqlThreadData& threadData = this->GetThreadData();
	bool retry;
	do
	{
		retry = false;
		try
		{
			MySqlTransaction transaction( threadData.m_kConnection );
			threadData.m_kTypeMapping.SetLogOnMapping( transaction );

			transaction.Commit();
		}
		catch (MySqlRetryTransactionException& e)
		{
			const char* pMsg = e.what();
			retry = true;
		}
		catch (std::exception& e)
		{
			threadData.m_kExceptionStr = e.what();
		}
	} while (retry);
}

void SetLoginMappingTask::OnRunComplete()
{
	MySqlThreadData& threadData = this->GetThreadData();
	if (threadData.m_kExceptionStr.length())
	{
		FV_ERROR_MSG( "MySqlDatabase::SetLoginMapping: %s\n",
		           threadData.m_kExceptionStr.c_str() );
	}

	FvUInt64 duration = this->StopThreadTaskTiming();
	if (duration > THREAD_TASK_WARNING_DURATION)
		FV_WARNING_MSG( "SetLoginMappingTask for '%s' took %f seconds\n",
					threadData.m_kTypeMapping.GetBoundLogOnName().c_str(),
					double(duration)/StampsPerSecondD() );

	FvIDatabase::ISetLoginMappingHandler& handler = m_kHandler;
	delete this;

	handler.OnSetLoginMappingComplete();
}

void MySqlDatabase::SetLoginMapping( const FvString& username,
	const FvString& password, const FvEntityDBKey& ekey,
	FvIDatabase::ISetLoginMappingHandler& handler )
{
	SetLoginMappingTask* pTask =
		new SetLoginMappingTask( *this, username, password, ekey, handler );
	pTask->DoTask();
}

class GetEntityTask : public MySqlThreadTask
{
	FvIDatabase::IGetEntityHandler &m_kHandler;

public:
	GetEntityTask( MySqlDatabase& owner,
		FvIDatabase::IGetEntityHandler& handler );

	virtual void Run();
	virtual void OnRunComplete();

private:
	bool FillKey( MySqlTransaction& transaction, FvEntityDBKey& ekey );
};

GetEntityTask::GetEntityTask( MySqlDatabase& owner,
		FvIDatabase::IGetEntityHandler& handler )
	: MySqlThreadTask( owner ), m_kHandler( handler )
{
	this->StartThreadTaskTiming();
}

void GetEntityTask::Run()
{
	MySqlThreadData& threadData = this->GetThreadData();
	bool isOK = true;
	threadData.m_kExceptionStr.clear();
	try
	{
		MySqlTransaction	transaction( threadData.m_kConnection );
		MySqlTypeMapping&	typeMapping = threadData.m_kTypeMapping;
		FvEntityDBKey&		ekey = m_kHandler.GetKey();
		FvEntityDBRecordOut&	erec = m_kHandler.OutRec();
		bool				definitelyExists = false;
		if (erec.IsStrmProvided())
		{
			definitelyExists = typeMapping.GetEntityToBound( transaction, ekey );
			isOK = definitelyExists;
		}

		if (isOK && erec.IsBaseMBProvided() && erec.GetBaseMB())
		{
			if (!definitelyExists)
				isOK = this->FillKey( transaction, ekey );

			if (isOK)
			{
				definitelyExists = true;
				if (!typeMapping.GetLogOnRecord( transaction, ekey.m_uiTypeID,
					ekey.m_iDBID, *erec.GetBaseMB() ) )
					erec.SetBaseMB( 0 );

			}
		}

		if (isOK && !definitelyExists)
		{	
			isOK = this->FillKey( transaction, ekey );
		}
		transaction.Commit();
	}
	catch (std::exception& e)
	{
		threadData.m_kExceptionStr = e.what();
		isOK = false;
	}

	threadData.m_bIsOK = isOK;
}

void GetEntityTask::OnRunComplete()
{
	MySqlThreadData& threadData = this->GetThreadData();
	if (threadData.m_kExceptionStr.length() > 0)
		FV_ERROR_MSG( "MySqlDatabase::GetEntity: %s\n",
			threadData.m_kExceptionStr.c_str() );
	else if (threadData.m_kConnection.HasFatalError())
		threadData.m_bIsOK = false;

	if ( threadData.m_bIsOK )
	{
		FvEntityDBRecordOut&	erec = m_kHandler.OutRec();
		if (erec.IsStrmProvided())
		{
			FvEntityDBKey& ekey = m_kHandler.GetKey();
			threadData.m_kTypeMapping.BoundToStream( ekey.m_uiTypeID, erec.GetStrm(),
				m_kHandler.GetPasswordOverride() );
		}
	}

	const FvEntityDBKey& ekey = m_kHandler.GetKey();
	FvUInt64 duration = this->StopThreadTaskTiming();
	if (duration > THREAD_TASK_WARNING_DURATION)
		FV_WARNING_MSG( "GetEntityTask for entity %"FMT_DBID" of type %d "
					"named '%s' took %f seconds\n",
					ekey.m_iDBID, ekey.m_uiTypeID, ekey.m_kName.c_str(),
					double(duration)/StampsPerSecondD() );

	FvIDatabase::IGetEntityHandler& handler = m_kHandler;
	bool isOK = threadData.m_bIsOK;
	delete this;

	handler.OnGetEntityComplete( isOK );
}

bool GetEntityTask::FillKey( MySqlTransaction& transaction,
	FvEntityDBKey& ekey )
{
	MySqlTypeMapping& typeMapping = this->GetThreadData().m_kTypeMapping;
	bool isOK;
	if (typeMapping.HasNameProp( ekey.m_uiTypeID ))
	{
		if (ekey.m_iDBID)
		{
			isOK = typeMapping.GetEntityName( transaction, ekey.m_uiTypeID,
				ekey.m_iDBID, ekey.m_kName );
		}
		else
		{
			ekey.m_iDBID = typeMapping.getEntityDbID( transaction,
						ekey.m_uiTypeID, ekey.m_kName );
			isOK = ekey.m_iDBID != 0;
		}
	}
	else
	{
		if (ekey.m_iDBID)
		{
			isOK = typeMapping.CheckEntityExists( transaction, ekey.m_uiTypeID,
				ekey.m_iDBID );
			ekey.m_kName.clear();
		}
		else
		{
			isOK = false;
		}
	}

	return isOK;
}

void MySqlDatabase::GetEntity( FvIDatabase::IGetEntityHandler& handler )
{
	GetEntityTask*	pGetEntityTask = new GetEntityTask( *this, handler );
	pGetEntityTask->DoTask();
}

class PutEntityTask : public MySqlThreadTask
{
	enum BaseRefAction
	{
		BaseRefActionNone,
		BaseRefActionWrite,
		BaseRefActionRemove
	};

	bool m_bWriteEntityData;
	BaseRefAction m_kBaseRefAction;
	FvIDatabase::IPutEntityHandler &m_kHandler;

public:
	PutEntityTask( MySqlDatabase& owner,
				   const FvEntityDBKey& ekey, FvEntityDBRecordIn& erec,
				   FvIDatabase::IPutEntityHandler& handler );

	virtual void Run();
	virtual void OnRunComplete();
};

PutEntityTask::PutEntityTask( MySqlDatabase& owner,
							  const FvEntityDBKey& ekey,
							  FvEntityDBRecordIn& erec,
							  FvIDatabase::IPutEntityHandler& handler )
	: MySqlThreadTask( owner ), m_bWriteEntityData( false ),
	m_kBaseRefAction( BaseRefActionNone ), m_kHandler( handler )
{
	this->StartThreadTaskTiming();

	MySqlThreadData& threadData = this->GetThreadData();

	threadData.m_kEntityDBKey = ekey;
	threadData.m_bIsOK = true;
	threadData.m_kExceptionStr.clear();

	if (erec.IsStrmProvided())
	{
		threadData.m_kTypeMapping.StreamToBound( ekey.m_uiTypeID, ekey.m_iDBID,
												erec.GetStrm() );
		m_bWriteEntityData = true;
	}

	if (erec.IsBaseMBProvided())
	{
		FvEntityMailBoxRef* pBaseMB = erec.GetBaseMB();
		if (pBaseMB)
		{
			threadData.m_kTypeMapping.BaseRefToBound( *pBaseMB );
			m_kBaseRefAction =  BaseRefActionWrite;
		}
		else
		{
			m_kBaseRefAction = BaseRefActionRemove;
		}
	}
}

void PutEntityTask::Run()
{
	MySqlThreadData& 	threadData = this->GetThreadData();
	bool 				retry;
	do
	{
		retry = false;
		try
		{
			FvDatabaseID		dbID = threadData.m_kEntityDBKey.m_iDBID;
			FvEntityTypeID		typeID = threadData.m_kEntityDBKey.m_uiTypeID;
			bool				isOK = threadData.m_bIsOK;
			MySqlTransaction	transaction( threadData.m_kConnection );
			bool				definitelyExists = false;

			if (m_bWriteEntityData)
			{
				if (dbID)
				{
					isOK = threadData.m_kTypeMapping.UpdateEntity( transaction,
																typeID );
				}
				else
				{
					dbID = threadData.m_kTypeMapping.NewEntity( transaction, typeID );
					isOK = (dbID != 0);
				}

				definitelyExists = isOK;
			}

			if (isOK && m_kBaseRefAction != BaseRefActionNone)
			{
				if (!definitelyExists)
				{
					isOK = threadData.m_kTypeMapping.CheckEntityExists( transaction,
								typeID, dbID );
				}

				if (isOK)
				{
					if (m_kBaseRefAction == BaseRefActionWrite)
					{
						threadData.m_kTypeMapping.AddLogOnRecord( transaction,
								typeID, dbID );
					}
					else
					{
						threadData.m_kTypeMapping.RemoveLogOnRecord( transaction,
								typeID, dbID );
						if (transaction.AffectedRows() == 0)
						{
							threadData.m_kExceptionStr = "Failed to remove logon record";
						}
					}
				}
			}

			transaction.Commit();

			threadData.m_kEntityDBKey.m_iDBID = dbID;
			threadData.m_bIsOK = isOK;

		}
		catch (MySqlRetryTransactionException& e)
		{
			const char* pMsg = e.what();
			retry = true;
		}
		catch (std::exception& e)
		{
			threadData.m_kExceptionStr = e.what();
			threadData.m_bIsOK = false;
		}
	} while (retry);
}

void PutEntityTask::OnRunComplete()
{
	MySqlThreadData& threadData = this->GetThreadData();
	if (threadData.m_kExceptionStr.length())
		FV_ERROR_MSG( "MySqlDatabase::PutEntity: %s\n", threadData.m_kExceptionStr.c_str() );
	else if (threadData.m_kConnection.HasFatalError())
		threadData.m_bIsOK = false;
	else if (!threadData.m_bIsOK)
		FV_WARNING_MSG( "MySqlDatabase::PutEntity: Failed to write entity %"FMT_DBID
					" of type %d into MySQL database.\n",
					threadData.m_kEntityDBKey.m_iDBID, threadData.m_kEntityDBKey.m_uiTypeID  );

	FvUInt64 duration = this->StopThreadTaskTiming();
	if (duration > THREAD_TASK_WARNING_DURATION)
		FV_WARNING_MSG( "PutEntityTask for entity %"FMT_DBID" of type %d "
					"took %f seconds\n",
					threadData.m_kEntityDBKey.m_iDBID, threadData.m_kEntityDBKey.m_uiTypeID,
					double(duration)/StampsPerSecondD() );

	bool isOK = threadData.m_bIsOK;
	FvDatabaseID dbID = threadData.m_kEntityDBKey.m_iDBID;
	FvIDatabase::IPutEntityHandler& handler = m_kHandler;
	delete this;

	handler.OnPutEntityComplete( isOK, dbID );
}

void MySqlDatabase::PutEntity( const FvEntityDBKey& ekey, FvEntityDBRecordIn& erec,
							   IPutEntityHandler& handler )
{
	FV_ASSERT( erec.IsStrmProvided() || erec.IsBaseMBProvided() );

	PutEntityTask* pTask =
		new PutEntityTask( *this, ekey, erec, handler );
	pTask->DoTask();
}

class DelEntityTask : public MySqlThreadTask
{
	FvIDatabase::IDelEntityHandler &m_kHandler;

public:
	DelEntityTask( MySqlDatabase& owner, const FvEntityDBKey& ekey,
		           FvIDatabase::IDelEntityHandler& handler );

	virtual void Run();
	virtual void OnRunComplete();
};

DelEntityTask::DelEntityTask( MySqlDatabase& owner, const FvEntityDBKey& ekey,
		FvIDatabase::IDelEntityHandler& handler )
	: MySqlThreadTask(owner), m_kHandler(handler)
{
	this->StartThreadTaskTiming();

	MySqlThreadData& threadData = this->GetThreadData();

	threadData.m_kEntityDBKey = ekey;
	threadData.m_bIsOK = true;
	threadData.m_kExceptionStr.clear();
}

void DelEntityTask::Run()
{
	MySqlThreadData& 	threadData = this->GetThreadData();
	bool 				retry;
	do
	{
		retry = false;
		FvEntityDBKey&		ekey = threadData.m_kEntityDBKey;
		MySqlTypeMapping&	typeMapping = threadData.m_kTypeMapping;
		try
		{
			MySqlTransaction transaction( threadData.m_kConnection );

			if (ekey.m_iDBID == 0)
			{
				ekey.m_iDBID = threadData.m_kTypeMapping.getEntityDbID( transaction,
					ekey.m_uiTypeID, ekey.m_kName );
			}

			if (ekey.m_iDBID)
			{
				if (typeMapping.DeleteEntityWithID( transaction, ekey.m_uiTypeID,
					ekey.m_iDBID ))
				{
					typeMapping.RemoveLogOnRecord( transaction, ekey.m_uiTypeID, ekey.m_iDBID );
				}
				else
				{
					threadData.m_bIsOK = false;
				}
				transaction.Commit();
			}
			else
			{
				threadData.m_bIsOK = false;
			}
		}
		catch (MySqlRetryTransactionException& e)
		{
			const char* pMsg = e.what();
			retry = true;
		}
		catch (std::exception& e)
		{
			threadData.m_kExceptionStr = e.what();
			threadData.m_bIsOK = false;
		}
	} while (retry);
}

void DelEntityTask::OnRunComplete()
{
	MySqlThreadData& threadData = this->GetThreadData();

	if (threadData.m_kExceptionStr.length())
		FV_ERROR_MSG( "MySqlDatabase::DelEntity: %s\n", threadData.m_kExceptionStr.c_str() );
	else if (threadData.m_kConnection.HasFatalError())
		threadData.m_bIsOK = false;

	FvUInt64 duration = this->StopThreadTaskTiming();
	if (duration > THREAD_TASK_WARNING_DURATION)
		FV_WARNING_MSG( "DelEntityTask for entity %"FMT_DBID" of type %d "
					"took %f seconds\n",
					threadData.m_kEntityDBKey.m_iDBID, threadData.m_kEntityDBKey.m_uiTypeID,
					double(duration)/StampsPerSecondD() );

	bool isOK = threadData.m_bIsOK;
	FvIDatabase::IDelEntityHandler& handler = m_kHandler;
	delete this;

	handler.OnDelEntityComplete(isOK);
}

void MySqlDatabase::DelEntity( const FvEntityDBKey & ekey,
	FvIDatabase::IDelEntityHandler& handler )
{
	DelEntityTask* pTask =
			new DelEntityTask( *this, ekey, handler );
	pTask->DoTask();
}

class ExecuteRawCommandTask : public MySqlThreadTask
{
	FvString m_kCommand;
	FvIDatabase::IExecuteRawCommandHandler &m_kHandler;

public:
	ExecuteRawCommandTask( MySqlDatabase& owner, const FvString& command,
		FvIDatabase::IExecuteRawCommandHandler& handler )
		: MySqlThreadTask(owner), m_kCommand(command), m_kHandler(handler)
	{
		this->StartThreadTaskTiming();

		MySqlThreadData& threadData = this->GetThreadData();

		threadData.m_bIsOK = true;
		threadData.m_kExceptionStr.clear();
	}

	virtual void Run();
	virtual void OnRunComplete();
};

void ExecuteRawCommandTask::Run()
{
	MySqlThreadData& threadData = this->GetThreadData();

	bool retry;
	do
	{
		retry = false;

		int errorNum;
		MySqlTransaction transaction( threadData.m_kConnection, errorNum );
		if (errorNum == 0)
		{
			errorNum = transaction.Query( m_kCommand );
			if (errorNum == 0)
			{
				MYSQL_RES* pResult = transaction.StoreResult();
				if (pResult)
				{
					MySqlResult result( *pResult );
					FvBinaryOStream& stream = m_kHandler.Response();
					stream << FvString();	
					stream << FvUInt32( result.GetNumFields() );
					stream << FvUInt32( result.GetNumRows() );
					while ( result.GetNextRow() )
					{
						for ( unsigned int i = 0; i < result.GetNumFields(); ++i )
						{
							FvDBInterfaceUtils::AddPotentialNullBlobToStream(
									stream, FvDBInterfaceUtils::Blob(
										result.GetField( i ),
										result.GetFieldLen( i ) ) );
						}
					}
				}
				else
				{
					errorNum = transaction.GetLastErrorNum();
					if (errorNum == 0)
					{
						FvBinaryOStream& stream = m_kHandler.Response();
						stream << FvString();
						stream << FvUInt32( 0 );
						stream << FvUInt64( transaction.AffectedRows() );
					}
				}
			}
		}

		if (errorNum != 0)
		{
			if (transaction.ShouldRetry())
			{
				retry = true;
			}
			else
			{
				threadData.m_kExceptionStr = transaction.GetLastError();
				threadData.m_bIsOK = false;

				m_kHandler.Response() << threadData.m_kExceptionStr;
			}
		}
		else
		{
			transaction.Commit();
		}
	} while (retry);
}

void ExecuteRawCommandTask::OnRunComplete()
{
	MySqlThreadData& threadData = this->GetThreadData();
	if (threadData.m_kExceptionStr.length())
		FV_ERROR_MSG( "MySqlDatabase::ExecuteRawCommand: %s\n",
			threadData.m_kExceptionStr.c_str() );

	FvUInt64 duration = this->StopThreadTaskTiming();
	if (duration > THREAD_TASK_WARNING_DURATION)
		FV_WARNING_MSG( "ExecuteRawCommandTask for '%s' took %f seconds\n",
					m_kCommand.c_str(), double(duration)/StampsPerSecondD() );

	FvIDatabase::IExecuteRawCommandHandler& handler = m_kHandler;
	delete this;

	handler.OnExecuteRawCommandComplete();
}

void MySqlDatabase::ExecuteRawCommand( const FvString & command,
	FvIDatabase::IExecuteRawCommandHandler& handler )
{
	ExecuteRawCommandTask* pTask =
		new ExecuteRawCommandTask( *this, command, handler );
	pTask->DoTask();
}

class PutIDsTask : public MySqlThreadTask
{
	int m_iNumIDs;
	FvEntityID *m_pkIDs;

public:
	PutIDsTask( MySqlDatabase& owner, int numIDs, const FvEntityID * ids )
		: MySqlThreadTask(owner), m_iNumIDs(numIDs), m_pkIDs(new FvEntityID[numIDs])
	{
		this->StartThreadTaskTiming();

		memcpy( m_pkIDs, ids, sizeof(FvEntityID)*numIDs );

		this->GetThreadData().m_kExceptionStr.clear();
	}
	virtual ~PutIDsTask()
	{
		delete [] m_pkIDs;
	}

	virtual void Run();
	virtual void OnRunComplete();
};

void PutIDsTask::Run()
{
	MySqlThreadData& threadData = this->GetThreadData();
	bool retry;
	do
	{
		retry = false;
		try
		{
			MySqlTransaction transaction( threadData.m_kConnection );

			const FvEntityID * ids = m_pkIDs;
			const FvEntityID * end = m_pkIDs + m_iNumIDs;
			while (ids != end)
			{
				threadData.m_iBoundID = *ids++;
				threadData.m_kConnection.Execute( *threadData.m_spPutIDStatement );
			}

			transaction.Commit();
		}
		catch (MySqlRetryTransactionException& e)
		{
			const char* pMsg = e.what();
			retry = true;
		}
		catch (std::exception& e)
		{
			threadData.m_kExceptionStr = e.what();
		}
	} while (retry);
}

void PutIDsTask::OnRunComplete()
{
	MySqlThreadData& threadData = this->GetThreadData();
	if (threadData.m_kExceptionStr.length())
		FV_ERROR_MSG( "MySqlDatabase::PutIDs: %s\n",
			threadData.m_kExceptionStr.c_str() );

	FvUInt64 duration = this->StopThreadTaskTiming();
	if (duration > THREAD_TASK_WARNING_DURATION)
		FV_WARNING_MSG( "PutIDsTask for %d IDs took %f seconds\n",
					m_iNumIDs, double(duration)/StampsPerSecondD() );

	delete this;
}

void MySqlDatabase::PutIDs( int numIDs, const FvEntityID * ids )
{
	PutIDsTask* pTask = new PutIDsTask( *this, numIDs, ids );
	pTask->DoTask();
}

class GetIDsTask : public MySqlThreadTask
{
	int m_iNumIDs;
	FvIDatabase::IGetIDsHandler &m_kHandler;

public:
	GetIDsTask( MySqlDatabase& owner, int numIDs,
		FvIDatabase::IGetIDsHandler& handler )
		: MySqlThreadTask(owner), m_iNumIDs(numIDs), m_kHandler(handler)
	{
		this->StartThreadTaskTiming();
		this->GetThreadData().m_kExceptionStr.clear();
	}

	virtual void Run();
	virtual void OnRunComplete();
};

void GetIDsTask::Run()
{
	MySqlThreadData& threadData = this->GetThreadData();
	bool retry;
	do
	{
		retry = false;
		try
		{
			MySqlTransaction transaction( threadData.m_kConnection );

			FvBinaryOStream& strm = m_kHandler.GetIDStrm();

			threadData.m_iBoundLimit = m_iNumIDs;
			threadData.m_kConnection.Execute( *threadData.m_spGetIDsStatement );
			int numIDsRetrieved = threadData.m_spGetIDsStatement->ResultRows();
			while (threadData.m_spGetIDsStatement->Fetch())
			{
				strm << threadData.m_iBoundID;
			}
			if (numIDsRetrieved > 0)
				threadData.m_kConnection.Execute( *threadData.m_spDelIDsStatement );
			threadData.m_iBoundLimit = m_iNumIDs - numIDsRetrieved;
			if (threadData.m_iBoundLimit)
			{
				threadData.m_kConnection.Execute( *threadData.m_spIncIDStatement );
				threadData.m_kConnection.Execute( *threadData.m_spGetIDStatement );
				threadData.m_spGetIDStatement->Fetch();
				while (numIDsRetrieved++ < m_iNumIDs)
				{
					strm << --threadData.m_iBoundID;
				}
			}

			transaction.Commit();
		}
		catch (MySqlRetryTransactionException& e)
		{
			const char* pMsg = e.what();
			retry = true;
			m_kHandler.ResetStrm();
		}
		catch (std::exception& e)
		{
			threadData.m_kExceptionStr = e.what();
		}
	} while (retry);
}

void GetIDsTask::OnRunComplete()
{
	MySqlThreadData& threadData = this->GetThreadData();
	if (threadData.m_kExceptionStr.length())
		FV_ERROR_MSG( "MySqlDatabase::GetIDs: %s\n",
			threadData.m_kExceptionStr.c_str() );

	FvUInt64 duration = this->StopThreadTaskTiming();
	if (duration > THREAD_TASK_WARNING_DURATION)
		FV_WARNING_MSG( "GetIDsTask for %d IDs took %f seconds\n",
					m_iNumIDs, double(duration)/StampsPerSecondD() );

	FvIDatabase::IGetIDsHandler& handler = m_kHandler;
	delete this;

	handler.OnGetIDsComplete();
}

void MySqlDatabase::GetIDs( int numIDs, FvIDatabase::IGetIDsHandler& handler )
{
	GetIDsTask* pTask = new GetIDsTask( *this, numIDs, handler );
	pTask->DoTask();
}


class WriteSpaceDataTask : public MySqlThreadTask
{
	FvMemoryOStream	m_kData;
	FvUInt32 m_uiNumSpaces;

public:
	WriteSpaceDataTask( MySqlDatabase& owner, FvBinaryIStream& spaceData )
		: MySqlThreadTask( owner ), m_uiNumSpaces( 0 )
	{
		this->StartThreadTaskTiming();

		MySqlThreadData& threadData = this->GetThreadData();

		threadData.m_kExceptionStr.clear();
		m_kData.Transfer( spaceData, spaceData.RemainingLength() );

		owner.OnWriteSpaceOpStarted();
	}

	virtual void Run();
	virtual void OnRunComplete();
};

void WriteSpaceDataTask::Run()
{
	MySqlThreadData& threadData = this->GetThreadData();
	bool 				retry;
	do
	{
		retry = false;
		try
		{
			MySqlTransaction transaction( threadData.m_kConnection );

			transaction.Execute( *threadData.m_spDelSpaceIDsStatement );
			transaction.Execute( *threadData.m_spDelSpaceDataStatement );

			m_uiNumSpaces = WriteSpaceDataStreamToDB( threadData.m_kConnection,
					threadData.m_iBoundSpaceID,
					*threadData.m_spWriteSpaceStatement,
					threadData.m_kBoundSpaceData,
					*threadData.m_spWriteSpaceDataStatement,
					m_kData );

			transaction.Commit();
		}
		catch (MySqlRetryTransactionException& e)
		{
			const char* pMsg = e.what();
			retry = true;
			m_kData.Rewind();
		}
		catch (std::exception & e)
		{
			threadData.m_kExceptionStr = e.what();
		}
	} while (retry);

}

void WriteSpaceDataTask::OnRunComplete()
{
	MySqlThreadData& threadData = this->GetThreadData();
	if (threadData.m_kExceptionStr.length())
		FV_ERROR_MSG( "MySqlDatabase::WriteSpaceData: Execute failed (%s)\n",
				   threadData.m_kExceptionStr.c_str() );

	FvUInt64 duration = this->StopThreadTaskTiming();
	if (duration > THREAD_TASK_WARNING_DURATION)
		FV_WARNING_MSG( "WriteSpaceDataTask for %u spaces took %f seconds\n",
					this->m_uiNumSpaces, double(duration)/StampsPerSecondD() );

	MySqlDatabase& owner = this->GetOwner();
	delete this;

	owner.OnWriteSpaceOpCompleted();
}

void MySqlDatabase::WriteSpaceData( FvBinaryIStream& spaceData )
{
	WriteSpaceDataTask* pTask = new WriteSpaceDataTask( *this, spaceData );
	pTask->DoTask();
}

bool MySqlDatabase::GetSpacesData( FvBinaryOStream& strm )
{
	MySql& connection = this->GetMainThreadData().m_kConnection;

	try
	{
		MySqlBindings paramBindings;
		MySqlBindings resultBindings;

		MySqlStatement spacesStmt( connection,
								   "SELECT id from FutureVisionSpaces" );
		FvSpaceID spaceID;
		resultBindings << spaceID;
		spacesStmt.BindResult( resultBindings );

		MySqlStatement spaceDataStmt( connection,
			"SELECT spaceEntryID, entryKey, data "
					"FROM FutureVisionSpaceData where id = ?" );
		paramBindings << spaceID;
		spaceDataStmt.BindParams( paramBindings );

		FvUInt64 boundSpaceEntryID;
		FvUInt16 boundSpaceDataKey;
		MySqlBuffer boundSpaceData( m_iMaxSpaceDataSize );
		resultBindings.clear();
		resultBindings << boundSpaceEntryID;
		resultBindings << boundSpaceDataKey;
		resultBindings << boundSpaceData;
		spaceDataStmt.BindResult( resultBindings );

		connection.Execute( spacesStmt );

		int numSpaces = spacesStmt.ResultRows();
		std::vector< FvSpaceID > spaceIDs;
		spaceIDs.reserve( numSpaces );

		strm << numSpaces;

		FV_INFO_MSG( "MySqlDatabase::GetSpacesData: numSpaces = %d\n", numSpaces );

		for (int i = 0; i < numSpaces; ++i)
		{
			spacesStmt.Fetch();
			spaceIDs.push_back( spaceID );
		}

		for (size_t i = 0; i < spaceIDs.size(); ++i)
		{
			spaceID = spaceIDs[i];
			strm << spaceID;
			connection.Execute( spaceDataStmt );

			int numData = spaceDataStmt.ResultRows();
			strm << numData;

			for (int dataIndex = 0; dataIndex < numData; ++dataIndex)
			{
				spaceDataStmt.Fetch();
				strm << boundSpaceEntryID;
				strm << boundSpaceDataKey;
				strm << boundSpaceData.GetString();
			}
		}
	}
	catch (std::exception & e)
	{
		FV_ERROR_MSG( "MySqlDatabase::GetSpacesData: Failed to get spaces data: "
				"%s\n", e.what() );
		return false;
	}

	return true;
}

void MySqlDatabase::RestoreEntities( FvDBEntityRecoverer& recoverer )
{
	MySql& connection = this->GetMainThreadData().m_kConnection;

	try
	{
		std::map< int, FvEntityTypeID > typeTranslation;

		{
			MySqlStatement typeStmt( connection,
					"SELECT typeID, FutureVisionID FROM FutureVisionEntityTypes" );
			MySqlBindings resultBindings;
			int dbTypeID;
			FvEntityTypeID iFutureVisionTypeID;
			resultBindings << dbTypeID << iFutureVisionTypeID;
			typeStmt.BindResult( resultBindings );
			connection.Execute( typeStmt );

			int numResults = typeStmt.ResultRows();

			for (int i = 0; i < numResults; ++i)
			{
				typeStmt.Fetch();
				typeTranslation[ dbTypeID ] = iFutureVisionTypeID;
			}
		}

		{
			MySqlStatement logOnsStmt( connection,
					"SELECT databaseID, typeID from FutureVisionLogOns" );
			MySqlBindings resultBindings;
			FvDatabaseID dbID;
			int dbTypeID;
			resultBindings << dbID << dbTypeID;
			logOnsStmt.BindResult( resultBindings );

			connection.Execute( logOnsStmt );

			int numResults = logOnsStmt.ResultRows();

			if (numResults > 0)
			{
				recoverer.Reserve( numResults );

				for (int i = 0; i < numResults; ++i)
				{
					logOnsStmt.Fetch();
					FvEntityTypeID bwTypeID = typeTranslation[ dbTypeID ];
					recoverer.AddEntity( bwTypeID, dbID );
				}

				connection.Execute( "DELETE FROM FutureVisionLogOns" );
			}

			recoverer.Start();
		}
	}
	catch (std::exception & e)
	{
		FV_ERROR_MSG( "MySqlDatabase::restoreGameState: Restore entities failed (%s)\n",
				e.what() );
		recoverer.Abort();
	}
}

class SetGameTimeTask : public MySqlThreadTask
{
public:
	SetGameTimeTask( MySqlDatabase& owner, FvTimeStamp gameTime )
		: MySqlThreadTask(owner)
	{
		this->StartThreadTaskTiming();

		MySqlThreadData& threadData = this->GetThreadData();
		threadData.m_uiGameTime = gameTime;
		threadData.m_kExceptionStr.clear();
	}

	virtual void Run();
	virtual void OnRunComplete();
};

void SetGameTimeTask::Run()
{
	MySqlThreadData& threadData = this->GetThreadData();
	try
	{
		threadData.m_kConnection.Execute( *threadData.m_spSetGameTimeStatement );
	}
	catch (std::exception & e)
	{
		threadData.m_kExceptionStr = e.what();
	}
}

void SetGameTimeTask::OnRunComplete()
{
	MySqlThreadData& threadData = this->GetThreadData();
	if (threadData.m_kExceptionStr.length())
		FV_ERROR_MSG( "MySqlDatabase::SetGameTime: "
					"Execute failed for time %u (%s)\n",
					threadData.m_uiGameTime, threadData.m_kExceptionStr.c_str() );

	FvUInt64 duration = this->StopThreadTaskTiming();
	if (duration > THREAD_TASK_WARNING_DURATION)
		FV_WARNING_MSG( "SetGameTimeTask for game time %u took %f seconds\n",
					threadData.m_uiGameTime, double(duration)/StampsPerSecondD() );

	delete this;
}

void MySqlDatabase::SetGameTime( FvTimeStamp gameTime )
{
	SetGameTimeTask* pTask = new SetGameTimeTask( *this, gameTime );
	pTask->DoTask();
}

namespace MySQL
{

bool GetGameTime( MySql& connection, FvTimeStamp& gameTime )
{
	MySqlUnPrep::Statement stmt( connection,
			"SELECT * FROM FutureVisionGameTime" );
	MySqlUnPrep::Bindings bindings;
	bindings << gameTime;
	stmt.BindResult( bindings );

	connection.Execute( stmt );

	FV_ASSERT_DEV( stmt.ResultRows() == 1 );

	return stmt.Fetch();
}

bool GetMaxSecondaryDBAppID( MySql& connection, FvInt32& maxAppID )
{
	MySqlUnPrep::Statement stmt( connection,
			"SELECT MAX( appID ) FROM FutureVisionSecondaryDatabases" );
	MySqlUnPrep::Bindings bindings;
	MySqlValueWithNull< FvInt32 > maxAppIDBuf;
	bindings << maxAppIDBuf;
	stmt.BindResult( bindings );

	connection.Execute( stmt );

	bool isOK = stmt.Fetch();
	if (isOK && maxAppIDBuf.Get())
	{
		maxAppID = *maxAppIDBuf.Get();
	}

	return isOK;
}


class GetBaseAppMgrInitDataTask : public MySqlThreadTask
{
	FvIDatabase::IGetBaseAppMgrInitDataHandler &m_kHandler;
	FvTimeStamp	m_uiGameTime;
	FvInt32 m_iMaxAppID;

public:
	GetBaseAppMgrInitDataTask( MySqlDatabase& owner,
			FvIDatabase::IGetBaseAppMgrInitDataHandler& handler ) :
		MySqlThreadTask( owner ), m_kHandler( handler ),
		m_uiGameTime( 0 ), m_iMaxAppID( 0 )
	{
		this->MySqlThreadTask::StandardInit();
	}

	virtual void Run()
	{
		MySqlThreadData& threadData = this->GetThreadData();
		WrapInTransaction( threadData.m_kConnection, threadData, *this );
	}

	void Execute( MySql& connection, MySqlThreadData& threadData )
	{
		GetGameTime( connection, m_uiGameTime );
		GetMaxSecondaryDBAppID( connection, m_iMaxAppID );
	}

	virtual void OnRunComplete()
	{
		this->MySqlThreadTask::StandardOnRunComplete<
				GetBaseAppMgrInitDataTask >();

		FvIDatabase::IGetBaseAppMgrInitDataHandler& 	handler = m_kHandler;
		FvTimeStamp gameTime = m_uiGameTime;
		FvInt32 maxAppID = m_iMaxAppID;

		delete this;

		handler.OnGetBaseAppMgrInitDataComplete( gameTime, maxAppID );
	}

	static const char *ErrorMethodName()	{ return "GetBaseAppMgrInitData"; }
};

}

void MySqlDatabase::GetBaseAppMgrInitData(
		IGetBaseAppMgrInitDataHandler& handler )
{
	MySQL::GetBaseAppMgrInitDataTask* pTask =
			new MySQL::GetBaseAppMgrInitDataTask( *this, handler );
	pTask->DoTask();
}

void MySqlDatabase::RemapEntityMailboxes( const FvNetAddress& srcAddr,
		const FvBackupHash & destAddrs )
{
	try
	{
		MySql& connection = this->GetMainThreadData().m_kConnection;

		std::stringstream updateStmtStrm;
		updateStmtStrm << "UPDATE FutureVisionLogOns SET ip=?, port=? WHERE ip="
				<< ntohl( srcAddr.m_uiIP ) << " AND port=" << ntohs( srcAddr.m_uiPort )
				<< " AND ((((objectID * " << destAddrs.Prime()
				<< ") % 0x100000000) >> 8) % " << destAddrs.VirtualSize()
				<< ")=?";

		MySqlStatement updateStmt( connection, updateStmtStrm.str() );
		FvUInt32 	boundAddress;
		FvUInt16	boundPort;
		int		i;

		MySqlBindings params;
		params << boundAddress << boundPort << i;

		updateStmt.BindParams( params );

		MySqlThreadResPool& threadResPool = this->GetThreadResPool();
		threadResPool.ThreadPool().WaitForAllTasks();

		for (i = 0; i < int(destAddrs.Size()); ++i)
		{
			boundAddress = ntohl( destAddrs[i].m_uiIP );
			boundPort = ntohs( destAddrs[i].m_uiPort );

			connection.Execute( updateStmt );
		}
		for (i = int(destAddrs.Size()); i < int(destAddrs.VirtualSize()); ++i)
		{
			int realIdx = i/2;
			boundAddress = ntohl( destAddrs[realIdx].m_uiIP );
			boundPort = ntohs( destAddrs[realIdx].m_uiPort );

			connection.Execute( updateStmt );
		}
	}
	catch (std::exception& e)
	{
		FV_ERROR_MSG( "MySqlDatabase::RemapEntityMailboxes: Remap entity "
				"mailboxes failed (%s)\n", e.what() );
	}
}

namespace MySQL
{

template < class STATEMENT >
class SimpleSecondaryDBTask : public MySqlThreadTask
{
public:
	SimpleSecondaryDBTask( MySqlDatabase& owner,
			const FvIDatabase::SecondaryDBEntry& entry ) :
		MySqlThreadTask( owner )
	{
		this->MySqlThreadTask::StandardInit();

		this->GetThreadData().m_kSecondaryDBOps.EntryBuf().Set(
				ntohl( entry.m_kAddr.m_uiIP ), ntohs( entry.m_kAddr.m_uiPort ),
				entry.m_iAppID, entry.m_kLocation );
	}

	virtual void Run()
	{
		MySqlThreadData& threadData = this->GetThreadData();
		WrapInTransaction( threadData.m_kConnection, threadData,
				STATEMENT::GetStmt( threadData ) );
	}
	virtual void OnRunComplete()
	{
		this->MySqlThreadTask::StandardOnRunComplete< SimpleSecondaryDBTask >();
		delete this;
	}

	virtual FvString GetTaskInfo() const
	{
		return this->GetThreadData().m_kSecondaryDBOps.EntryBuf().GetAsString();
	}

	static const char *ErrorMethodName()
	{
		return STATEMENT::ErrorMethodName();
	}
};

struct AddSecondaryDBEntry
{
	static MySqlStatement &GetStmt( MySqlThreadData& threadData )
	{
		return threadData.m_kSecondaryDBOps.AddStmt( threadData.m_kConnection );
	}
	static const char *ErrorMethodName()	{ return "AddSecondaryDB"; }
};

void GetSecondaryDBEntries( MySql& connection,
		FvIDatabase::SecondaryDBEntries& entries,
		const FvString& condition = FvString() )
{
	std::stringstream getStmtStrm;
	getStmtStrm << "SELECT ip,port,appID,location FROM "
			"FutureVisionSecondaryDatabases" << condition;

	MySqlUnPrep::Statement	getStmt( connection, getStmtStrm.str() );
	SecondaryDBOps::DbEntryBuffer buffer;
	MySqlUnPrep::Bindings	bindings;
	buffer.AddToBindings( bindings );
	getStmt.BindResult( bindings );

	connection.Execute( getStmt );

	while (getStmt.Fetch())
	{
		entries.push_back( FvIDatabase::SecondaryDBEntry(
				htonl( buffer.m_uiIP ),
				htons( buffer.m_uiPort ),
				buffer.m_iAppID,
				buffer.m_kLocation.GetString() ) );
	}
}

class UpdateSecondaryDBsTask : public MySqlThreadTask
{
	FvIDatabase::IUpdateSecondaryDBshandler &m_kHandler;
	FvString m_kCondition;
	std::auto_ptr< FvIDatabase::SecondaryDBEntries > m_spEntries;

public:
	UpdateSecondaryDBsTask( MySqlDatabase& owner, const FvBaseAppIDs& ids,
			FvIDatabase::IUpdateSecondaryDBshandler& handler ) :
		MySqlThreadTask( owner ), m_kHandler( handler ),
		m_spEntries( new FvIDatabase::SecondaryDBEntries )
	{
		this->MySqlThreadTask::StandardInit();

		if (!ids.empty())
		{
			std::stringstream conditionStrm;
			conditionStrm << " WHERE appID NOT IN (";
			FvBaseAppIDs::const_iterator iter = ids.begin();
			conditionStrm << *iter;
			for (; iter != ids.end(); ++iter)
			{
				conditionStrm << "," << *iter;
			}
			conditionStrm << ')';
			m_kCondition = conditionStrm.str();
		}
	}

	virtual void Run()
	{
		MySqlThreadData& threadData = this->GetThreadData();
		WrapInTransaction( threadData.m_kConnection, threadData, *this );
	}

	void Execute( MySql& connection, MySqlThreadData& threadData )
	{
		GetSecondaryDBEntries( connection, *m_spEntries, m_kCondition );

		std::stringstream delStmtStrm_;
		delStmtStrm_ << "DELETE FROM FutureVisionSecondaryDatabases" << m_kCondition;
		connection.Execute( delStmtStrm_.str() );
	}

	virtual void OnRunComplete()
	{
		this->MySqlThreadTask::StandardOnRunComplete< UpdateSecondaryDBsTask >();

		FvIDatabase::IUpdateSecondaryDBshandler&			handler( m_kHandler );
		std::auto_ptr< FvIDatabase::SecondaryDBEntries >	pEntries( m_spEntries );

		delete this;

		handler.OnUpdateSecondaryDBsComplete( *pEntries );
	}

	static const char *ErrorMethodName()	{ return "UpdateSecondaryDBs"; }
};

class GetSecondaryDBsTask : public MySqlThreadTask
{
	typedef FvIDatabase::IGetSecondaryDBsHandler Handler;

	Handler &m_kHandler;
	std::auto_ptr< FvIDatabase::SecondaryDBEntries > m_spEntries;

public:
	GetSecondaryDBsTask( MySqlDatabase& owner, Handler& handler ) :
		MySqlThreadTask( owner ), m_kHandler( handler ),
		m_spEntries( new FvIDatabase::SecondaryDBEntries )
	{
		this->MySqlThreadTask::StandardInit();
	}

	virtual void Run()
	{
		MySqlThreadData& threadData = this->GetThreadData();
		WrapInTransaction( threadData.m_kConnection, threadData, *this );
	}

	void Execute( MySql& connection, MySqlThreadData& threadData )
	{
		GetSecondaryDBEntries( connection, *m_spEntries );
	}

	virtual void OnRunComplete()
	{
		this->MySqlThreadTask::StandardOnRunComplete< GetSecondaryDBsTask >();

		Handler&										handler( m_kHandler );
		std::auto_ptr< FvIDatabase::SecondaryDBEntries > 	pEntries( m_spEntries );
		delete this;

		handler.OnGetSecondaryDBsComplete( *pEntries );
	}

	static const char *ErrorMethodName()	{ return "GetSecondaryDBs"; }
};

struct ClearSecondaryDBs
{
	int	m_iNumCleared;

	ClearSecondaryDBs(): m_iNumCleared( 0 ) {}

	void Execute( MySql& connection )
	{
		connection.Query( "DELETE FROM FutureVisionSecondaryDatabases");
		m_iNumCleared = int( connection.AffectedRows() );
	}

	void SetExceptionStr( const char * errorStr )
	{
		FV_ERROR_MSG( "MySqlDatabase::clearSecondaryDBs: %s\n", errorStr );
		m_iNumCleared = -1;
	}

} clearSecondaryDBs;

}

void MySqlDatabase::AddSecondaryDB( const SecondaryDBEntry& entry )
{
	typedef MySQL::SimpleSecondaryDBTask< MySQL::AddSecondaryDBEntry >
			AddSecondaryDBTask;
	AddSecondaryDBTask* pTask = new AddSecondaryDBTask( *this, entry );
	pTask->DoTask();
}

void MySqlDatabase::UpdateSecondaryDBs( const FvBaseAppIDs& ids,
		IUpdateSecondaryDBshandler& handler )
{
	MySQL::UpdateSecondaryDBsTask* pTask =
			new MySQL::UpdateSecondaryDBsTask( *this, ids, handler );
	pTask->DoTask();
}

void MySqlDatabase::GetSecondaryDBs( IGetSecondaryDBsHandler& handler )
{
	MySQL::GetSecondaryDBsTask* pTask =
			new MySQL::GetSecondaryDBsTask( *this, handler );
	pTask->DoTask();
}

FvUInt32 MySqlDatabase::GetNumSecondaryDBs()
{
	MySql& connection = this->GetMainThreadData().m_kConnection;

	return MySqlDatabase::GetNumSecondaryDBs( connection );
}

FvUInt32 MySqlDatabase::GetNumSecondaryDBs( MySql& connection )
{
	MySqlUnPrep::Statement getCountStmt( connection,
			"SELECT COUNT(*) FROM FutureVisionSecondaryDatabases" );

	FvUInt32 count;
	MySqlUnPrep::Bindings bindings;
	bindings << count;
	getCountStmt.BindResult( bindings );

	connection.Execute( getCountStmt );
	getCountStmt.Fetch();

	return count;
}

int MySqlDatabase::ClearSecondaryDBs()
{
	MySQL::ClearSecondaryDBs clearSecondaryDBs;

	MySqlThreadData& mainThreadData = this->GetMainThreadData();
	WrapInTransaction( mainThreadData.m_kConnection, clearSecondaryDBs );

	return clearSecondaryDBs.m_iNumCleared;
}

bool MySqlDatabase::LockDB()
{
	return m_pkThreadResPool->LockDB();
}

bool MySqlDatabase::UnlockDB()
{
	return m_pkThreadResPool->UnlockDB();
}


void MySqlDatabase::CreateSpecialFutureVisionTables( MySql& connection )
{
	char buffer[512];

	connection.Execute( "CREATE TABLE IF NOT EXISTS FutureVisionEntityTypes "
			 "(typeID INT NOT NULL AUTO_INCREMENT, FutureVisionID INT, "
			 "name CHAR(255) NOT NULL UNIQUE, PRIMARY KEY(typeID), "
			 "KEY(FutureVisionID)) ENGINE="MYSQL_ENGINE_TYPE );

	connection.Execute( "CREATE TABLE IF NOT EXISTS FutureVisionLogOns "
			 "(databaseID BIGINT NOT NULL, typeID INT NOT NULL, "
			 "objectID INT, ip INT UNSIGNED, port SMALLINT UNSIGNED, "
			 "salt SMALLINT UNSIGNED, PRIMARY KEY(typeID, databaseID)) "
			 "ENGINE="MYSQL_ENGINE_TYPE );
	FvSNPrintf( buffer, sizeof(buffer),
			"CREATE TABLE IF NOT EXISTS FutureVisionLogOnMapping "
			 "(logOnName VARCHAR(%d) NOT NULL, password VARCHAR(%d),"
			 " typeID INT NOT NULL, recordName VARCHAR(%d),"
			 " PRIMARY KEY(logOnName)) ENGINE="MYSQL_ENGINE_TYPE,
			 BWMySQLMaxLogOnNameLen, BWMySQLMaxLogOnPasswordLen,
			 BWMySQLMaxNamePropertyLen );
	connection.Execute( buffer );

	connection.Execute( "CREATE TABLE IF NOT EXISTS FutureVisionNewID "
					 "(id INT NOT NULL) ENGINE="MYSQL_ENGINE_TYPE );
	connection.Execute( "CREATE TABLE IF NOT EXISTS FutureVisionUsedIDs "
					 "(id INT NOT NULL) ENGINE="MYSQL_ENGINE_TYPE );

	connection.Execute( "CREATE TABLE IF NOT EXISTS FutureVisionGameTime "
					"(time INT NOT NULL) ENGINE="MYSQL_ENGINE_TYPE );

	const FvString& blobTypeName =
		MySqlTypeTraits<FvString>::colTypeStr( m_iMaxSpaceDataSize );
	connection.Execute( "CREATE TABLE IF NOT EXISTS FutureVisionSpaces "
			"(id INT NOT NULL UNIQUE) ENGINE="MYSQL_ENGINE_TYPE );
	FvSNPrintf( buffer, sizeof(buffer),
			"CREATE TABLE IF NOT EXISTS FutureVisionSpaceData "
			"(id INT NOT NULL, INDEX (id), "
			"spaceEntryID BIGINT NOT NULL, "
			"entryKey SMALLINT UNSIGNED NOT NULL, "
			"data %s NOT NULL ) ENGINE="MYSQL_ENGINE_TYPE,
			blobTypeName.c_str() );
	connection.Execute( buffer );

	FvSNPrintf( buffer, sizeof(buffer),
			"ALTER TABLE FutureVisionSpaceData MODIFY data %s",
			 blobTypeName.c_str() );
	connection.Execute( buffer );

	MySQL::SecondaryDBOps::CreateTable( connection );

	connection.Execute( "CREATE TABLE IF NOT EXISTS FutureVisionEntityDefsChecksum "
			 "(checksum CHAR(255)) ENGINE="MYSQL_ENGINE_TYPE );
}

void MySqlDatabase::InitSpecialFutureVisionTables( MySql& connection,
		const FvEntityDefs& entityDefs )
{
	MySqlTransaction transaction( connection );

	transaction.Execute( "DELETE FROM FutureVisionUsedIDs" );
	transaction.Execute( "DELETE FROM FutureVisionNewID" );
	transaction.Execute( "INSERT INTO FutureVisionNewID (id) VALUES (1)" );
	transaction.Execute( "DELETE FROM FutureVisionEntityDefsChecksum" );

	transaction.Execute( "INSERT INTO FutureVisionGameTime "
			"SELECT 0 FROM FutureVisionNewID "
			"WHERE NOT EXISTS(SELECT * FROM FutureVisionGameTime)" );

	//const FvMD5::Digest& digest = entityDefs.GetPersistentPropertiesDigest();
	//! modify by Uman, 20101119
	const FvMD5::Digest& digest = entityDefs.GetDigest();
	FvString stmt;

	stmt = "INSERT INTO FutureVisionEntityDefsChecksum VALUES ('";
	stmt += digest.Quote();
	stmt += "')";
	transaction.Execute( stmt );

	transaction.Commit();
}

bool MySqlDatabase::CheckSpecialFutureVisionTables( MySql& connection )
{
	struct CheckList
	{
		FvString		tableName;
		unsigned int	numColumns;
	};
	static const CheckList checkList[] =
	{	{ "FutureVisionEntityTypes", 3 }
	, 	{ "FutureVisionLogOns", 6 }
	, 	{ "FutureVisionLogOnMapping", 4 }
	, 	{ "FutureVisionNewID", 1 }
	, 	{ "FutureVisionUsedIDs", 1 }
	, 	{ "FutureVisionGameTime", 1 }
	, 	{ "FutureVisionSpaces", 1 }
	, 	{ "FutureVisionSpaceData", 4 }
	, 	{ "FutureVisionSecondaryDatabases", 4 }
	, 	{ "FutureVisionEntityDefsChecksum", 1 }
	};

	bool isOK = true;
	for (size_t i = 0; i < sizeof(checkList)/sizeof(CheckList); ++i)
	{
		MySqlTableMetadata	tableMetadata( connection, checkList[i].tableName );
		if (!tableMetadata.IsValid() ||
				(tableMetadata.GetNumFields() != checkList[i].numColumns))
		{
#ifndef FV_ENABLE_TABLE_SCHEMA_ALTERATIONS
			FV_INFO_MSG( "\tTable %s should have %u columns\n",
					checkList[i].tableName.c_str(),
					checkList[i].numColumns );
#endif
			isOK = false;
		}
	}

	return isOK;
}

