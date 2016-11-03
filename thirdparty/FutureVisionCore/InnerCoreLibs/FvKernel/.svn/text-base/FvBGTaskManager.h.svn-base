//{future header message}
#ifndef __FvBGTaskManager_H__
#define __FvBGTaskManager_H__

#include <list>

#include "FvDebug.h"
#include "FvConcurrency.h"
#include "FvSmartPointer.h"

class FvBGTaskManager;

class FV_KERNEL_API FvBackgroundTask : public FvSafeReferenceCount
{
public:
	virtual void DoBackgroundTask( FvBGTaskManager &kMgr ) = 0;
	virtual void DoMainThreadTask( FvBGTaskManager &kMgr ) {};

protected:
	virtual ~FvBackgroundTask() {};
};

typedef FvSmartPointer< FvBackgroundTask > FvBackgroundTaskPtr;

class FV_KERNEL_API FvCStyleBackgroundTask : public FvBackgroundTask
{
public:
	typedef void (*FUNC_PTR)( void * );

	FvCStyleBackgroundTask( FUNC_PTR pfBGFunc, void *pkBGArg,
		FUNC_PTR pfFGFunc = NULL, void *pkFGArg = NULL );

	void DoBackgroundTask( FvBGTaskManager &kMgr );
	void DoMainThreadTask( FvBGTaskManager &kMgr );

private:
	FUNC_PTR m_pfBGFunc;
	void *m_pkBGArg;

	FUNC_PTR m_pfFGFunc;
	void * m_pkFGArg;
};

class FV_KERNEL_API FvBackgroundTaskThread : public FvSimpleThread
{
public:
	FvBackgroundTaskThread( FvBGTaskManager &kMgr );

private:
	static void Start( void *pkArg );
	void Run();

	FvBGTaskManager &m_kBGTaskManager;
};

class FV_KERNEL_API FvThreadFinisher : public FvBackgroundTask
{
public:
	FvThreadFinisher( FvBackgroundTaskThread *pkThread ) : m_pkThread( pkThread ) {}

	virtual void DoBackgroundTask( FvBGTaskManager &kMgr ) {}
	virtual void DoMainThreadTask( FvBGTaskManager &kMgr );

private:
	FvBackgroundTaskThread *m_pkThread;
};

class FV_KERNEL_API FvBGTaskManager
{
public:

	enum 
	{ 
		MIN = 0,
		LOW = 32,
		MEDIUM = 64,
		DEFAULT = MEDIUM,
		HIGH = 96,
		MAX = 128
	};

	FvBGTaskManager();
	~FvBGTaskManager();

	void Tick();

	void StartThreads( int iNumThreads );

	void StopAll( bool bDiscardPendingTasks = true, bool bWaitForThreads = true );

	void AddBackgroundTask( FvBackgroundTaskPtr spTask, int iPriority = DEFAULT );
	void AddMainThreadTask( FvBackgroundTaskPtr spTask );

	int NumRunningThreads() const { return m_iNumRunningThreads; }

	int NumUnstoppedThreads() const { return m_iNumUnstoppedThreads; }

	static FvBGTaskManager &Instance();
	static void Fini();

	void OnThreadFinished( FvBackgroundTaskThread *pkThread );
	FvBackgroundTaskPtr PullBackgroundTask();

private:
	static FvBGTaskManager *ms_pkInstance;

	class BackgroundTaskList
	{
	public:
		void Push( FvBackgroundTaskPtr spTask, int iPriority = DEFAULT );
		FvBackgroundTaskPtr Pull();
		void Clear();

	private:
		typedef std::list< std::pair< int, FvBackgroundTaskPtr > > List;
		List m_kList;
		FvSimpleMutex m_kMutex;
		FvSimpleSemaphore m_kSemaphore;
	};

	BackgroundTaskList m_kBGTaskList;

	typedef std::list< FvBackgroundTaskPtr > ForegroundTaskList;
	ForegroundTaskList m_kFGTaskList;
	FvSimpleMutex m_kFGTaskListMutex;

	int m_iNumRunningThreads;
	int m_iNumUnstoppedThreads;
};

#endif // __FvBGTaskManager_H__