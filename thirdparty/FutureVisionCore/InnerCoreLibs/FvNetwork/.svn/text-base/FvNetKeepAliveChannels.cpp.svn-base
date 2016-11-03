#include "FvNetKeepaliveChannels.h"
#include "FvNetChannel.h"
#include "FvNetNub.h"

static const float KEEP_ALIVE_PING_PERIOD = 2.5f; 		// seconds

//static const float KEEP_ALIVE_TIMEOUT = 60.f; 	// seconds
static const float KEEP_ALIVE_TIMEOUT = 200.f; 	// seconds

void FvNetKeepAliveChannels::AddIfNecessary( FvNetChannel & channel )
{
	if (channel.IsAnonymous())
	{
		FvNetMonitoredChannels::AddIfNecessary( channel );
	}
}


FvNetKeepAliveChannels::iterator & FvNetKeepAliveChannels::ChannelIter(
	FvNetChannel & channel ) const
{
	return channel.m_kKeepAliveIter;
}

float FvNetKeepAliveChannels::DefaultPeriod() const
{
	return KEEP_ALIVE_PING_PERIOD;
}

int FvNetKeepAliveChannels::HandleTimeout( FvNetTimerID, void * )
{
	const FvUInt64 inactivityPeriod =	FvUInt64( 2 * m_fPeriod * ::StampsPerSecond() );
	const FvUInt64 now = ::Timestamp();
	const FvUInt64 checkTime = now - inactivityPeriod;
	const FvUInt64 deadTime = now - 
			FvUInt64( KEEP_ALIVE_TIMEOUT * ::StampsPerSecond() );

	iterator iter = m_kChannels.begin();

	while (iter != m_kChannels.end())
	{
		FvNetChannel & channel = **iter++;

		if (channel.LastReceivedTime() < deadTime)
		{
			FV_ERROR_MSG( "FvNetKeepAliveChannels::Check: "
				"Channel to %s has timed out (%.3fs)\n",
				channel.c_str(),
				(now - channel.LastReceivedTime()) / StampsPerSecondD() );

			this->DelIfNecessary( channel );

			channel.HasRemoteFailed( true );
		}

		else if (channel.LastReceivedTime() < checkTime)
		{
			channel.Bundle().Reliable( FV_NET_RELIABLE_DRIVER );
			channel.Send();
		}
	}

	return 0;
}

