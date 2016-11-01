#include "FvNetCondemnedChannels.h"
#include "FvNetChannel.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 )

void FvNetCondemnedChannels::Add( FvNetChannel * pChannel )
{
	if (this->ShouldDelete( pChannel, ::Timestamp() ))
	{
		pChannel->Destroy();
	}
	else
	{
		if (pChannel->IsIndexed())
		{
			FvNetChannel *& rpChannel = m_kIndexedChannels[ pChannel->id() ];

			if (rpChannel)
			{
				rpChannel->Destroy();
				FV_WARNING_MSG( "FvNetNub::DeleteChannel( %s ): "
								"Already have a channel with id %d\n",
							pChannel->c_str(),
							pChannel->id() );
			}

			rpChannel = pChannel;
		}
		else
		{
			m_kNonIndexedChannels.push_back( pChannel );
		}

		if (m_kTimerID == FV_NET_TIMER_ID_NONE)
		{
			const int seconds = AGE_LIMIT;
			m_kTimerID =
				pChannel->nub().RegisterTimer( int( seconds * 1000000 ), this );
		}
	}
}

FvNetChannel * FvNetCondemnedChannels::Find( FvNetChannelID channelID ) const
{
	IndexedChannels::const_iterator iter =
		m_kIndexedChannels.find( channelID );

	return (iter != m_kIndexedChannels.end()) ? iter->second : NULL;
}

bool FvNetCondemnedChannels::ShouldDelete( FvNetChannel * pChannel, FvUInt64 now )
{
	const FvUInt64 ageLimit = AGE_LIMIT * StampsPerSecond();

	bool shouldDelete =
		!pChannel->HasUnackedPackets() || pChannel->HasRemoteFailed();

	if (!shouldDelete &&
		(pChannel->LastReceivedTime() + ageLimit < now) &&
		(pChannel->LastReliableSendTime() + ageLimit < now))
	{
		FV_WARNING_MSG( "FvNetCondemnedChannels::HandleTimeout: "
						"Condemned channel %s has timed out.\n",
					pChannel->c_str() );
		shouldDelete = true;
	}

	return shouldDelete;
}

bool FvNetCondemnedChannels::DeleteFinishedChannels()
{
	FvNetNub * pNub = NULL;
	FvUInt64 now = ::Timestamp();

	{
		NonIndexedChannels::iterator iter = m_kNonIndexedChannels.begin();

		while (iter != m_kNonIndexedChannels.end())
		{
			FvNetChannel * pChannel = *iter;
			NonIndexedChannels::iterator oldIter = iter;
			++iter;

			pNub = &pChannel->nub();

			if (this->ShouldDelete( pChannel, now ))
			{
				pChannel->Destroy();
				m_kNonIndexedChannels.erase( oldIter );
			}
		}
	}

	{
		IndexedChannels::iterator iter = m_kIndexedChannels.begin();

		while (iter != m_kIndexedChannels.end())
		{
			FvNetChannel * pChannel = iter->second;
			IndexedChannels::iterator oldIter = iter;
			++iter;

			pNub = &pChannel->nub();

			if (this->ShouldDelete( pChannel, now ))
			{
				pChannel->Destroy();
				m_kIndexedChannels.erase( oldIter );
			}
		}
	}

	bool isEmpty = m_kNonIndexedChannels.empty() && m_kIndexedChannels.empty();

	if (isEmpty && m_kTimerID)
	{
		FV_ASSERT( pNub );
		pNub->CancelTimer( m_kTimerID );
		m_kTimerID = 0;
	}

	return isEmpty;
}

int FvNetCondemnedChannels::NumCriticalChannels() const
{
	int count = 0;

	for (NonIndexedChannels::const_iterator iter = m_kNonIndexedChannels.begin();
		 iter != m_kNonIndexedChannels.end(); ++iter)
	{
		if ((*iter)->HasUnackedCriticals())
		{
			++count;
		}
	}

	for (IndexedChannels::const_iterator iter = m_kIndexedChannels.begin();
		 iter != m_kIndexedChannels.end(); ++iter)
	{
		if (iter->second->HasUnackedCriticals())
		{
			++count;
		}
	}

	return count;
}

int FvNetCondemnedChannels::HandleTimeout( FvNetTimerID, void * )
{
	this->DeleteFinishedChannels();

	return 0;
}
