#include "FvNetIrregularChannels.h"
#include "FvNetChannel.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 )

void FvNetIrregularChannels::AddIfNecessary( FvNetChannel & channel )
{
	if (channel.m_bIsIrregular &&
		channel.HasUnackedPackets())
	{
		FvNetMonitoredChannels::AddIfNecessary( channel );
	}
}

FvNetIrregularChannels::iterator & FvNetIrregularChannels::ChannelIter(
	FvNetChannel & channel ) const
{
	return channel.m_kIrregularIter;
}


float FvNetIrregularChannels::DefaultPeriod() const
{
	return 1.f;
}

int FvNetIrregularChannels::HandleTimeout( FvNetTimerID, void * )
{
	iterator iter = m_kChannels.begin();

	while (iter != m_kChannels.end())
	{
		FvNetChannel & channel = **iter++;

		if (channel.HasUnackedPackets() && channel.IsIrregular())
		{
			channel.Send();

			if (channel.HasRemoteFailed())
			{
				FV_ERROR_MSG( "FvNetIrregularChannels::HandleTimeout: "
					"Removing dead channel to %s\n",
					channel.c_str() );

				this->DelIfNecessary( channel );
			}
		}
		else
		{
			this->DelIfNecessary( channel );
		}
	}

	return 0;
}
