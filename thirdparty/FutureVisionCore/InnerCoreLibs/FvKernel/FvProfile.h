//{future header message}
#ifndef __FvProfile_H__
#define __FvProfile_H__

#include "FvKernel.h"
#include "FvSmartPointer.h"
#include <iostream>

//#define FV_ENABLE_WATCHERS 1

#if FV_ENABLE_WATCHERS

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include "FvWatcher.h"
#include "FvTimestamp.h"

class FvProfileVal;
class FvProfileGroup;
typedef FvSmartPointer< FvProfileGroup > FvProfileGroupPtr;

class FV_KERNEL_API FvProfileGroup : public FvDirectoryWatcher
{
public:
	explicit FvProfileGroup( const char * watcherPath = NULL );
	~FvProfileGroup();

	typedef std::vector< FvProfileVal* > Profiles;
	typedef Profiles::iterator iterator;

	iterator Begin() { return m_kProfiles.begin(); }
	iterator End() { return m_kProfiles.end(); }

	Profiles &Stack() { return m_kStack; }
	void Add( FvProfileVal * pVal );
	void Reset();

	FvProfileVal *pRunningTime() { return m_kProfiles[0]; }
	const FvProfileVal *pRunningTime() const { return m_kProfiles[0]; }
	FvUInt64 RunningTime() const;

	static FvProfileGroup &DefaultGroup();

private:
	Profiles m_kProfiles;

	Profiles m_kStack;

	FvDirectoryWatcherPtr m_spSummaries;
	FvDirectoryWatcherPtr m_spDetails;

	static FvProfileGroupPtr ms_spDefaultGroup;
};


class FV_KERNEL_API FvProfileVal
{
public:
	FvProfileVal( const FvString & name, FvProfileGroup * pGroup = NULL );
	~FvProfileVal();

	void Start()
	{
		FvUInt64 now = ::Timestamp();

		if (m_iInProgress == 0)
		{
			m_uiLastTime = now;
		}

		++m_iInProgress;

		FvProfileGroup::Profiles & stack = m_pkGroup->Stack();

		if (!stack.empty())
		{
			FvProfileVal & profile = *stack.back();
			profile.m_uiLastIntTime = now - profile.m_uiLastIntTime;
			profile.m_uiSumIntTime += profile.m_uiLastIntTime;
		}

		stack.push_back( this );
		m_uiLastIntTime = now;
	}


	void Stop( FvUInt32 qty = 0 )
	{
		FvUInt64 now = ::Timestamp();

		if (--m_iInProgress == 0)
		{
			m_uiLastTime = now - m_uiLastTime;
			m_uiSumTime += m_uiLastTime;

		}

		m_uiLastQuantity = qty;
		m_uiSumQuantity += qty;

		++m_uiCount;

		FvProfileGroup::Profiles & stack = m_pkGroup->Stack();
		FV_ASSERT( stack.back() == this );
		stack.pop_back();

		m_uiLastIntTime = now - m_uiLastIntTime;
		m_uiSumIntTime += m_uiLastIntTime;

		if (!stack.empty())
		{
			stack.back()->m_uiLastIntTime = now;
		}
	}

	void Reset()
	{
		m_uiLastTime = 0;
		m_uiSumTime = 0;
		m_uiLastIntTime = 0;
		m_uiSumIntTime = 0;
		m_uiLastQuantity = 0;
		m_uiSumQuantity = 0;
		m_uiCount = 0;
		m_iInProgress = 0;
	}

	bool Running() const
	{
		return m_iInProgress > 0;
	}

	const char *c_str() const { return m_kName.c_str(); }

	FvString m_kName;

	FvProfileGroup *m_pkGroup;

	FvUInt64 m_uiLastTime;		
	FvUInt64 m_uiSumTime;		
	FvUInt64 m_uiLastIntTime;	
	FvUInt64 m_uiSumIntTime;
	FvUInt32 m_uiLastQuantity;
	FvUInt32 m_uiSumQuantity;
	FvUInt32 m_uiCount;	
	int	m_iInProgress;

	static FvWatcher &Watcher();
	static FvWatcher &DetailWatcher();
};

std::ostream& operator<<( std::ostream &s, const FvProfileVal &v );

std::istream& operator>>( std::istream &s, FvProfileVal &v );


struct FV_KERNEL_API FvNiceTime
{
	explicit FvNiceTime(FvUInt64 t) : m_uiTime(t) {}

	FvUInt64 m_uiTime;
};

FV_KERNEL_API std::ostream& operator<<( std::ostream &o, const FvNiceTime &nt );

class FV_KERNEL_API FvProfileGroupResetter
{
public:
	FvProfileGroupResetter();
	~FvProfileGroupResetter();

	void NominateProfileVal( FvProfileVal * pVal = NULL );
	void AddProfileGroup( FvProfileGroup * pGroup );

	static FvProfileGroupResetter & instance();

private:
	FvProfileVal *m_pkNominee;

	std::vector< FvProfileGroup * >	m_kGroups;

	bool m_bDoingReset;

	void ResetIfDesired( FvProfileVal & val );

	friend std::istream& operator>>( std::istream &s, FvProfileVal &v );
};

class FV_KERNEL_API FvMethodProfiler
{
public:
	FvMethodProfiler( FvProfileVal & profileVal ) :
		m_kProfileVal( profileVal )
	{
		m_kProfileVal.Start();
	}

	~FvMethodProfiler()
	{
		m_kProfileVal.Stop();
	}

private:
	FvProfileVal &m_kProfileVal;
};

#define FV_SCOPED_PROFILER( val ) FvMethodProfiler __profilerScope( val );

#else // FV_ENABLE_WATCHERS

struct FV_KERNEL_API FvNiceTime
{
	explicit FvNiceTime(FvUInt64 t) : m_uiTime(t) {}

	FvUInt64 m_uiTime;
};

FV_KERNEL_API std::ostream& operator<<( std::ostream &o, const FvNiceTime &nt );

#define FV_SCOPED_PROFILER( val )

#endif // !FV_ENABLE_WATCHERS

#endif // __FvProfile_H__