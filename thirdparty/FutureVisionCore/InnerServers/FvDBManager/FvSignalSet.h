//{future header message}
#ifndef __FvSignalSet_H__
#define __FvSignalSet_H__

#include <FvDebug.h>

#include <signal.h>

class FvSignalSet
{
public:
	enum InitState
	{
		EMPTY, FULL
	};

	FvSignalSet( InitState state = EMPTY ) : m_bIsClear( state == EMPTY )
	{
		if (state == EMPTY)
		{
			sigemptyset( &m_kSignals );
		}
		else
		{
			sigfillset( &m_kSignals );
		}
	}

	void Set( int signal )
	{
		FV_VERIFY( sigaddset( &m_kSignals, signal ) == 0 );
		m_bIsClear = false;
	}
	void Clear()
	{
		FV_VERIFY( sigemptyset( &m_kSignals ) == 0 );
		m_bIsClear = true;
	}

	bool IsClear() const			{ return m_bIsClear; }
	bool IsSet( int signal ) const
	{
		return sigismember( &m_kSignals, signal ) ? true : false;
	}

	operator const sigset_t*() const 	{ return &m_kSignals; }

private:
	sigset_t m_kSignals;
	bool m_bIsClear;
};

class FvSignalBlocker
{
public:
	FvSignalBlocker( const FvSignalSet &kBlockSignals )
	{
		FV_VERIFY( pthread_sigmask( SIG_SETMASK, kBlockSignals, &m_kOldSignals )
				== 0 );
	}
	~FvSignalBlocker()
	{
		FV_VERIFY( pthread_sigmask( SIG_SETMASK, &m_kOldSignals, NULL ) == 0 );
	}

private:
	sigset_t m_kOldSignals;
};


#endif // __FvSignalSet_H__
