#include "FvProfile.h"
#include "FvTimestamp.h"
#include "FvBinaryStream.h"

//#define FV_ENABLE_WATCHERS 1

#if FV_ENABLE_WATCHERS

FvProfileVal::FvProfileVal( const FvString & name, FvProfileGroup * pGroup ) :
	m_kName( name ),
	m_pkGroup( pGroup )
{
	this->Reset();

	if (m_pkGroup == NULL)
	{
		m_pkGroup = &FvProfileGroup::DefaultGroup();
	}

	m_pkGroup->Add( this );
}


FvProfileVal::~FvProfileVal()
{
	if (m_pkGroup)
	{
		std::remove( m_pkGroup->Begin(), m_pkGroup->End(), this );
	}
}


FvWatcher & FvProfileVal::Watcher()
{
	static FvDataWatcher<FvProfileVal> *watchMe = NULL;

	if (watchMe == NULL)
	{
		FvProfileVal * pNull = NULL;
		watchMe = new FvDataWatcher<FvProfileVal>( *pNull, FvWatcher::WT_READ_WRITE );
	}

	return *watchMe;
}

FvWatcher & FvProfileVal::DetailWatcher()
{
	static FvDirectoryWatcher	*watchMe = NULL;

	if (watchMe == NULL)
	{
		watchMe = new FvDirectoryWatcher();
		FvProfileVal	* pNull = NULL;

		watchMe->AddChild( "count", new FvDataWatcher<long>(
							   (long&)pNull->m_uiCount, FvWatcher::WT_READ_ONLY ) );
		watchMe->AddChild( "inProgress", new FvDataWatcher<bool>(
							   (bool&)pNull->m_iInProgress, FvWatcher::WT_READ_ONLY ) );
		watchMe->AddChild( "sumQuantity", new FvDataWatcher<long>(
							   (long&)pNull->m_uiSumQuantity, FvWatcher::WT_READ_ONLY ) );
		watchMe->AddChild( "lastQuantity", new FvDataWatcher<long>(
							   (long&)pNull->m_uiLastQuantity, FvWatcher::WT_READ_ONLY ) );
		watchMe->AddChild( "sumTime", new FvDataWatcher<FvInt64>(
							   (FvInt64&)pNull->m_uiSumTime, FvWatcher::WT_READ_ONLY ) );
		watchMe->AddChild( "lastTime", new FvDataWatcher<FvInt64>(
							   (FvInt64&)pNull->m_uiLastTime, FvWatcher::WT_READ_ONLY ) );
		watchMe->AddChild( "sumIntTime", new FvDataWatcher<FvInt64>(
							   (FvInt64&)pNull->m_uiSumIntTime, FvWatcher::WT_READ_ONLY ) );
		watchMe->AddChild( "lastIntTime", new FvDataWatcher<FvInt64>(
							   (FvInt64&)pNull->m_uiLastIntTime, FvWatcher::WT_READ_ONLY ) );
	}

	return *watchMe;
}

std::ostream& operator<<( std::ostream & s, const FvProfileVal & val )
{
	FvUInt64 lastTime = val.m_iInProgress ?
		(Timestamp() - val.m_uiLastTime) : val.m_uiLastTime;

	s << "Count: " << val.m_uiCount;
	if (val.m_iInProgress) s << " (inProgress)";
	s << std::endl;

	if (val.m_uiCount == 0)
	{
		return s;
	}

	if (val.m_uiSumQuantity != 0)
	{
		s << "Quantity: " << val.m_uiSumQuantity <<
			" (last " << val.m_uiLastQuantity <<
			", avg " << ((double)val.m_uiSumQuantity)/val.m_uiCount << ')' << std::endl;
	}

	s << "Time: " << FvNiceTime( val.m_uiSumTime ) <<
	" (last " << FvNiceTime( lastTime ) << ')' << std::endl;

	if (val.m_uiSumTime != 0)
	{
		s << "Time/Count: " << FvNiceTime( val.m_uiSumTime / val.m_uiCount ) << std::endl;

		if (val.m_uiSumQuantity)
		{
			s << "Time/Qty: " <<
				FvNiceTime( val.m_uiSumTime / val.m_uiSumQuantity ) << std::endl;
		}
	}

	return s;
}

std::istream& operator>>( std::istream & s, FvProfileVal & v )
{
	v.Reset();
	FvProfileGroupResetter::instance().ResetIfDesired( v );
	return s;
}

FvProfileGroupPtr FvProfileGroup::ms_spDefaultGroup = NULL;

FvProfileGroup::FvProfileGroup( const char * watcherPath ) :
	m_spSummaries( new FvDirectoryWatcher() ),
	m_spDetails(  new FvDirectoryWatcher() )
{
	FvProfileVal * pRunningTime = new FvProfileVal( "runningTime", this );
	pRunningTime->Start();

	this->AddChild( "summaries", m_spSummaries );
	this->AddChild( "details", m_spDetails );

#ifdef FV_SERVER
	if (watcherPath)
	{
		FvWatcher::RootWatcher().AddChild( watcherPath, this );
	}
#endif
}

FvProfileGroup::~FvProfileGroup()
{
	delete this->pRunningTime();
}

void FvProfileGroup::Add( FvProfileVal * pVal )
{
	m_kProfiles.push_back( pVal );

	m_spSummaries->AddChild( pVal->c_str(), &FvProfileVal::Watcher(), pVal );
	m_spDetails->AddChild( pVal->c_str(), &FvProfileVal::DetailWatcher(), pVal );
}

FvUInt64 FvProfileGroup::RunningTime() const
{
	return ::Timestamp() - this->pRunningTime()->m_uiLastTime;
}

void FvProfileGroup::Reset()
{
	for (iterator iter = this->Begin(); iter != this->End(); ++iter)
	{
		(*iter)->Reset();
	}

	this->pRunningTime()->Start();
}

FvProfileGroup & FvProfileGroup::DefaultGroup()
{
	if (ms_spDefaultGroup == NULL)
	{
		ms_spDefaultGroup = new FvProfileGroup( "profiles" );
	}

	return *ms_spDefaultGroup;
}


FvProfileGroupResetter::FvProfileGroupResetter() :
	m_pkNominee( 0 ),
	m_kGroups( 0 ),
	m_bDoingReset( false )
{
}

FvProfileGroupResetter::~FvProfileGroupResetter()
{
}

void FvProfileGroupResetter::NominateProfileVal( FvProfileVal * pVal )
{
	m_pkNominee = pVal;
}

void FvProfileGroupResetter::AddProfileGroup( FvProfileGroup * pGroup )
{
	if (m_bDoingReset) return;

	m_kGroups.push_back( pGroup );
}

FvProfileGroupResetter & FvProfileGroupResetter::instance()
{
	static FvProfileGroupResetter staticInstance;
	return staticInstance;
}

void FvProfileGroupResetter::ResetIfDesired( FvProfileVal & val )
{
	if (m_bDoingReset) return;
	if (m_pkNominee != &val) return;

	m_bDoingReset = true;

	std::vector<FvProfileGroup *>::iterator iter;

	for (iter = m_kGroups.begin(); iter != m_kGroups.end(); iter++)
	{
		(*iter)->Reset();
	}

	m_bDoingReset = false;
}

#endif // FV_ENABLE_WATCHERS

std::ostream& operator<<(std::ostream &o, const FvNiceTime &nt)
{
	static double microsecondsPerStamp = 1000000.0 / StampsPerSecondD();

	double microDouble = ((double)(FvInt64)nt.m_uiTime) * microsecondsPerStamp;
	FvUInt64 micros = (FvUInt64)microDouble;

	double truncatedStampDouble = (double)(FvInt64)
		( nt.m_uiTime - ((FvUInt64)(((double)(FvInt64)micros)/microsecondsPerStamp)));
	FvUInt32 picos = (FvUInt32)( truncatedStampDouble * 1000000.0 *
		microsecondsPerStamp );

	if (picos > 1000000)
	{
		FvUInt32 negPicos = 1 + ~picos;
		if (negPicos > 1000000)
		{
			picos = 0;
		}
		else
		{
			picos = negPicos;
			micros--;
		}
	}

	if (micros == 0 && picos == 0)
	{
		o << "0us";
		return o;
	}

	if (micros>=1000000)
	{
		FvUInt32 s = FvUInt32(micros/1000000);

		if (s>=60)
		{
			o << (s/60) << "min ";
		}
		o << (s%60) << "sec ";
	}

	char	lastBit[32];
	FvSNPrintf( lastBit, sizeof(lastBit), "%03d %03d",
		(int)(FvUInt32)((micros/1000)%1000),
		(int)(FvUInt32)(micros%1000) );
	o << lastBit;

	{
		FvSNPrintf(lastBit, sizeof(lastBit), ".%03d",
			(int)((picos/1000)%1000) );
		o << lastBit;
	}

	o << "us";

	return o;
}