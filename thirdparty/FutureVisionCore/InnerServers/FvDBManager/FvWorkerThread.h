//{future header message}
#ifndef __FvWorkerThread_H__
#define __FvWorkerThread_H__

#include <vector>

#include <FvConcurrency.h>
#include <FvNetNub.h>

class FvWorkerThreadManager;


FV_INLINE void ThreadSleep( int usecs )
{
#ifdef _WIN32
	DWORD msecs = usecs/1000;
	if (msecs == 0)
		msecs = 1;
	Sleep(msecs);
#else
	timeval	timeout;
	timeout.tv_sec = usecs/1000000;
	timeout.tv_usec = usecs%1000000;
	select( 0, NULL, NULL, NULL, &timeout );
#endif
}


template <class CONTAINER>
struct auto_container
{
	CONTAINER	container;

	auto_container()	{}
	template <class ARGTYPE>
	explicit auto_container( ARGTYPE a ) : container( a ) {}
	template <class ARG1TYPE, class ARG2TYPE>
	auto_container( ARG1TYPE a, ARG2TYPE b ) : container( a, b ) {}
	template <class ARG1TYPE, class ARG2TYPE, class ARG3TYPE>
	auto_container( ARG1TYPE a, ARG2TYPE b, ARG3TYPE c ) :
		container( a, b, c ) {}
	~auto_container()	{	deleteContents( container );	}

	static void deleteContents( CONTAINER& c )
	{
		std::for_each( c.begin(), c.end(), Delete() );
	}
	static void nullifyContents( CONTAINER& c )
	{
		std::for_each( c.begin(), c.end(), DeleteAndNullify() );
	}

private:
	auto_container( const auto_container& );
	auto_container& operator=( auto_container& );

	struct Delete
	{ void operator()( typename CONTAINER::value_type& v ) { delete v; } };
	struct DeleteAndNullify
	{ void operator()( typename CONTAINER::value_type& v ) { delete v; v = NULL; } };

};

class FvWorkerThread
{
public:
	struct ITask
	{
		virtual void Run() = 0;
		virtual void OnRunComplete() = 0;
	};

private:
	struct ThreadData
	{
		FvSimpleSemaphore m_kWorkSema;	
									
		FvSimpleSemaphore m_kReadySema;	
									
		FvWorkerThreadManager &m_kWorkerThreadManager;	
									
		ITask *m_pkTask;		
								

		ThreadData(FvWorkerThreadManager& threadMgr)
			: m_kWorkSema(), m_kReadySema(), m_kWorkerThreadManager(threadMgr), m_pkTask(NULL)
		{
			m_kReadySema.Push();
		}
	};
	ThreadData m_kThreadData;
	FvSimpleThread m_kThread;

public:
	FvWorkerThread(FvWorkerThreadManager& mgr);
	~FvWorkerThread();

	bool DoTask( ITask& task )	{	return DoTaskImpl(&task);	}

private:
	FvWorkerThread(const FvWorkerThread&);
	FvWorkerThread& operator=(const FvWorkerThread&);

	bool DoTaskImpl( ITask* task );
	static void ThreadMainLoop( void* arg );
};

class FvWorkerThreadManager : public FvNetNub::IOpportunisticPoller,
                        public FvNetTimerExpiryHandler
{
	typedef std::vector< FvWorkerThread::ITask* > CompletedTasks;

	FvNetNub &m_kNub;
	FvNetTimerID m_kTimerID;
	CompletedTasks m_kCompletedTasks;
	FvSimpleMutex m_kCompletedTasksLock;

public:
	FvWorkerThreadManager( FvNetNub& nub );
	virtual ~FvWorkerThreadManager();

	int ProcessCompletedTasks();

	bool WaitForTaskCompletion( int numTasks, int timeoutMicroSecs = -1 );

	void OnTaskComplete( FvWorkerThread::ITask& task );

	virtual void Poll();

	virtual int HandleTimeout( FvNetTimerID id, void * arg );

private:
	FvWorkerThreadManager( const FvWorkerThreadManager& );
	FvWorkerThreadManager& operator=( const FvWorkerThreadManager& );

	#ifdef FV_WORKERTHREAD_SELFTEST
		void SelfTest();
	#endif
};

class FvWorkerThreadPool
{
	class PoolItem : public FvWorkerThread::ITask
	{
		FvWorkerThreadPool &m_kPool;
		FvWorkerThread m_kThread;
		FvWorkerThread::ITask *m_pkOrigTask;

	public:
		PoolItem( FvWorkerThreadManager& mgr, FvWorkerThreadPool& pool )
			: m_kPool(pool), m_kThread(mgr)
		{}
		virtual ~PoolItem() {}

		bool DoTask( FvWorkerThread::ITask& task )
		{
			m_pkOrigTask = &task;
			return m_kThread.DoTask(*this);
		}

		virtual void Run();
		virtual void OnRunComplete();
	};

	typedef std::vector< PoolItem* > PoolItems;

	FvWorkerThreadManager &m_kWorkerThreadManager;
	auto_container< PoolItems >	m_kThreads;
	PoolItems m_kFreeThreads;

public:
	FvWorkerThreadPool( FvWorkerThreadManager& mgr, int numThreads );

	bool DoTask( FvWorkerThread::ITask& task );
	static void DoTaskInCurrentThread( FvWorkerThread::ITask& task );

	int GetNumFreeThreads() const	{	return int(m_kFreeThreads.size());	}
	int GetNumBusyThreads() const	{	return int(m_kThreads.container.size()) - GetNumFreeThreads();	}

	bool WaitForOneTask( int timeoutMicroSecs = -1 );
	bool WaitForAllTasks( int timeoutMicroSecs = -1 );

	void OnTaskComplete( PoolItem& poolItem );
};

#endif // __FvWorkerThread_H__
