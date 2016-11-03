#include "FvDebugFilter.h"

FvDebugFilter* FvDebugFilter::ms_pkInstance = NULL;

void FvDebugFilter::AddCriticalCallback( FvCriticalMessageCallback *pkCallback )
{
	m_kCriticalCallbacks.push_back(pkCallback);
}

void FvDebugFilter::DeleteCriticalCallback( FvCriticalMessageCallback *pkCallback )
{
	CriticalCallbacks::iterator it = m_kCriticalCallbacks.begin();
	for ( ; it != m_kCriticalCallbacks.end(); ++it )
	{
		if ( (*it) == pkCallback )
		{
			m_kCriticalCallbacks.erase( it );
			return;
		}
	}
}

void FvDebugFilter::AddMessageCallback( FvDebugMessageCallback *pkCallback )
{
	m_kMessageCallbacks.push_back(pkCallback);
}

void FvDebugFilter::DeleteMessageCallback( FvDebugMessageCallback *pkCallback )
{
	DebugCallbacks::iterator it = m_kMessageCallbacks.begin();
	for ( ; it != m_kMessageCallbacks.end(); ++it )
	{
		if ( (*it) == pkCallback )
		{
			m_kMessageCallbacks.erase( it );
			if (m_kMessageCallbacks.size() == 0) { Fini(); }
			return;
		}
	}
}
