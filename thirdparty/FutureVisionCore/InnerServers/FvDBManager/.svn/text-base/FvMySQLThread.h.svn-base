//{future header message}
#ifndef __FvMySQLThread_H__
#define __FvMySQLThread_H__

#include <list>

#include "FvWorkerThread.h"
#include "FvMySQLWrapper.h"
#include "FvMySQLTypeMapping.h"
#include "FvMySQLDatabase.h"
#include "FvMySQLNamedLock.h"

#define THREAD_TASK_WARNING_DURATION 		StampsPerSecond()
#define THREAD_TASK_TIMING_RESET_DURATION 	(5 * StampsPerSecond())

struct SpaceDataBinding
{
	FvInt64 m_iSpaceKey;
	FvUInt16 m_uiDataKey;
	MySqlBuffer	m_kData;

	SpaceDataBinding( int maxSpaceDataSize ) : m_kData( maxSpaceDataSize ) {}
};
MySqlBindings& operator<<( MySqlBindings& bindings, SpaceDataBinding& sdb );

FvUInt32 WriteSpaceDataStreamToDB( MySql& connection,
		FvSpaceID& spaceIDBinding, MySqlStatement& insertSpaceIDStmt,
		SpaceDataBinding& spaceDataBinding, MySqlStatement& insertSpaceDataStmt,
		FvBinaryIStream& stream );

namespace MySQL
{
	class SecondaryDBOps
	{
	public:
		struct DbEntryBuffer
		{
			FvUInt32 m_uiIP;
			FvUInt16 m_uiPort;
			FvInt32	m_iAppID;
			MySqlBuffer	m_kLocation;

			DbEntryBuffer() :
				m_uiIP( 0 ), m_uiPort( 0 ), m_kLocation( MAX_SECONDARY_DB_LOCATION_LENGTH )
			{}

			void Set( FvUInt32 ipAddr, FvUInt16	portNum, FvInt32 appId,
					const FvString loc )
			{
				m_uiIP = ipAddr;
				m_uiPort = portNum;
				m_iAppID = appId;
				m_kLocation.SetString( loc );
			}

			template <class BINDINGS>
			void AddToBindings( BINDINGS& bindings )
			{
				bindings << m_uiIP << m_uiPort << m_iAppID << m_kLocation;
			}

			FvString GetAsString() const;
		};

	private:
		DbEntryBuffer m_kEntryBuf;
		std::auto_ptr<MySqlStatement> m_spAddSTMT;

	public:

		static void CreateTable( MySql& connection );

		DbEntryBuffer &EntryBuf()	{ return m_kEntryBuf; }
		const DbEntryBuffer& EntryBuf()	const { return m_kEntryBuf; }
		MySqlStatement &AddStmt( MySql& connection );
	};
}

struct MySqlThreadData
{
	MySql m_kConnection;
	MySqlTypeMapping m_kTypeMapping;
	FvUInt64 m_uiStartTimestamp;

	FvEntityID m_iBoundID;
	int m_iBoundLimit;
	std::auto_ptr<MySqlStatement> m_spPutIDStatement;
	std::auto_ptr<MySqlUnPrep::Statement> m_spGetIDsStatement;
	std::auto_ptr<MySqlUnPrep::Statement> m_spDelIDsStatement;

	std::auto_ptr<MySqlStatement> m_spIncIDStatement;
	std::auto_ptr<MySqlStatement> m_spGetIDStatement;

	FvTimeStamp m_uiGameTime;
	std::auto_ptr<MySqlStatement> m_spSetGameTimeStatement;

	FvSpaceID m_iBoundSpaceID;
	SpaceDataBinding m_kBoundSpaceData;
	std::auto_ptr<MySqlStatement> m_spWriteSpaceStatement;
	std::auto_ptr<MySqlStatement> m_spWriteSpaceDataStatement;

	std::auto_ptr<MySqlStatement> m_spDelSpaceIDsStatement;
	std::auto_ptr<MySqlStatement> m_spDelSpaceDataStatement;

	MySQL::SecondaryDBOps m_kSecondaryDBOps;

	FvEntityDBKey m_kEntityDBKey;
	bool m_bIsOK;
	FvString m_kExceptionStr;

	MySqlThreadData( const FvDBConfigConnection& connInfo,
					  int maxSpaceDataSize,
					  const FvEntityDefs& entityDefs,
					  const char * tblNamePrefix = TABLE_NAME_PREFIX );
};

class MySqlThreadResPool : public FvNetTimerExpiryHandler
{
public:
	struct IThreadDataProcessor
	{
		virtual void Process( MySqlThreadData& threadData ) = 0;
		virtual void Done() = 0;
	};

private:
	typedef std::vector< MySqlThreadData* > ThreadDataVector;

	struct ThreadDataProcJob
	{
		IThreadDataProcessor &m_kProcessor;
		ThreadDataVector m_kUnprocessedItems;

		ThreadDataProcJob( IThreadDataProcessor& proc,
				const ThreadDataVector& items )
			: m_kProcessor( proc ), m_kUnprocessedItems( items ) {}
	};
	typedef std::list< ThreadDataProcJob > ThreadDataProcJobs;

	FvWorkerThreadPool m_kThreadPool;
	auto_container< ThreadDataVector > m_kThreadDataPool;
	ThreadDataVector m_kFreeThreadData;
	MySqlThreadData m_kMainThreadData;
	MySQL::NamedLock m_kDBLock;

	FvNetNub &m_kNub;
	FvNetTimerID m_kKeepAliveTimerID;

	ThreadDataProcJobs m_kThreadDataJobs;

	unsigned int m_uiOPCount;
	FvUInt64 m_uiOPDurationTotal;
	FvUInt64 m_uiResetTimeStamp;

public:
	MySqlThreadResPool( FvWorkerThreadManager& threadMgr,
						FvNetNub& nub,
		                int numConnections,
						int maxSpaceDataSize,
		                const FvDBConfigConnection& connInfo,
	                    const FvEntityDefs& entityDefs,
	                    bool shouldLockDB = true );
	virtual ~MySqlThreadResPool();

	int	GetNumConnections()	{	return int(m_kThreadDataPool.container.size()) + 1;	}
	MySqlThreadData &GetMainThreadData()	{ return m_kMainThreadData; }
	MySqlThreadData *AcquireThreadData( int timeoutMicroSeconds = -1 );
	void ReleaseThreadData( MySqlThreadData& threadData );
	MySqlThreadData &AcquireThreadDataAlways( int mainThreadThreshold = 1 );
	void ReleaseThreadDataAlways( MySqlThreadData& threadData );
	bool IsMainThreadData( MySqlThreadData& threadData ) const
	{	return &threadData == &m_kMainThreadData;	}

	void DoTask( FvWorkerThread::ITask& task, MySqlThreadData& threadData );

	void ApplyToAllThreadDatas( IThreadDataProcessor& processor );

	void StartThreadTaskTiming( MySqlThreadData& threadData );
	FvUInt64 StopThreadTaskTiming( MySqlThreadData& threadData );

	double GetBusyThreadsMaxElapsedSecs() const;
	double GetOpCountPerSec() const;
	double GetAvgOpDuration() const;

	FvWorkerThreadPool &ThreadPool()	{ return m_kThreadPool; }
	const FvWorkerThreadPool &GetThreadPool() const	{ return m_kThreadPool; }

	virtual int HandleTimeout( FvNetTimerID id, void * arg );

	bool LockDB()			{ return m_kDBLock.Lock(); }
	bool UnlockDB()			{ return m_kDBLock.Unlock(); }
	bool IsDBLocked() const	{ return m_kDBLock.IsLocked(); }

private:
	MySqlThreadData *AcquireThreadDataInternal();
	void ApplyThreadDataJob( MySqlThreadData& threadData,
			ThreadDataProcJob& job );
};

class MySqlThreadTask :	public FvWorkerThread::ITask
{
	MySqlDatabase &m_kOwner;
	MySqlThreadData &m_kThreadData;
	bool m_bIsTaskReady;

public:
	MySqlThreadTask( MySqlDatabase& owner, int acquireMainThreadThreshold = 3 )
		: m_kOwner(owner),
		m_kThreadData( m_kOwner.GetThreadResPool().AcquireThreadDataAlways(
						acquireMainThreadThreshold ) ),
		m_bIsTaskReady(true)
	{}

	virtual ~MySqlThreadTask()
	{
		if (m_kThreadData.m_kConnection.HasFatalError())
		{
			FV_ERROR_MSG( "MySqlDatabase: %s\n",
					m_kThreadData.m_kConnection.GetFatalErrorStr().c_str() );
			m_kOwner.OnConnectionFatalError();
		}

		m_kOwner.GetThreadResPool().ReleaseThreadDataAlways( m_kThreadData );
	}

	MySqlThreadData &GetThreadData()	{	return m_kThreadData;	}
	const MySqlThreadData &GetThreadData() const {	return m_kThreadData;	}
	MySqlDatabase &GetOwner()			{	return m_kOwner;	}
	void SetTaskReady( bool isReady )	{	m_bIsTaskReady = isReady;	}
	bool IsTaskReady() const			{	return m_bIsTaskReady;	}

	void StartThreadTaskTiming()
	{	m_kOwner.GetThreadResPool().StartThreadTaskTiming( this->GetThreadData() );	}
	FvUInt64 StopThreadTaskTiming()
	{	return m_kOwner.GetThreadResPool().StopThreadTaskTiming( this->GetThreadData() );	}

	void DoTask()
	{
		if (m_bIsTaskReady)
			m_kOwner.GetThreadResPool().DoTask( *this, m_kThreadData );
		else
			this->OnRunComplete();
	}

	void StandardInit()
	{
		this->StartThreadTaskTiming();
		this->GetThreadData().m_kExceptionStr.clear();
	}

	template < class ERROR_CONFIG >
	bool StandardOnRunComplete()
	{
		MySqlThreadData& threadData = this->GetThreadData();
		bool hasError = threadData.m_kExceptionStr.length();
		if (hasError)
		{
			FV_ERROR_MSG( "MySqlDatabase::%s( %s ): %s\n",
					ERROR_CONFIG::ErrorMethodName(),
					this->GetTaskInfo().c_str(),
					threadData.m_kExceptionStr.c_str() );
		}

		FvUInt64 duration = this->StopThreadTaskTiming();
		if (duration > ERROR_CONFIG::ThreadTaskWarningDuration())
		{
			FV_WARNING_MSG( "MySqlDatabase::%s( %s ): Task took %f seconds\n",
						ERROR_CONFIG::ErrorMethodName(),
						this->GetTaskInfo().c_str(),
						double(duration)/StampsPerSecondD() );
		}

		return hasError;
	}

	static FvUInt64 ThreadTaskWarningDuration()
	{
		return THREAD_TASK_WARNING_DURATION;
	}

	virtual FvString GetTaskInfo() const
	{
		return FvString();
	}
};

template <class QUERY>
class ThreadDataQuery
{
	MySqlThreadData &m_kThreadData;
	QUERY &m_kQuery;

public:
	ThreadDataQuery( MySqlThreadData& threadData, QUERY& query ) :
		m_kThreadData( threadData ), m_kQuery( query )
	{}

	void Execute( MySql& connection )
	{
		m_kQuery.Execute( connection, m_kThreadData );
	}

	void SetExceptionStr( const char * str )
	{
		m_kThreadData.m_kExceptionStr = str;
		m_kThreadData.m_bIsOK = false;
	}
};

template <class QUERY>
bool WrapInTransaction( MySql& connection, MySqlThreadData& threadData,
		QUERY& query )
{
	ThreadDataQuery< QUERY > tQuery( threadData, query );
	return WrapInTransaction( connection, tQuery );
}

class SingleStatementQuery
{
	MySqlStatement& stmt_;
public:
	SingleStatementQuery( MySqlStatement& stmt ) : stmt_( stmt ) {}
	void Execute( MySql& connection, MySqlThreadData& threadData )
	{
		connection.Execute( stmt_ );
	}
};

FV_INLINE bool WrapInTransaction( MySql& connection, MySqlThreadData& threadData,
		MySqlStatement& stmt )
{
	SingleStatementQuery query( stmt );
	return WrapInTransaction( connection, threadData, query );
}

#endif // __FvMySQLThread_H__
