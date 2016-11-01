//{future header message}
#ifndef __FvDebugFilter_H__
#define __FvDebugFilter_H__

#include "FvKernel.h"
#include <vector>

struct FV_KERNEL_API FvCriticalMessageCallback
{
	virtual void HandleCritical( const char *pcMsg ) = 0;
	virtual ~FvCriticalMessageCallback() {};
};

struct FV_KERNEL_API FvDebugMessageCallback
{
	virtual bool HandleMessage( int iComponentPriority,
		int iMessagePriority, const char *pcFormat, va_list kArgPtr ) = 0;
	virtual ~FvDebugMessageCallback() {};
};

class FV_KERNEL_API FvDebugFilter
{
public:
	FvDebugFilter() :
		m_iFilterThreshold( 0 ),
		m_bHasDevelopmentAssertions( true )
	{
	}

	static FvDebugFilter & Instance()
	{
		if (ms_pkInstance == NULL)
		{
			ms_pkInstance = new FvDebugFilter();
		}
		return *ms_pkInstance;
	}

	static void Fini()
	{
		if (ms_pkInstance)
		{
			delete ms_pkInstance;
			ms_pkInstance = NULL;
		}
	}

	static bool ShouldAccept( int iComponentPriority, int iMessagePriority )
	{
		return (iMessagePriority >=
			FvDebugFilter::Instance().FilterThreshold() + iComponentPriority);
	}

	int FilterThreshold() const				{ return m_iFilterThreshold; }

	void FilterThreshold( int value )			{ m_iFilterThreshold = value; }

	bool HasDevelopmentAssertions() const	{ return m_bHasDevelopmentAssertions; }

	void HasDevelopmentAssertions( bool value )	{ m_bHasDevelopmentAssertions = value; }

	typedef std::vector<FvCriticalMessageCallback *> CriticalCallbacks;
	const CriticalCallbacks & GetCriticalCallbacks( ) const
		{ return m_kCriticalCallbacks; }

	void AddCriticalCallback( FvCriticalMessageCallback *pkCallback );
	void DeleteCriticalCallback( FvCriticalMessageCallback *pkCallback );

	typedef std::vector<FvDebugMessageCallback *> DebugCallbacks;
	const DebugCallbacks & GetMessageCallbacks( ) const
		{ return m_kMessageCallbacks; }

	void AddMessageCallback( FvDebugMessageCallback *pkCallback );
	void DeleteMessageCallback( FvDebugMessageCallback *pkCallback );


private:
	int m_iFilterThreshold;
	bool m_bHasDevelopmentAssertions;

	CriticalCallbacks m_kCriticalCallbacks;
	DebugCallbacks m_kMessageCallbacks;

	static FvDebugFilter *ms_pkInstance;
};

#endif // __FvDebugFilter_H__