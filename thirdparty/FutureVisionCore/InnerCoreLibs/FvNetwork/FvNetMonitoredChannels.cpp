#include "FvNetMonitoredChannels.h"
#include "FvNetChannel.h"
#include "FvNetNub.h"

void FvNetMonitoredChannels::SetPeriod( float seconds, FvNetNub & nub )
{
	if (m_kTimerID != FV_NET_TIMER_ID_NONE)
	{
		nub.CancelTimer( m_kTimerID );
	}

	if (seconds > 0.f)
	{
		m_kTimerID = nub.RegisterTimer( int( seconds * 1000000 ), this );
	}
	m_fPeriod = seconds;
}

void FvNetMonitoredChannels::StopMonitoring( FvNetNub & nub )
{
	this->SetPeriod( 0.f, nub );
}

void FvNetMonitoredChannels::AddIfNecessary( FvNetChannel & channel )
{
	iterator & iter = this->ChannelIter( channel );

	if (iter == m_kChannels.end())
	{
		iter = m_kChannels.insert( m_kChannels.end(), &channel );

		if (m_kTimerID == FV_NET_TIMER_ID_NONE)
		{
			this->SetPeriod( this->DefaultPeriod(), channel.nub() );
		}
	}
}

void FvNetMonitoredChannels::DelIfNecessary( FvNetChannel & channel )
{
	iterator & iter = this->ChannelIter( channel );

	if (iter != m_kChannels.end())
	{
		m_kChannels.erase( iter );
		iter = m_kChannels.end();
	}
}
