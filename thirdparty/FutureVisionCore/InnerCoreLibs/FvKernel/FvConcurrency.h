//{future header message}
#ifndef __FvConcurrency_H__
#define __FvConcurrency_H__

#include "FvDebug.h"
#include "FvKernel.h"

#if defined( PLAYSTATION3 )
#include <cell/atomic.h>
#endif

typedef void (* SimpleThreadFunc)(void*);

struct FV_KERNEL_API FvThreadTrampolineInfo
{
	FvThreadTrampolineInfo( SimpleThreadFunc func, void * arg )
	{
		this->m_pfFunc = func;
		this->m_pkArg = arg;
	}
	SimpleThreadFunc m_pfFunc;
	void * m_pkArg;
};

class FV_KERNEL_API FvDummyMutex
{
public:
	void Grab() {}
	bool GrabTry() { return true; }
	void Give() {}
};

#ifdef FV_WIN32

#ifndef _XBOX360
#include "windows.h"
#else
#include <xtl.h>
#endif

#include <process.h>

FV_INLINE
unsigned long OurThreadID()
{
	return (unsigned long)(GetCurrentThreadId());
}

class FV_KERNEL_API FvSimpleMutex
{
public:
	FvSimpleMutex() : m_bGone( false ) { InitializeCriticalSection( &m_kMutex ); }
	~FvSimpleMutex()	{ DeleteCriticalSection( &m_kMutex ); }

	void Grab()		{ EnterCriticalSection( &m_kMutex ); FV_ASSERT( !m_bGone ); m_bGone = true; }
	void Give()		{ m_bGone = false; LeaveCriticalSection( &m_kMutex ); }
	bool GrabTry()
	{
		if (TryEnterCriticalSection( &m_kMutex ))
		{
			FV_ASSERT( !m_bGone );
			m_bGone = true;
			return true;
		}
		return false;
	};

private:
	CRITICAL_SECTION m_kMutex;
	bool m_bGone;
};

class FV_KERNEL_API FvRecursionCheckingMutex
{
public:
	FvRecursionCheckingMutex() : m_bGone( false ) { InitializeCriticalSection( &m_kMutex ); }
	~FvRecursionCheckingMutex()	{ DeleteCriticalSection( &m_kMutex ); }

	bool Grab()
	{
		EnterCriticalSection( &m_kMutex );
		if( m_bGone )
		{
			LeaveCriticalSection( &m_kMutex );
			return false;
		} else {
			m_bGone = true;
			return true;
		}
	}
	void Give()		{ m_bGone = false; LeaveCriticalSection( &m_kMutex ); }

private:
	CRITICAL_SECTION m_kMutex;
	bool m_bGone;
};

class FV_KERNEL_API FvSimpleSemaphore
{
public:
	FvSimpleSemaphore() : m_hSema( CreateSemaphore( NULL, 0, 32767, NULL ) ) { }
	~FvSimpleSemaphore()	{ CloseHandle( m_hSema ); }

	void Push()			{ ReleaseSemaphore( m_hSema, 1, NULL ); }
	void Pull()			{ WaitForSingleObject( m_hSema, INFINITE ); }
	bool PullTry()		{ return WaitForSingleObject( m_hSema, 0 ) == WAIT_OBJECT_0; }

private:
	HANDLE m_hSema;
};

class FV_KERNEL_API FvSimpleThread
{
public:
	FvSimpleThread()
		: m_hThread( NULL )
	{}

	FvSimpleThread( SimpleThreadFunc threadfunc, void * arg )
	{
		Init( threadfunc, arg );
	}

	~FvSimpleThread()
	{
		WaitForSingleObject( m_hThread, INFINITE );
		CloseHandle( m_hThread );
	}

	void Init( SimpleThreadFunc threadfunc, void * arg )
	{
		FvThreadTrampolineInfo * info = new FvThreadTrampolineInfo( threadfunc, arg );
		unsigned threadId;
		m_hThread = HANDLE( _beginthreadex( 0, 0, Trampoline, info, 0, &threadId ) );
	}

	struct AutoHandle	// Ugly!!
	{
		AutoHandle( HANDLE h ) { HANDLE p = GetCurrentProcess();
			DuplicateHandle( p, h, p, &real_, 0, TRUE, 0 ); }
		~AutoHandle() { CloseHandle( real_ ); }
		operator HANDLE() const { return real_; }
		HANDLE real_;
	};

	HANDLE Handle() const	{ return m_hThread; }	// exposed for more complex ops
	static AutoHandle Current()	{ return AutoHandle( GetCurrentThread() ); }

private:
	HANDLE m_hThread;

	static unsigned __stdcall Trampoline( void * arg )
	{
		FvThreadTrampolineInfo * info = static_cast<FvThreadTrampolineInfo*>(arg);

#if FV_ENABLE_STACK_TRACKER
		__try
#endif // FV_ENABLE_STACK_TRACKER
		{
			info->m_pfFunc( info->m_pkArg );
		}

#if FV_ENABLE_STACK_TRACKER 
		__except( ExceptionFilter(GetExceptionCode()) )
		{
		}
#endif // FV_ENABLE_STACK_TRACKER

		delete info;
		return 0;
	}
};

#ifndef _XBOX360
FV_INLINE char AtomicSwap( void *& dst, void * curVal, void * newVal )
{
	__asm mov eax, curVal
	__asm mov edx, newVal
	__asm mov ecx, dst
	__asm lock cmpxchg [ecx], edx
	__asm setz al
}
#else
FV_INLINE char AtomicSwap( void *& dst, void * curVal, void * newVal )
{
	InterlockedExchange( (volatile long *)&dst, (long)newVal );
	return 1;
}
#endif

#if !defined(FV_SINGLE_THREADED)
	#define FV_THREADLOCAL(type) __declspec( thread ) type
#else
	#define FV_THREADLOCAL(type) type
#endif

#else // FV_WIN32

#ifndef FV_SINGLE_THREADED
#include <pthread.h>
#include <errno.h>

class FV_KERNEL_API FvSimpleMutex
{
public:
	FvSimpleMutex()	{ pthread_mutex_init( &m_kMutex, NULL); }
	~FvSimpleMutex()	{ pthread_mutex_destroy( &m_kMutex ); }

	void Grab()		{ pthread_mutex_lock( &m_kMutex ); }
	bool GrabTry()	{ return (pthread_mutex_trylock( &m_kMutex ) == 0); }
	void Give()		{ pthread_mutex_unlock( &m_kMutex ); }

private:
	pthread_mutex_t m_kMutex;
};

class FV_KERNEL_API FvRecursionCheckingMutex
{
public:
	FvRecursionCheckingMutex()
	{
		pthread_mutexattr_t errorchecking;
		pthread_mutexattr_init( &errorchecking );
		pthread_mutexattr_settype( &errorchecking, PTHREAD_MUTEX_ERRORCHECK_NP );
		pthread_mutex_init( &m_kMutex, &errorchecking );
		pthread_mutexattr_destroy( &errorchecking );
	}
	~FvRecursionCheckingMutex()
		{ pthread_mutex_destroy( &m_kMutex ); }

	bool Grab()
	{
		int result = pthread_mutex_lock( &m_kMutex );
		if ( result == EDEADLK )
		{
			return false;
		} else {
			return true;
		}
	}
	void Give()		{ pthread_mutex_unlock( &m_kMutex ); }

private:
	pthread_mutex_t m_kMutex;
};

class FV_KERNEL_API FvSimpleSemaphore
{
public:
	FvSimpleSemaphore() :
		value_( 0 )
	{
		pthread_mutex_init( &lock_, NULL );
		pthread_cond_init( &wait_, NULL );
	}
	~FvSimpleSemaphore()
	{
		pthread_cond_destroy( &wait_ );
		pthread_mutex_destroy( &lock_ );
	}

	void pull()
	{
		pthread_mutex_lock( &lock_ );
		value_--;
		if (value_ < 0)
			pthread_cond_wait( &wait_, &lock_ );
		pthread_mutex_unlock( &lock_ );
	}
	bool pullTry()
	{
		bool gotit = true;
		pthread_mutex_lock( &lock_ );
		if (value_ <= 0)
			gotit = false;
		else
			value_--;
		pthread_mutex_unlock( &lock_ );
		return gotit;
	}
	void push()
	{
		pthread_mutex_lock( &lock_ );
		value_++;
		if (value_ <= 0)
			pthread_cond_signal( &wait_ );
		pthread_mutex_unlock( &lock_ );
	}

private:
	pthread_mutex_t	lock_;
	pthread_cond_t	wait_;
	int			value_;
};

FV_INLINE unsigned long OurThreadID()
{
	return (unsigned long)(pthread_self());
}

class FV_KERNEL_API FvSimpleThread
{
public:
	FvSimpleThread() : m_hThread( 0 )
	{
	}

	FvSimpleThread( SimpleThreadFunc threadfunc, void * arg )
	{
		this->init( threadfunc, arg );
	}

	void init( SimpleThreadFunc threadfunc, void * arg )
	{
		FvThreadTrampolineInfo * info = new FvThreadTrampolineInfo( threadfunc, arg );
		pthread_create( &m_hThread, NULL, Trampoline, info );
	}
	~FvSimpleThread()
	{
		pthread_join( m_hThread, NULL );
	}

	pthread_t handle() const	{ return m_hThread; }
	static pthread_t current()	{ return pthread_self(); }

private:
	pthread_t m_hThread;

	static void * Trampoline( void * arg )
	{
		FvThreadTrampolineInfo * info = static_cast<FvThreadTrampolineInfo*>(arg);
		info->m_pfFunc( info->m_pkArg );
		delete info;
		return NULL;
	}
};
#else // !FV_SINGLE_THREADED

class FV_KERNEL_API FvSimpleMutex
{
public:
	FvSimpleMutex() {}
	~FvSimpleMutex() {}

	void Grab() {}
	void Give() {}
	bool GrabTry() { return true; }
};

class FV_KERNEL_API FvRecursionCheckingMutex
{
public:
	FvRecursionCheckingMutex(): locked_( false ) {};

	bool Grab()
	{
		if( locked_ )
		{
			return false;
		} else {
			locked_ = true;
			return true;
		}
	}
	void Give()		{ locked_ = false; }

private:
	bool locked_;
};

class FV_KERNEL_API FvSimpleSemaphore
{
public:
	FvSimpleSemaphore() {}
	~FvSimpleSemaphore() {}

	void push() {}
	void pull() {}
	bool pullTry() { return true; }
};

FV_INLINE unsigned long OurThreadID()
{
	return 0;
}

class FV_KERNEL_API FvSimpleThread
{
public:
	FvSimpleThread() {}
	FvSimpleThread( SimpleThreadFunc threadfunc, void * arg )
	{
		this->Init( threadfunc, arg );
	}

	void Init( SimpleThreadFunc threadfunc, void * arg )
	{
		FV_CRITICAL_MSG( "SimpleThread being initialised when "
			"FV_SINGLE_THREADED!" );
	}


	~FvSimpleThread(){}

	int Handle() const { return 0; }
	static int Current() { return 0; }

};
#endif // FV_SINGLE_THREADED

#ifndef FV_SINGLE_THREADED
# define FV_THREADLOCAL(type) __thread type
#else
# define FV_THREADLOCAL(type) type
#endif

#if defined( PLAYSTATION3 )

FV_INLINE bool AtomicSwap( void *& dst, void * curVal, void * newVal )
{
	FvUInt32 ret = cellAtomicCompareAndSwap32(
		(uint32_t *)&dst, (uint32_t)curVal, (uint32_t)newVal );
	return ( ret == (uint32_t)curVal );
}

#else // PLAYSTATION3

FV_INLINE bool AtomicSwap( void *& dst, void * curVal, void * newVal )
{
	char ret;

	__asm__ volatile (
			"lock cmpxchg %2, %1\n\t"	// (atomically) Compare and Exchange
			"setz %0\n"
		:	"=r"	(ret)		// %0 is ret on output
		:	"m"		(dst),		// %1 is dst on input
			"r"		(newVal),	// %2 is newVal on input
			"a"		(curVal)	// eax is curVal on input
		: "memory" );			// memory is modified
	return ret;
}

#endif // !PLAYSTATION3

#endif // !FV_WIN32

class FV_KERNEL_API FvSimpleMutexHolder
{
public:
	FvSimpleMutexHolder( FvSimpleMutex & sm ) : m_kMutex( sm )	{ m_kMutex.Grab(); }
	~FvSimpleMutexHolder()								{ m_kMutex.Give(); }
private:
	FvSimpleMutex & m_kMutex;
};


#ifdef FV_WIN32
 #define ARGTYPE  FvUInt64
#else // FV_WIN32
 #define ARGTYPE  intptr_t
#endif // !FV_WIN32

class FV_KERNEL_API FvMatrixMutexHolder
{
	static const int MUTEX_MATRIX_SIZE = 271;
	static FvSimpleMutex m_akMutex[MUTEX_MATRIX_SIZE];
	static const unsigned long INVALID_THREAD_ID = 0;
	static volatile unsigned long m_uiThreadID[MUTEX_MATRIX_SIZE];
	void Grab( ARGTYPE uiHashInput )
	{
#ifdef FV_WIN32
		FvUInt64 ui = uiHashInput;
#else // FV_WIN32
		FvUInt64 ui = (int)uiHashInput;
#endif // !FV_WIN32
		m_uiIndex = ((ui * ui) >> 3) % MUTEX_MATRIX_SIZE;
		m_bGrab = m_uiThreadID[ m_uiIndex ] != OurThreadID();
		if( m_bGrab )
		{
			m_akMutex[ m_uiIndex ].Grab();
			m_uiThreadID[ m_uiIndex ] = OurThreadID();
		}
	}
	ARGTYPE m_uiIndex;
	bool m_bGrab;
public:
	FvMatrixMutexHolder( const void* p )
	{
		Grab( (ARGTYPE)p );
	}
	FvMatrixMutexHolder( unsigned int i )
	{
		Grab( i );
	}
	~FvMatrixMutexHolder()
	{
		if( m_bGrab )
		{
			m_uiThreadID[ m_uiIndex ] = INVALID_THREAD_ID;
			m_akMutex[ m_uiIndex ].Give();
		}
	}
};

#undef ARGTYPE

extern void FV_KERNEL_API (*BeginThreadBlockingOperation)();
extern void FV_KERNEL_API (*CeaseThreadBlockingOperation)();

#endif // __FvConcurrency_H__