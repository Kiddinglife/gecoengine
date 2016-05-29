#include "JackieSimpleMutex.h"

JackieSimpleMutex::JackieSimpleMutex() //: isInitialized(false)
{
	// Prior implementation of Initializing in Lock() was not threadsafe
#if defined(WINDOWS_PHONE_8) || defined(WINDOWS_STORE_RT)
	InitializeCriticalSectionEx(&criticalSection, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
#elif defined(_WIN32)
	InitializeCriticalSection(&criticalSection);
#else
	m_count = 0;
	m_threadID = 0;
	pthread_mutex_init(&m_mutex, 0);
#endif
}

JackieSimpleMutex::~JackieSimpleMutex()
{
#ifdef _WIN32
	DeleteCriticalSection(&criticalSection);
#else
	pthread_mutex_destroy(&m_mutex );
#endif
}


void JackieSimpleMutex::Lock(void)
{
#ifdef _WIN32
	EnterCriticalSection(&criticalSection);
#else
	/// avoid deadlock
	if ( m_count && m_threadID == pthread_self() ) { ++m_count; return ; }
	pthread_mutex_lock( &m_mutex );
	++m_count;
	m_threadID = pthread_self();
#endif
}


void JackieSimpleMutex::Unlock(void)
{
#ifdef _WIN32
	LeaveCriticalSection(&criticalSection);
#else
	if ( m_count > 1 ) { m_count--; return ; }
	--m_count;
	m_threadID = 0;
	pthread_mutex_unlock(&m_mutex);
#endif
}

