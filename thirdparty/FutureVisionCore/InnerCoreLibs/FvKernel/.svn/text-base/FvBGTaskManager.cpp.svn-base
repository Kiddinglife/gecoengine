#include "FvBGTaskManager.h"

#include "FvWatcher.h"

FV_DECLARE_DEBUG_COMPONENT2( "CStdMF", 0 )

FvBGTaskManager* FvBGTaskManager::ms_pkInstance = NULL;

FvCStyleBackgroundTask::FvCStyleBackgroundTask( FUNC_PTR pfBGFunc, void *pkBGArg,
		FUNC_PTR pfFGFunc, void *pkFGArg ) :
	m_pfBGFunc( pfBGFunc ),
	m_pkBGArg( pkBGArg ),
	m_pfFGFunc( pfFGFunc ),
	m_pkFGArg( pkFGArg )
{
}

void FvCStyleBackgroundTask::DoBackgroundTask( FvBGTaskManager &kMgr )
{
	if (m_pfBGFunc)
	{
		(*m_pfBGFunc)( m_pkBGArg );
	}

	if (m_pfFGFunc)
	{
		kMgr.AddMainThreadTask( this );
	}
}

void FvCStyleBackgroundTask::DoMainThreadTask( FvBGTaskManager &kMgr )
{
	if (m_pfFGFunc)
	{
		(*m_pfFGFunc)( m_pkFGArg );
	}
}

#ifdef _WIN32
#if (ENABLE_DIARIES)
#define ENABLE_THREAD_TIMING_SYNCHRONISATION 0
#endif	//ENABLE_DIARIES
#endif	//_WIN32

#if ENABLE_THREAD_TIMING_SYNCHRONISATION
volatile int g_iThreadSync = 0;
#endif // ENABLE_THREAD_TIMING_SYNCHRONISATION

FvBackgroundTaskThread::FvBackgroundTaskThread( FvBGTaskManager &kMgr ) :
	FvSimpleThread(),
	m_kBGTaskManager( kMgr )
{
	this->FvSimpleThread::Init( FvBackgroundTaskThread::Start, this );

#if ENABLE_THREAD_TIMING_SYNCHRONISATION
	while ( !g_iThreadSync );
	DiaryEntryPtr de = Diary::instance().add( "Global thread start 1" );
	de->stop();
#endif // ENABLE_THREAD_TIMING_SYNCHRONISATION
}

void FvBackgroundTaskThread::Start( void *pkArg )
{
	FvBackgroundTaskThread *pTaskThread = (FvBackgroundTaskThread*)pkArg;

#if ENABLE_THREAD_TIMING_SYNCHRONISATION
	SetThreadAffinityMask(GetCurrentThread(), 2);
	g_iThreadSync = 1;
	DiaryEntryPtr de = Diary::instance().add( "Global thread start 2" );
	de->stop();
#endif // ENABLE_THREAD_TIMING_SYNCHRONISATION

	pTaskThread->Run();
}

void FvBackgroundTaskThread::Run()
{
	while (true)
	{
		FvBackgroundTaskPtr spTask = m_kBGTaskManager.PullBackgroundTask();

		if (spTask)
		{
			#ifdef FORCE_MAIN_THREAD
			FV_ASSERT( 0 );
			#endif // FORCE_MAIN_THREAD
			spTask->DoBackgroundTask( m_kBGTaskManager );
		}
		else
		{
			break;
		}
	}

	m_kBGTaskManager.AddMainThreadTask( new FvThreadFinisher( this ) );
}

void FvThreadFinisher::DoMainThreadTask( FvBGTaskManager &kMgr )
{
	kMgr.OnThreadFinished( m_pkThread );
}

void FvBGTaskManager::BackgroundTaskList::Push( FvBackgroundTaskPtr spTask,
												int iPriority )
{
	{
		FvSimpleMutexHolder kHolder( m_kMutex );

		List::reverse_iterator kIter = m_kList.rbegin();

		while ((kIter != m_kList.rend()) && (iPriority > kIter->first))
		{
			++kIter;
		}

		m_kList.insert( kIter.base(), std::make_pair( iPriority, spTask ) );
	}

	m_kSemaphore.Push();
}

FvBackgroundTaskPtr FvBGTaskManager::BackgroundTaskList::Pull()
{
	m_kSemaphore.Pull();
	m_kMutex.Grab();
	FvBackgroundTaskPtr spTask = m_kList.front().second;
	m_kList.pop_front();
	m_kMutex.Give();

	return spTask;
}

void FvBGTaskManager::BackgroundTaskList::Clear()
{
	m_kMutex.Grab();

	while (m_kSemaphore.PullTry())
	{
		m_kList.pop_front();
	}

	m_kMutex.Give();
}

FvBGTaskManager & FvBGTaskManager::Instance()
{
	if (ms_pkInstance == NULL)
	{
		ms_pkInstance = new FvBGTaskManager();
	}
	return *ms_pkInstance;

}

void FvBGTaskManager::Fini()
{
	if (ms_pkInstance)
	{
		delete ms_pkInstance;
		ms_pkInstance = NULL;
	}
}

FvBGTaskManager::FvBGTaskManager() :
	m_iNumRunningThreads( 0 ),
	m_iNumUnstoppedThreads( 0 )
{
}

FvBGTaskManager::~FvBGTaskManager()
{
	this->StopAll();
}

void FvBGTaskManager::StartThreads( int iNumThreads )
{
	if (m_iNumUnstoppedThreads != 0)
	{
		FV_WARNING_MSG( "FvBGTaskManager::init: "
				"Starting %d threads when %d still unstopped\n",
			iNumThreads, m_iNumUnstoppedThreads );
	}

	m_iNumUnstoppedThreads += iNumThreads;
	m_iNumRunningThreads += iNumThreads;

	for (int i = 0; i < iNumThreads; ++i)
	{
		new FvBackgroundTaskThread( *this );
	}
}

void FvBGTaskManager::StopAll( bool bDiscardPendingTasks, bool bWaitForThreads )
{
	if (bDiscardPendingTasks)
	{
		m_kBGTaskList.Clear();
	}

	for (int i = 0; i < m_iNumUnstoppedThreads; ++i)
	{
		m_kBGTaskList.Push( NULL );
	}

	m_iNumUnstoppedThreads = 0;

	if (bWaitForThreads && m_iNumRunningThreads > 0)
	{
		FV_TRACE_MSG( "FvBGTaskManager::StopAll: Waiting for threads: %d\n",
			m_iNumRunningThreads );

		while (true)
		{
			this->Tick();

			if (m_iNumRunningThreads)
			{
#ifdef _WIN32
				Sleep( 100 );
#else
				usleep( 100000 );
#endif
			}
			else
			{
				break;
			}
		}
	}
}

void FvBGTaskManager::AddBackgroundTask( FvBackgroundTaskPtr spTask, int iPriority )
{
#ifdef FORCE_MAIN_THREAD
	spTask->DoBackgroundTask( *this );
#else // FORCE_MAIN_THREAD
	m_kBGTaskList.Push( spTask, iPriority );
#endif // !FORCE_MAIN_THREAD
}

void FvBGTaskManager::AddMainThreadTask( FvBackgroundTaskPtr spTask )
{
	m_kFGTaskListMutex.Grab();
	m_kFGTaskList.push_back( spTask );
	m_kFGTaskListMutex.Give();
}

void FvBGTaskManager::Tick()
{
	ForegroundTaskList kNewTasks;

	m_kFGTaskListMutex.Grab();
	m_kFGTaskList.swap( kNewTasks );
	m_kFGTaskListMutex.Give();

	ForegroundTaskList::iterator kIter = kNewTasks.begin();

	while (kIter != kNewTasks.end())
	{
		(*kIter)->DoMainThreadTask( *this );

		++kIter;
	}
}

void FvBGTaskManager::OnThreadFinished( FvBackgroundTaskThread *pkThread )
{
	--m_iNumRunningThreads;
	delete pkThread;
	FV_TRACE_MSG( "FvBGTaskManager::OnThreadFinished: "
		"Thread finished. %d remaining\n", m_iNumRunningThreads );
}

FvBackgroundTaskPtr FvBGTaskManager::PullBackgroundTask()
{
	return m_kBGTaskList.Pull();
}
