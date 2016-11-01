#include "FvMySQLThread.h"
#include "FvMySQLNotPrepared.h"
#include "FvDBConfig.h"

#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT( 0 );

MySqlBindings& operator<<( MySqlBindings& bindings, SpaceDataBinding& sdb )
{
	bindings << sdb.m_iSpaceKey;
	bindings << sdb.m_uiDataKey;
	bindings << sdb.m_kData;

	return bindings;
}

FvUInt32 WriteSpaceDataStreamToDB( MySql& connection,
		FvSpaceID& spaceIDBinding, MySqlStatement& insertSpaceIDStmt,
		SpaceDataBinding& spaceDataBinding, MySqlStatement& insertSpaceDataStmt,
		FvBinaryIStream& stream )
{
	FvUInt32 numSpaces;
	stream >> numSpaces;

	for (FvUInt32 spaceIndex = 0; spaceIndex < numSpaces; ++spaceIndex)
	{
		stream >> spaceIDBinding;

		connection.Execute( insertSpaceIDStmt );

		FvUInt32 numData;
		stream >> numData;

		for (FvUInt32 dataIndex = 0; dataIndex < numData; ++dataIndex)
		{
			stream >> spaceDataBinding.m_iSpaceKey;
			stream >> spaceDataBinding.m_uiDataKey;
			stream >> spaceDataBinding.m_kData;

			connection.Execute( insertSpaceDataStmt );
		}
	}

	return numSpaces;
}

namespace MySQL
{
FvString MySQL::SecondaryDBOps::DbEntryBuffer::GetAsString() const
{
	in_addr	addr;
	addr.s_addr = htonl( m_uiIP );
// xdonlee
#ifndef _WIN32
	char	addrStr[INET_ADDRSTRLEN];
	inet_ntop( AF_INET, &addr, addrStr, sizeof( addrStr ) );

	std::stringstream 	ss;
	ss << "addr=" << addrStr << ':' << m_uiPort << ", appID=" << m_iAppID
			<< ", m_kLocation=" << m_kLocation.GetString();
#else // !_WIN32
	std::stringstream 	ss;
	ss << "addr=" << inet_ntoa((in_addr&)m_uiIP) << ':' << m_uiPort << ", appID=" << m_iAppID
		<< ", location=" << m_kLocation.GetString();
#endif // _WIN32

	return ss.str();
}

void SecondaryDBOps::CreateTable( MySql& connection )
{
	FV_ASSERT( MAX_SECONDARY_DB_LOCATION_LENGTH < 1<<16 );

	connection.Execute( "CREATE TABLE IF NOT EXISTS "
			"FutureVisionSecondaryDatabases (ip INT UNSIGNED NOT NULL, "
			"port SMALLINT UNSIGNED NOT NULL, appID INT NOT NULL, "
			"location BLOB NOT NULL, INDEX addr (ip, port, appID)) "
			"ENGINE="MYSQL_ENGINE_TYPE );
}

MySqlStatement& SecondaryDBOps::AddStmt( MySql& connection )
{
	if (!m_spAddSTMT.get())
	{
		m_spAddSTMT.reset( new MySqlStatement( connection,
				"INSERT INTO FutureVisionSecondaryDatabases "
				"(ip, port, appID, location) VALUES (?,?,?,?)" ) );
		MySqlBindings bindings;
		m_kEntryBuf.AddToBindings( bindings );
		m_spAddSTMT->BindParams( bindings );
	}

	return *m_spAddSTMT;
}

}


MySqlThreadData::MySqlThreadData(  const FvDBConfigConnection& connInfo,
								   int maxSpaceDataSize,
                                   const FvEntityDefs& entityDefs,
                                   const char * tblNamePrefix )
	: m_kConnection( connInfo ),
	m_kTypeMapping( m_kConnection, entityDefs, tblNamePrefix ),
	m_uiStartTimestamp(0),
	m_spPutIDStatement( new MySqlStatement( m_kConnection,
							"INSERT INTO FutureVisionUsedIDs (id) VALUES (?)" ) ),
	m_spGetIDsStatement( new MySqlUnPrep::Statement( m_kConnection,
							"SELECT id FROM FutureVisionUsedIDs LIMIT ? FOR UPDATE" ) ),
	m_spDelIDsStatement( new MySqlUnPrep::Statement( m_kConnection,
							"DELETE FROM FutureVisionUsedIDs LIMIT ?" ) ),
	m_spIncIDStatement( new MySqlStatement( m_kConnection,
							"UPDATE FutureVisionNewID SET id=id+?" ) ),
	m_spGetIDStatement( new MySqlStatement( m_kConnection,
							"SELECT id FROM FutureVisionNewID LIMIT 1" ) ),
	m_spSetGameTimeStatement( new MySqlStatement( m_kConnection,
								"UPDATE FutureVisionGameTime SET time=?" ) ),
	m_kBoundSpaceData( maxSpaceDataSize ),
	m_spWriteSpaceStatement( new MySqlStatement( m_kConnection,
							"REPLACE INTO FutureVisionSpaces (id) "
							"VALUES (?)" ) ),
	m_spWriteSpaceDataStatement( new MySqlStatement( m_kConnection,
							"INSERT INTO FutureVisionSpaceData "
							"(id, spaceEntryID, entryKey, data) "
							"VALUES (?, ?, ?, ?)" ) ),
	m_spDelSpaceIDsStatement( new MySqlStatement( m_kConnection,
							"DELETE from FutureVisionSpaces" ) ),
	m_spDelSpaceDataStatement( new MySqlStatement( m_kConnection,
							"DELETE from FutureVisionSpaceData" ) ),
	m_kEntityDBKey( 0, 0 )
{
	MySqlBindings b;

	b << m_iBoundID;
	m_spPutIDStatement->BindParams( b );
	m_spGetIDStatement->BindResult( b );

	b.clear();
	b << m_iBoundLimit;
	m_spIncIDStatement->BindParams( b );

	b.clear();
	b << m_uiGameTime;
	m_spSetGameTimeStatement->BindParams( b );

	b.clear();
	b << m_iBoundSpaceID;
	m_spWriteSpaceStatement->BindParams( b );

	b.clear();
	b << m_iBoundSpaceID;
	b << m_kBoundSpaceData;
	m_spWriteSpaceDataStatement->BindParams( b );

	MySqlUnPrep::Bindings b2;
	b2 << m_iBoundID;
	m_spGetIDsStatement->BindResult( b2 );

	b2.clear();
	b2 << m_iBoundLimit;
	m_spGetIDsStatement->BindParams( b2 );
	m_spDelIDsStatement->BindParams( b2 );
}


class PingTask : public FvWorkerThread::ITask
{
	MySqlThreadResPool&	owner_;
	MySqlThreadData& 	threadData_;
	bool				m_bPingOK;

public:
	PingTask( MySqlThreadResPool& owner ) :
		owner_( owner ), threadData_( owner.AcquireThreadDataAlways() ),
		m_bPingOK( true )
	{
		owner_.StartThreadTaskTiming( threadData_ );
	}

	virtual ~PingTask()
	{
		FvUInt64 duration = owner_.StopThreadTaskTiming( threadData_ );
		if (duration > THREAD_TASK_WARNING_DURATION)
			FV_WARNING_MSG( "PingTask took %f seconds\n",
						double(duration) / StampsPerSecondD() );
		owner_.ReleaseThreadDataAlways( threadData_ );
	}

	virtual void Run()
	{
		m_bPingOK = threadData_.m_kConnection.Ping();
	}
	virtual void OnRunComplete()
	{
		if (!m_bPingOK)
		{
			MySqlError error(threadData_.m_kConnection.Get());
			FV_ERROR_MSG( "MySQL m_kConnection Ping failed: %s\n",
						error.what() );
			threadData_.m_kConnection.OnFatalError( error );
		}
		delete this;
	}
};


MySqlThreadResPool::MySqlThreadResPool( FvWorkerThreadManager& threadMgr,
										FvNetNub& nub,
									    int numConnections,
										int maxSpaceDataSize,
								        const FvDBConfigConnection& connInfo,
                                        const FvEntityDefs& entityDefs,
                                        bool shouldLockDB )
	: m_kThreadPool( threadMgr, numConnections - 1 ), m_kThreadDataPool(),
	m_kFreeThreadData(),
	m_kMainThreadData( connInfo, maxSpaceDataSize, entityDefs ),
	m_kDBLock( m_kMainThreadData.m_kConnection, connInfo.GenerateLockName(),
			shouldLockDB ),
	m_kNub( nub ),
	m_uiOPCount( 0 ), m_uiOPDurationTotal( 0 ), m_uiResetTimeStamp( Timestamp() )
{
	int numThreads = m_kThreadPool.GetNumFreeThreads();
	FV_ASSERT( (numThreads == 0) || ((numThreads > 0) && mysql_thread_safe()) );

	struct InitMySqlTask : public FvWorkerThread::ITask
	{
		virtual void Run()	{	mysql_thread_init();	}
		virtual void OnRunComplete() {}
	} initMySqlTask;
	while ( m_kThreadPool.DoTask( initMySqlTask ) ) {}
	m_kThreadPool.WaitForAllTasks();

	m_kThreadDataPool.container.reserve( numThreads );
	for ( int i = 0; i < numThreads; ++i )
	{
		m_kThreadDataPool.container.push_back(
			new MySqlThreadData( connInfo, maxSpaceDataSize, entityDefs ) );
	}

	m_kFreeThreadData = m_kThreadDataPool.container;

	m_kKeepAliveTimerID = m_kNub.RegisterTimer( 1800000000, this );
}

int MySqlThreadResPool::HandleTimeout( FvNetTimerID id, void * arg )
{
	FV_ASSERT( id == m_kKeepAliveTimerID );
	ThreadDataVector::size_type numFreeConnections = m_kFreeThreadData.size();
	for ( ThreadDataVector::size_type i = 0; i < numFreeConnections; ++i )
	{
		PingTask* pTask = new PingTask( *this );
		this->ThreadPool().DoTask( *pTask );
	}
	if (!m_kMainThreadData.m_kConnection.Ping())
		FV_ERROR_MSG( "MySQL m_kConnection Ping failed: %s\n",
					m_kMainThreadData.m_kConnection.GetLastError() );

	return 0;
}

MySqlThreadResPool::~MySqlThreadResPool()
{
	m_kNub.CancelTimer( m_kKeepAliveTimerID );

	m_kThreadPool.WaitForAllTasks();
	struct CleanupMySqlTask : public FvWorkerThread::ITask
	{
		virtual void Run()	{	mysql_thread_end();	}
		virtual void OnRunComplete() {}
	} cleanupMySqlTask;
	while ( m_kThreadPool.DoTask( cleanupMySqlTask ) ) {}
	m_kThreadPool.WaitForAllTasks();
}

FV_INLINE MySqlThreadData* MySqlThreadResPool::AcquireThreadDataInternal()
{
	MySqlThreadData* pThreadData = 0;
	if (m_kFreeThreadData.size() > 0)
	{
		pThreadData = m_kFreeThreadData.back();
		m_kFreeThreadData.pop_back();
	}
	return pThreadData;
}

MySqlThreadData* MySqlThreadResPool::AcquireThreadData(int timeoutMicroSeconds)
{
	MySqlThreadData* pThreadData = this->AcquireThreadDataInternal();
	if (!pThreadData && (timeoutMicroSeconds != 0))
	{
		m_kThreadPool.WaitForOneTask(timeoutMicroSeconds);
		pThreadData = this->AcquireThreadDataInternal();
	}

	return pThreadData;
}

void MySqlThreadResPool::ReleaseThreadData( MySqlThreadData& threadData )
{
	m_kFreeThreadData.push_back( &threadData );
	FV_ASSERT( m_kFreeThreadData.size() <= m_kThreadDataPool.container.size() );

	for ( ThreadDataProcJobs::iterator it = m_kThreadDataJobs.begin();
			it != m_kThreadDataJobs.end();  )
	{
		ThreadDataProcJobs::iterator cur = it;
		++it;

		this->ApplyThreadDataJob( threadData, *cur );
		if (cur->m_kUnprocessedItems.size() == 0)
		{
			cur->m_kProcessor.Done();
			m_kThreadDataJobs.erase( cur );
		}
	}
}

MySqlThreadData& MySqlThreadResPool::AcquireThreadDataAlways( int
		mainThreadThreshold )
{
	MySqlThreadData* pThreadData = this->AcquireThreadDataInternal();
	if (!pThreadData)
	{
		if (this->GetNumConnections() <= mainThreadThreshold)
		{
			pThreadData = &m_kMainThreadData;
		}
		else
		{
			m_kThreadPool.WaitForOneTask();
			pThreadData = this->AcquireThreadDataInternal();
			FV_ASSERT( pThreadData );
		}
	}
	return *pThreadData;
}

void MySqlThreadResPool::ReleaseThreadDataAlways( MySqlThreadData& threadData )
{
	if (!this->IsMainThreadData( threadData ))
	{
		this->ReleaseThreadData( threadData );
	}
}

void MySqlThreadResPool::DoTask( FvWorkerThread::ITask& task,
		MySqlThreadData& threadData )
{
	if (!threadData.m_kConnection.HasFatalError())
	{
		if (this->IsMainThreadData( threadData ))
		{
			FvWorkerThreadPool::DoTaskInCurrentThread( task );
		}
		else
		{
			FV_VERIFY( ThreadPool().DoTask( task ) );
		}
	}
	else
	{
		task.OnRunComplete();
	}
}

void MySqlThreadResPool::ApplyToAllThreadDatas( IThreadDataProcessor& processor )
{
	m_kThreadDataJobs.push_back( ThreadDataProcJob( processor,
								m_kThreadDataPool.container ) );

	ThreadDataProcJob& job = m_kThreadDataJobs.back();
	for ( ThreadDataVector::iterator it = m_kFreeThreadData.begin();
			it != m_kFreeThreadData.end(); ++it )
	{
		this->ApplyThreadDataJob( **it, job );
	}
	processor.Process( m_kMainThreadData );

	if (job.m_kUnprocessedItems.size() == 0)
	{
		processor.Done();
		m_kThreadDataJobs.pop_back();
	}
}

void MySqlThreadResPool::ApplyThreadDataJob( MySqlThreadData& threadData,
	ThreadDataProcJob& job )
{
	ThreadDataVector::iterator pFound =
			find( job.m_kUnprocessedItems.begin(), job.m_kUnprocessedItems.end(),
					&threadData );
	if (pFound != job.m_kUnprocessedItems.end())
	{
		job.m_kProcessor.Process( **pFound );
		job.m_kUnprocessedItems.erase( pFound );
	}
}

void MySqlThreadResPool::StartThreadTaskTiming( MySqlThreadData& threadData )
{
	threadData.m_uiStartTimestamp = Timestamp();
	if ((threadData.m_uiStartTimestamp - m_uiResetTimeStamp) > THREAD_TASK_TIMING_RESET_DURATION)
	{
		m_uiResetTimeStamp = threadData.m_uiStartTimestamp;
		m_uiOPDurationTotal = 0;
		m_uiOPCount = 0;
	}
}

FvUInt64 MySqlThreadResPool::StopThreadTaskTiming( MySqlThreadData& threadData )
{
	FvUInt64	opDuration = Timestamp() - threadData.m_uiStartTimestamp;
	m_uiOPDurationTotal += opDuration;
	++m_uiOPCount;
	threadData.m_uiStartTimestamp = 0;

	return opDuration;
}

double MySqlThreadResPool::GetBusyThreadsMaxElapsedSecs() const
{
	FvUInt64 maxElapsed = 0;
	FvUInt64 curTimestamp = Timestamp();
	for ( ThreadDataVector::const_iterator i = m_kThreadDataPool.container.begin();
			i != m_kThreadDataPool.container.end(); ++i )
	{
		if ((*i)->m_uiStartTimestamp > 0)
		{
			FvUInt64 elapsedTimestamp = curTimestamp - (*i)->m_uiStartTimestamp;
		 	if (elapsedTimestamp > maxElapsed)
		 		maxElapsed = elapsedTimestamp;
		}
	}
	return double(maxElapsed)/StampsPerSecondD();
}

double MySqlThreadResPool::GetOpCountPerSec() const
{
	return double(m_uiOPCount)/(double(Timestamp() - m_uiResetTimeStamp)/StampsPerSecondD());
}

double MySqlThreadResPool::GetAvgOpDuration() const
{
	return (m_uiOPCount > 0)
				? double(m_uiOPDurationTotal/m_uiOPCount)/StampsPerSecondD() : 0;
}
