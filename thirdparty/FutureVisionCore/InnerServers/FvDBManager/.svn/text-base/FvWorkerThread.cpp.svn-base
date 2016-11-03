#include "FvWorkerThread.h"
#include <FvTimeStamp.h>

FvWorkerThread::FvWorkerThread( FvWorkerThreadManager& mgr )
	: m_kThreadData(mgr), m_kThread( &FvWorkerThread::ThreadMainLoop, &m_kThreadData )
{}

FvWorkerThread::~FvWorkerThread()
{
	m_kThreadData.m_kReadySema.Pull();
	m_kThreadData.m_pkTask = 0;
	m_kThreadData.m_kWorkSema.Push();
}

bool FvWorkerThread::DoTaskImpl( ITask* pTask )
{
	bool isOK = m_kThreadData.m_kReadySema.PullTry();
	if (isOK)
	{
		m_kThreadData.m_pkTask = pTask;
		m_kThreadData.m_kWorkSema.Push();
	}

	return isOK;
}

void FvWorkerThread::ThreadMainLoop( void* arg )
{
	ThreadData*	pData = reinterpret_cast<ThreadData*>(arg);
	while (true)
	{
		pData->m_kWorkSema.Pull();
		if (pData->m_pkTask)
		{
			pData->m_pkTask->Run();
			ITask*	pCompletedTask = pData->m_pkTask;
			pData->m_kReadySema.Push();

			pData->m_kWorkerThreadManager.OnTaskComplete( *pCompletedTask );
		}
		else
			break;
	}
}



FvWorkerThreadManager::FvWorkerThreadManager( FvNetNub& nub )
	: m_kNub(nub), m_kCompletedTasks(), m_kCompletedTasksLock()
{
	FV_ASSERT(!m_kNub.GetOpportunisticPoller());
	m_kNub.SetOpportunisticPoller(this);

	m_kTimerID = m_kNub.RegisterTimer( 1000, this );

	#ifdef FV_WORKERTHREAD_SELFTEST
		this->SelfTest();
	#endif
}

FvWorkerThreadManager::~FvWorkerThreadManager()
{
	FV_ASSERT(m_kNub.GetOpportunisticPoller() == this);
	m_kNub.CancelTimer( m_kTimerID );

	m_kNub.SetOpportunisticPoller( 0 );
}

int FvWorkerThreadManager::ProcessCompletedTasks()
{
	CompletedTasks	completedTasks;

	m_kCompletedTasksLock.Grab();
	completedTasks.swap( m_kCompletedTasks );
	m_kCompletedTasksLock.Give();

	for ( CompletedTasks::const_iterator i = completedTasks.begin();
		i < completedTasks.end(); ++i )
	{
		(*i)->OnRunComplete();
	}

	return int(completedTasks.size());
}

bool FvWorkerThreadManager::WaitForTaskCompletion( int numTasks,
                                             int timeoutMicroSecs )
{
	int numCompleted = this->ProcessCompletedTasks();
	if (numCompleted < numTasks)
	{
		FvUInt64	endTimeStamp = Timestamp() +
					( FvUInt64(timeoutMicroSecs) * StampsPerSecond() ) / 1000000;
		do
		{
			ThreadSleep( 100 );
			numCompleted += this->ProcessCompletedTasks();
		} while ( (numCompleted < numTasks) &&
			      ( (timeoutMicroSecs < 0) || (Timestamp() < endTimeStamp) ) );
	}

	return (numCompleted >= numTasks);
}

void FvWorkerThreadManager::OnTaskComplete( FvWorkerThread::ITask& task )
{
	FvSimpleMutexHolder mutexHolder( m_kCompletedTasksLock );
	m_kCompletedTasks.push_back( &task );
}

void FvWorkerThreadManager::Poll()
{
	this->ProcessCompletedTasks();
}

int FvWorkerThreadManager::HandleTimeout( FvNetTimerID /*id*/, void * /*arg*/ )
{
	return 0;
}


FvWorkerThreadPool::FvWorkerThreadPool( FvWorkerThreadManager& mgr, int numThreads )
	: m_kWorkerThreadManager(mgr), m_kThreads(), m_kFreeThreads()
{
	m_kThreads.container.reserve( numThreads );
	for ( int i = 0; i < numThreads; ++i )
	{
		PoolItem* pItem = new PoolItem( mgr, *this );
		m_kThreads.container.push_back( pItem );
	}

	m_kFreeThreads = m_kThreads.container;
}

bool FvWorkerThreadPool::DoTask( FvWorkerThread::ITask& task )
{
	bool isOK = (m_kFreeThreads.size() > 0);
	if (isOK)
	{
		PoolItem* pItem = m_kFreeThreads.back();
		m_kFreeThreads.pop_back();
		isOK = pItem->DoTask(task);
		FV_ASSERT(isOK);
	}
	return isOK;
}

void FvWorkerThreadPool::DoTaskInCurrentThread( FvWorkerThread::ITask& task )
{
	task.Run();
	task.OnRunComplete();
}

bool FvWorkerThreadPool::WaitForOneTask( int timeoutMicroSecs )
{
	int		numBusyTasksStart = this->GetNumBusyThreads();
	FvUInt64	endTimeStamp = Timestamp() +
				( FvUInt64(timeoutMicroSecs) * StampsPerSecond() ) / 1000000;
	bool isOK;
	do
	{
		int timeout = (timeoutMicroSecs >= 0) ?
						int(((endTimeStamp - Timestamp())*1000000)/StampsPerSecond()) :
						-1;
		isOK = m_kWorkerThreadManager.WaitForTaskCompletion( 1, timeout );
	} while (isOK && (numBusyTasksStart == this->GetNumBusyThreads()));

	return isOK;
}

bool FvWorkerThreadPool::WaitForAllTasks( int timeoutMicroSecs )
{
	FvUInt64	endTimeStamp = Timestamp() +
				( FvUInt64(timeoutMicroSecs) * StampsPerSecond() ) / 1000000;
	bool isOK = true;
	while (isOK && (this->GetNumBusyThreads() > 0))
	{
		int timeout = (timeoutMicroSecs >= 0) ?
						int(((endTimeStamp - Timestamp())*1000000)/StampsPerSecond()) :
						-1;
		isOK = m_kWorkerThreadManager.WaitForTaskCompletion( this->GetNumBusyThreads(), timeout );
	}

	return isOK;
}

void FvWorkerThreadPool::OnTaskComplete( PoolItem& poolItem )
{
	m_kFreeThreads.push_back( &poolItem );
}

void FvWorkerThreadPool::PoolItem::Run()
{
	m_pkOrigTask->Run();
}

void FvWorkerThreadPool::PoolItem::OnRunComplete()
{
	m_kPool.OnTaskComplete( *this );
	m_pkOrigTask->OnRunComplete();
}

#ifdef FV_WORKERTHREAD_SELFTEST
// ----------------------------------------------------------------------------
// Test
// ----------------------------------------------------------------------------
class CountSheep : public FvWorkerThread::ITask
{
	int		id_;
	int		numSheep_;
	bool	isAsleep_;

	static int globalId_;

public:
	CountSheep( int numSheep ) :
	  id_(globalId_++), numSheep_(numSheep), isAsleep_(false)
	{
		printf( "CountSheep%d prepared\n", id_ );
	}

	bool isAsleep()	{	return isAsleep_;	}

	// FvWorkerThread::ITask overrides
	virtual void Run();
	virtual void OnRunComplete();
};

int CountSheep::globalId_ = 1;

void CountSheep::Run()
{
	printf( "CountSheep%d start\n", id_ );
	int	count = 1;
	while (count <= numSheep_)
	{
		ThreadSleep(1000000);
		printf( "CountSheep%d sheep %d\n", id_, count );
		++count;
	}
}

void CountSheep::OnRunComplete()
{
	printf( "CountSheep%d complete\n", id_ );
	isAsleep_ = true;
}

void FvWorkerThreadManager::SelfTest()
{
	FvWorkerThreadPool	pool( *this, 3 );
	CountSheep			insomnia1( 10 );
	CountSheep			insomnia2( 5 );
	CountSheep			insomnia3( 20 );

	// Use up all threads.
	bool isOK = pool.DoTask( insomnia1 );
	FV_ASSERT(isOK);
	isOK = pool.DoTask( insomnia2 );
	FV_ASSERT(isOK);
	isOK = pool.DoTask( insomnia3 );
	FV_ASSERT(isOK);
	FV_ASSERT(pool.GetNumFreeThreads() == 0);

	// All threads should be busy now.
	CountSheep			insomnia4( 2 );
	isOK = pool.DoTask( insomnia4 );
	FV_ASSERT(!isOK && !insomnia4.isAsleep());

	// Do task in main thread.
	pool.DoTaskInCurrentThread( insomnia4 );
	FV_ASSERT(insomnia4.isAsleep());

	// Wait for 1 task.
	isOK = pool.WaitForOneTask();
	FV_ASSERT( isOK && (pool.GetNumFreeThreads() == 1) && insomnia2.isAsleep() );

	// Wait for all tasks.
	CountSheep	insomnia5( 5 );
	isOK = pool.DoTask( insomnia5 );
	FV_ASSERT(isOK && (pool.GetNumFreeThreads() == 0) );
	isOK = pool.WaitForAllTasks();
	FV_ASSERT(isOK && (pool.GetNumFreeThreads() == 3) &&
		insomnia1.isAsleep() && insomnia3.isAsleep() && insomnia5.isAsleep() );

	// Wait for 1 task with timeout
	CountSheep	insomnia6( 7 );
	isOK = pool.DoTask( insomnia6 );
	FV_ASSERT(isOK);
	isOK = pool.WaitForOneTask( 4000000 );
	FV_ASSERT( !isOK && (pool.GetNumBusyThreads() == 1) );
	isOK = pool.WaitForOneTask( 4000000 );
	FV_ASSERT( isOK && insomnia6.isAsleep() );

	// Wait for all tasks with timeout
	CountSheep	insomnia7( 5 );
	CountSheep	insomnia8( 10 );
	isOK = pool.DoTask( insomnia7 );
	FV_ASSERT(isOK);
	isOK = pool.DoTask( insomnia8 );
	FV_ASSERT(isOK);
	isOK = pool.WaitForAllTasks( 1000000 );
	FV_ASSERT( !isOK && (pool.GetNumBusyThreads() == 2) );
	isOK = pool.WaitForAllTasks( 6000000 );
	FV_ASSERT( !isOK && (pool.GetNumBusyThreads() == 1) );
	isOK = pool.WaitForAllTasks( 5000000 );
	FV_ASSERT( isOK && (pool.GetNumBusyThreads() == 0) );
}
#endif	// FV_WORKERTHREAD_SELFTEST
