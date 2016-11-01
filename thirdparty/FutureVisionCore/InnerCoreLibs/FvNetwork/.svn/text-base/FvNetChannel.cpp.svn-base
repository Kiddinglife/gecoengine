#include "FvNetChannel.h"
#include <FvXMLSection.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 )

const int EXTERNAL_CHANNEL_SIZE = 256;
const int INTERNAL_CHANNEL_SIZE = 4096;
const int INDEXED_CHANNEL_SIZE = 512;

unsigned int FvNetChannel::ms_uiMaxOverflowPackets[] =
{ 
	1024, // External channel.
	8192, // Internal channel
	4096  // Indexed channel (ie: entity channel).
};

bool FvNetChannel::ms_bAssertOnMaxOverflowPackets = false;

int FvNetChannel::ms_iSendWindowWarnThresholds[] =
	{ INTERNAL_CHANNEL_SIZE / 4, INDEXED_CHANNEL_SIZE / 4 };

int FvNetChannel::ms_iSendWindowCallbackThreshold = INDEXED_CHANNEL_SIZE/2;
FvNetChannel::SendWindowCallback FvNetChannel::ms_pfSendWindowCallback = NULL;

namespace
{

class WatcherIniter
{
public:
	WatcherIniter()
	{
		FvNetChannel::StaticInit();
	}
};

WatcherIniter ms_kWatcherIniter;

} 

FvNetChannel::FvNetChannel( FvNetNub & nub, const FvNetAddress & address, Traits traits,
		float minInactivityResendDelay,
		FvNetPacketFilterPtr pFilter, FvNetChannelID id ):
	m_pkNub( &nub ),
	m_eTraits( traits ),
	m_iID( id ),
	m_kChannelPushTimerID( FV_NET_TIMER_ID_NONE ),
	m_kInactivityTimerID( FV_NET_TIMER_ID_NONE ),
	m_uiInactivityExceptionPeriod( 0 ),
	m_uiVersion( 0 ),
	m_uiLastReceivedTime( 0 ),
	m_spFilter( pFilter ),
	m_kAddr( FvNetAddress::NONE ),
	m_pkBundle( NULL ),
	m_uiWindowSize(	(traits != INTERNAL)    ? EXTERNAL_CHANNEL_SIZE :
					(id == FV_NET_CHANNEL_ID_NULL) ? INTERNAL_CHANNEL_SIZE :
											  INDEXED_CHANNEL_SIZE ),
	m_uiSmallOutSeqAt( 0 ),
	m_uiLargeOutSeqAt( 0 ),
	m_uiFirstMissing( SEQ_NULL ),
	m_uiLastMissing( SEQ_NULL ),
	m_uiOldestUnackedSeq( SEQ_NULL ),
	m_uiLastReliableSendTime( 0 ),
	m_uiLastReliableResendTime( 0 ),
	m_uiRoundTripTime( (traits == INTERNAL) ?
		StampsPerSecond() / 10 : StampsPerSecond() ),
	m_uiMinInactivityResendDelay(
		FvUInt64( minInactivityResendDelay * StampsPerSecond() ) ),
	m_uiUnreliableInSeqAt( SEQ_NULL ),
	m_kUnackedPackets( m_uiWindowSize ),
	m_bHasSeenOverflowWarning( false ),
	m_uiInSeqAt( 0 ),
	m_kBufferedReceives( m_uiWindowSize ),
	m_uiNumBufferedReceives( 0 ),
	m_spFragments( NULL ),
	m_uiLastAck( SeqMask( m_uiSmallOutSeqAt - 1 ) ),
	m_kIrregularIter( nub.IrregularChannels().End() ),
	m_kKeepAliveIter( nub.KeepAliveChannels().End() ),
	m_bIsIrregular( false ),
	m_bIsCondemned( false ),
	m_bIsDestroyed( false ),
	m_pkBundlePrimer( NULL ),
	m_bHasRemoteFailed( false ),
	m_bIsAnonymous( false ),
	m_uiUnackedCriticalSeq( SEQ_NULL ),
	m_iPushUnsentAcksThreshold( 0 ),
	m_bShouldAutoSwitchToSrcAddr( false ),
	m_bWantsFirstPacket( false ),
	m_bShouldDropNextSend( false ),

	m_uiNumPacketsSent( 0 ),
	m_uiNumPacketsReceived( 0 ),
	m_uiNumBytesSent( 0 ),
	m_uiNumBytesReceived( 0 ),
	m_uiNumPacketsResent( 0 ),
	m_uiNumReliablePacketsSent( 0 ),

	m_spMessageFilter( NULL )
{
	this->IncRef();

	if (m_spFilter && m_iID != FV_NET_CHANNEL_ID_NULL)
	{
		FV_CRITICAL_MSG( "FvNetChannel::FvNetChannel: "
			"PacketFilters are not supported on indexed channels (id:%d)\n",
			m_iID );
	}

	this->ClearBundle();

	this->addr( address );

	for (unsigned int i = 0; i < m_kUnackedPackets.size(); i++)
	{
		m_kUnackedPackets[i] = NULL;
		m_kBufferedReceives[i] = NULL;
	}
}


void FvNetChannel::StaticInit()
{

#ifdef FV_SERVER
	// This is only interesting on the server.

	FV_WATCH( "network/internalSendWindowSizeThreshold",
		ms_iSendWindowWarnThresholds[ 0 ] );

	FV_WATCH( "network/indexedSendWindowSizeThreshold",
		ms_iSendWindowWarnThresholds[ 1 ] );
#endif

}


FvNetChannel * FvNetChannel::get( FvNetNub & nub, const FvNetAddress & address )
{
	FvNetChannel * pChannel = nub.FindChannel( address );

	if (pChannel)
	{
		FV_ASSERT( pChannel->IsAnonymous() );

		pChannel->IsAnonymous( false );

		FV_INFO_MSG( "FvNetChannel::get: "
			"Claimed anonymous channel to %s\n",
			pChannel->c_str() );

		if (pChannel->IsCondemned())
		{
			FV_WARNING_MSG( "FvNetChannel::get: "
				"Returned condemned channel to %s\n",
				pChannel->c_str() );
		}
	}
	else
	{
		pChannel = new FvNetChannel( nub, address, INTERNAL );
	}

	return pChannel;
}


void FvNetChannel::addr( const FvNetAddress & addr )
{
	if (m_kAddr != addr)
	{
		m_uiLastReceivedTime = ::Timestamp();

		if (!this->IsIndexed())
		{
			if (m_kAddr != FvNetAddress::NONE)
			{
				FV_VERIFY( m_pkNub->DeregisterChannel( *this ) );
			}

			m_kAddr = addr;

			if (m_kAddr != FvNetAddress::NONE)
			{
				FV_VERIFY( m_pkNub->RegisterChannel( *this ) );
			}
		}
		else
		{
			m_kAddr = addr;
		}
	}
}


FvNetChannel::~FvNetChannel()
{
	FV_ASSERT( m_bIsDestroyed );

	m_pkNub->OnChannelGone( this );

	this->Reset( FvNetAddress::NONE );

	if (m_kChannelPushTimerID != FV_NET_TIMER_ID_NONE)
	{
		m_pkNub->CancelTimer( m_kChannelPushTimerID );
	}

	delete m_pkBundle;
}


void FvNetChannel::Condemn()
{
	if (this->IsCondemned())
	{
		FV_WARNING_MSG( "Channel::condemn( %s ): Already condemned.\n",
			   this->c_str() );
		return;
	}

	if (this->HasUnsentData())
	{
		if (this->IsEstablished())
		{
			this->Send();
		}
		else
		{
			FV_WARNING_MSG( "FvNetChannel::Condemn( %s ): "
				"Unsent data was lost because channel not established\n",
				this->c_str() );
		}
	}

	this->IsIrregular( true );

	m_bIsCondemned = true;

	m_pkNub->CondemnedChannels().Add( this );
}


void FvNetChannel::Destroy()
{
	FV_IF_NOT_ASSERT_DEV( !m_bIsDestroyed )
	{
		return;
	}

	m_bIsDestroyed = true;

	this->DecRef();
}


void FvNetChannel::AddOverflowPacket( UnackedPacket * pPacket )
{
	unsigned int maxOverflowPackets = this->GetMaxOverflowPackets();

	if (maxOverflowPackets != 0)
	{
		if (ms_bAssertOnMaxOverflowPackets)
		{
			FV_ASSERT( m_kOverflowPackets.size() < maxOverflowPackets);
		}

		if (m_kOverflowPackets.size() > (maxOverflowPackets / 2))
		{
			if (!m_bHasSeenOverflowWarning)
			{
				FV_WARNING_MSG( "FvNetChannel::AddOverflowPacket: Overflow packet "
					"list size (%u) exceeding safety threshold (%u).\n",
					m_kOverflowPackets.size(), (maxOverflowPackets / 2) );
				m_bHasSeenOverflowWarning = true;
			}
		}
		else if (m_bHasSeenOverflowWarning)
		{
			if (m_kOverflowPackets.size() < (maxOverflowPackets / 3))
			{
				m_bHasSeenOverflowWarning = false;
			}
		}
	}

	m_kOverflowPackets.push_back( pPacket );
}


void FvNetChannel::InitFromStream( FvBinaryIStream & data,
	   const FvNetAddress & addr )
{
	FvUInt64 timeNow = Timestamp();
	m_uiLastReceivedTime = timeNow;
	m_kAddr = addr;

	data >> m_uiVersion;
	data >> m_uiSmallOutSeqAt;
	data >> m_uiLargeOutSeqAt;
	data >> m_uiOldestUnackedSeq;

	FvUInt32 count = (m_uiOldestUnackedSeq == SEQ_NULL) ?
					0 : SeqMask( m_uiLargeOutSeqAt - m_uiOldestUnackedSeq );

	m_uiLastAck = (m_uiOldestUnackedSeq != SEQ_NULL) ?
		SeqMask( m_uiOldestUnackedSeq - 1 ) : SeqMask( m_uiSmallOutSeqAt - 1 );

	m_uiFirstMissing = SEQ_NULL;
	m_uiLastMissing = SEQ_NULL;

	for (FvUInt32 i = 0; i < count; ++i)
	{
		FvNetSeqNum currSeq = SeqMask( m_uiOldestUnackedSeq + i );

		UnackedPacket * pUnacked = UnackedPacket::initFromStream( data, timeNow );

		if (i >= m_uiWindowSize)
		{
			FV_ASSERT( pUnacked );
			this->AddOverflowPacket( pUnacked );
		}
		else if (pUnacked)
		{
			m_kUnackedPackets[ currSeq ] = pUnacked;
		}
		else
		{
			m_uiLastAck = currSeq;

			if (m_uiFirstMissing == SEQ_NULL)
			{
				m_uiFirstMissing = m_uiOldestUnackedSeq;
				m_uiLastMissing = SeqMask( currSeq - 1 );
			}

			else if (m_kUnackedPackets[ currSeq - 1 ])
			{
				m_uiLastMissing = SeqMask( currSeq - 1 );
			}
		}
	}

	//! add by Uman, 20100306, 这里需要判断m_uiFirstMissing是否等于SEQ_NULL,可能会导致死循环
	if(m_uiFirstMissing != SEQ_NULL)
	{
		FvNetSeqNum seq = m_uiFirstMissing;
		UnackedPacket * pPrev = m_kUnackedPackets[ m_uiFirstMissing ];

		while (seq != m_uiLastMissing)
		{
			seq = SeqMask( seq + 1 );

			UnackedPacket * pCurr = m_kUnackedPackets[ seq ];

			if (pCurr)
			{
				pPrev->m_uiNextMissing = seq;
				pPrev = pCurr;
			}
		}
	}

	FvNetSeqNum			firstMissing;
	FvNetSeqNum			lastMissing;
	FvUInt32			lastAck;

	data >> firstMissing >> lastMissing >> lastAck;

	FV_ASSERT( firstMissing == m_uiFirstMissing );
	FV_ASSERT( lastMissing == m_uiLastMissing );
	FV_ASSERT( lastAck == m_uiLastAck );

	m_uiLastReliableSendTime = timeNow;
	m_uiLastReliableResendTime = timeNow;

	m_uiRoundTripTime = m_uiMinInactivityResendDelay / 2;

	data >> m_uiInSeqAt;
	data >> m_uiNumBufferedReceives;
	int numToReceive = m_uiNumBufferedReceives;

	for (FvUInt32 i = 1; i < m_uiWindowSize && numToReceive > 0; ++i)
	{
		FvNetPacketPtr pPacket =
			FvNetPacket::CreateFromStream( data, FvNetPacket::BUFFERED_RECEIVE );

		m_kBufferedReceives[ m_uiInSeqAt + i ] = pPacket;

		if (pPacket)
		{
			--numToReceive;
		}
	}

	FvUInt16 numChainedFragments;
	FvNetPacket * pPrevPacket = NULL;

	data >> numChainedFragments;

	for (FvUInt16 i = 0; i < numChainedFragments; i++)
	{
		FvNetPacketPtr pPacket =
			FvNetPacket::CreateFromStream( data, FvNetPacket::CHAINED_FRAGMENT );

		if (m_spFragments == NULL)
		{
			m_spFragments = new FvNetNub::FragmentedBundle(
				pPacket->FragEnd(),
				pPacket->FragEnd() - pPacket->GetSeq() + 1 - numChainedFragments,
				Timestamp(),
				pPacket.GetObject() );
		}
		else
		{
			pPrevPacket->Chain( pPacket.GetObject() );
		}

		pPrevPacket = pPacket.GetObject();
	}

	data >> m_uiUnackedCriticalSeq >> m_bWantsFirstPacket;

	m_pkNub->IrregularChannels().AddIfNecessary( *this );

#if 0
	data >> numPacketsSent_;
	data >> numPacketsReceived_;
	data >> numBytesSent_;
	data >> numBytesReceived_;
	data >> numPacketsResent_;
#endif

	FV_ASSERT( m_uiFirstMissing == SEQ_NULL ||
			m_kUnackedPackets[ m_uiFirstMissing ] != NULL );
	FV_ASSERT( (m_uiFirstMissing == SEQ_NULL) == (m_uiLastMissing == SEQ_NULL) );
	FV_ASSERT( (m_uiFirstMissing == SEQ_NULL) ||
			(m_uiFirstMissing == m_uiLastMissing) ||
			(m_kUnackedPackets[ m_uiFirstMissing ]->m_uiNextMissing <= m_uiLastMissing) );
	FV_ASSERT( m_uiFirstMissing <= m_uiLastMissing );
}


void FvNetChannel::AddToStream( FvBinaryOStream & data )
{
	if (this->HasUnsentData())
	{
		this->Send();
	}

	data << SeqMask( m_uiVersion + 1 );

	data << m_uiSmallOutSeqAt;
	data << m_uiLargeOutSeqAt;
	data << m_uiOldestUnackedSeq;

	FvUInt32 count = this->SendWindowUsage();

	FV_ASSERT( (count == 0) || m_kUnackedPackets[ m_uiOldestUnackedSeq ] );

	for (FvUInt32 i = 0; i < std::min( count, m_uiWindowSize ); ++i)
	{
		UnackedPacket::addToStream( m_kUnackedPackets[ m_uiOldestUnackedSeq + i ],
				data );
	}

	FV_ASSERT( m_kOverflowPackets.empty() ||
		(count == m_uiWindowSize + m_kOverflowPackets.size()) );

	for (OverflowPackets::iterator iter = m_kOverflowPackets.begin();
			iter != m_kOverflowPackets.end();
			++iter)
	{
		UnackedPacket::addToStream( *iter, data );
	}

	data << m_uiFirstMissing << m_uiLastMissing << m_uiLastAck;

	data << m_uiInSeqAt;

	data << m_uiNumBufferedReceives;
	int numToSend = m_uiNumBufferedReceives;

	for (FvUInt32 i = 1; i < m_uiWindowSize && numToSend > 0; ++i)
	{
		const FvNetPacket * pPacket = m_kBufferedReceives[ m_uiInSeqAt + i ].GetObject();
		FvNetPacket::AddToStream( data, pPacket, FvNetPacket::BUFFERED_RECEIVE );

		if (pPacket)
		{
			--numToSend;
		}
	}

	if (m_spFragments)
	{
		data << (FvUInt16)m_spFragments->m_spChain->ChainLength();

		for (const FvNetPacket * p = m_spFragments->m_spChain.GetObject();
			 p != NULL; p = p->Next())
		{
			FvNetPacket::AddToStream( data, p, FvNetPacket::CHAINED_FRAGMENT );
		}
	}
	else
	{
		data << (FvUInt16)0;
	}

	data << m_uiUnackedCriticalSeq << m_bWantsFirstPacket;

	FV_ASSERT( !m_bHasRemoteFailed );

#if 0
	data << numPacketsSent_;
	data << numPacketsReceived_;
	data << numBytesSent_;
	data << numBytesReceived_;
	data << numPacketsResent_;
#endif
}


void FvNetChannel::SendEvery( int microseconds )
{
	if (m_kChannelPushTimerID != FV_NET_TIMER_ID_NONE)
	{
		m_pkNub->CancelTimer( m_kChannelPushTimerID );
		m_kChannelPushTimerID = FV_NET_TIMER_ID_NONE;
	}

	if (microseconds)
	{
		m_kChannelPushTimerID = m_pkNub->RegisterTimer(
				microseconds, this, (void*)TIMEOUT_CHANNEL_PUSH );
	}
}


FvNetBundle & FvNetChannel::Bundle()
{
	return *m_pkBundle;
}


const FvNetBundle & FvNetChannel::Bundle() const
{
	return *m_pkBundle;
}


bool FvNetChannel::HasUnsentData() const
{
	const int primeMessages =
		m_pkBundlePrimer ? m_pkBundlePrimer->NumUnreliableMessages() : 0;

	return m_pkBundle->NumMessages() > primeMessages ||
		m_pkBundle->HasDataFooters() ||
		m_pkBundle->isReliable();
}


void FvNetChannel::Send( FvNetBundle * pBundle )
{
	if (m_bHasRemoteFailed)
	{
		FV_WARNING_MSG( "FvNetChannel::Send( %s ): "
			"Not doing anything due to remote process failure\n",
			this->c_str() );

		return;
	}

	bool isSendingOwnBundle = (pBundle == NULL);

	if (!isSendingOwnBundle)
	{
		FV_ASSERT( !this->IsIndexed() );

		if (this->ShouldSendFirstReliablePacket())
		{
			pBundle->m_spFirstPacket->EnableFlags( FvNetPacket::FLAG_CREATE_CHANNEL );
		}

		FV_ASSERT( !m_pkBundlePrimer );
	}
	else
	{
		pBundle = m_pkBundle;
	}

	FV_ASSERT( this->IsExternal() ||
		pBundle->NumMessages() == 0 ||
		pBundle->isReliable() );

	this->CheckResendTimers();

	if (isSendingOwnBundle && !this->HasUnsentData())
	{
		return;
	}

	if (m_bShouldDropNextSend)
	{
		m_pkNub->DropNextSend();
		m_bShouldDropNextSend = false;
	}

	m_pkNub->Send( m_kAddr, *pBundle, this );

	++m_uiNumPacketsSent;
	m_uiNumBytesSent += pBundle->Size();

	if (pBundle->isReliable())
	{
		++m_uiNumReliablePacketsSent;
	}

	m_pkNub->IrregularChannels().AddIfNecessary( *this );

	if (pBundle->IsCritical())
	{
		m_uiUnackedCriticalSeq =
			pBundle->m_spFirstPacket->GetSeq() + pBundle->SizeInPackets() - 1;
	}

	if (isSendingOwnBundle)
	{
		this->ClearBundle();
	}
	else
	{
		pBundle->Clear();
	}
}


void FvNetChannel::DelayedSend()
{
	if (this->IsIrregular())
	{
		m_pkNub->DelayedSend( this );
	}
}


void FvNetChannel::SendIfIdle()
{
	if (this->IsEstablished())
	{
		if (this->LastReliableSendOrResendTime() <
				::Timestamp() - m_uiMinInactivityResendDelay/2)
		{
			this->Send();
		}
	}
}


bool FvNetChannel::AddResendTimer( FvNetSeqNum seq, FvNetPacket * p,
		const FvNetReliableOrder * roBeg, const FvNetReliableOrder * roEnd )
{
	FV_ASSERT( (m_uiOldestUnackedSeq == SEQ_NULL) ||
			m_kUnackedPackets[ m_uiOldestUnackedSeq ] );
	FV_ASSERT( seq == p->GetSeq() );

	UnackedPacket * pUnackedPacket = new UnackedPacket( p );

	if (m_uiOldestUnackedSeq == SEQ_NULL)
	{
		m_uiOldestUnackedSeq = seq;
	}

	pUnackedPacket->m_uiLastSentAtOutSeq = seq;

	FvUInt64 now = ::Timestamp();
	pUnackedPacket->m_uiLastSentTime = now;
	m_uiLastReliableSendTime = now;

	pUnackedPacket->m_bWasResent = false;
	pUnackedPacket->m_uiNextMissing = SEQ_NULL;

	if (roBeg != roEnd)
	{
		pUnackedPacket->m_kReliableOrders.assign( roBeg, roEnd );
	}

	bool isOverflow = false;

	if (!m_kOverflowPackets.empty() || m_kUnackedPackets[ seq ] != NULL)
	{
		if (this->nub().IsVerbose())
		{
			FV_WARNING_MSG( "FvNetChannel::AddResendTimer( %s ):"
							"Window size exceeded, buffering #%u\n",
						this->c_str(), pUnackedPacket->m_spPacket->GetSeq() );
		}

		FV_ASSERT( seq == SeqMask( m_uiLargeOutSeqAt - 1 ) );

		isOverflow = true;
		this->AddOverflowPacket( pUnackedPacket );
		FV_ASSERT( SeqMask( m_uiSmallOutSeqAt + m_kOverflowPackets.size() ) ==
				m_uiLargeOutSeqAt );
	}
	else
	{
		m_kUnackedPackets[ seq ] = pUnackedPacket;
		m_uiSmallOutSeqAt = m_uiLargeOutSeqAt;
		FV_ASSERT( m_kOverflowPackets.empty() );
	}

	FV_ASSERT( (m_uiOldestUnackedSeq == SEQ_NULL) ||
			m_kUnackedPackets[ m_uiOldestUnackedSeq ] );

	return !isOverflow;
}


bool FvNetChannel::DelResendTimer( FvNetSeqNum seq )
{
	FV_ASSERT( m_uiFirstMissing == SEQ_NULL || m_kUnackedPackets[ m_uiFirstMissing ] );
	FV_ASSERT( (m_uiFirstMissing == SEQ_NULL) == (m_uiLastMissing == SEQ_NULL) );
	FV_ASSERT( (m_uiFirstMissing == SEQ_NULL) ||
			(m_uiFirstMissing == m_uiLastMissing) ||
			(m_kUnackedPackets[ m_uiFirstMissing ]->m_uiNextMissing <= m_uiLastMissing) );
	FV_ASSERT( m_uiFirstMissing <= m_uiLastMissing );
	FV_ASSERT( (m_uiOldestUnackedSeq == SEQ_NULL) ||
			m_kUnackedPackets[ m_uiOldestUnackedSeq ] );

	if (SeqMask( seq ) != seq)
	{
		FV_ERROR_MSG( "FvNetChannel::DelResendTimer( %s ): "
			"Got out-of-range seq #%u (outseq: #%u)\n",
			this->c_str(), seq, m_uiSmallOutSeqAt );

		return false;
	}

	if (SeqMask( (m_uiSmallOutSeqAt-1) - seq ) >= m_uiWindowSize)
	{
		FV_WARNING_MSG( "FvNetChannel::DelResendTimer( %s ): "
			"Called for seq #%u outside window #%u (maybe ok)\n",
			this->c_str(), seq, m_uiSmallOutSeqAt );

		return true;
	}

	UnackedPacket * pUnackedPacket = m_kUnackedPackets[ seq ];
	if (pUnackedPacket == NULL)
	{
		/*
		if (this->isInternal())
		{
			FV_DEBUG_MSG( "Channel::delResendTimer( %s ): "
				"Called for already acked packet #%d (ok)\n",
				this->c_str(), (int)seq );
		}
		*/
		return true;
	}

	if (!pUnackedPacket->m_bWasResent)
	{
		const FvUInt64 RTT_AVERAGE_DENOM = 10;

		m_uiRoundTripTime = ((m_uiRoundTripTime * (RTT_AVERAGE_DENOM - 1)) +
			(Timestamp() - pUnackedPacket->m_uiLastSentTime)) / RTT_AVERAGE_DENOM;
	}

	if (m_uiUnackedCriticalSeq == seq)
	{
		m_uiUnackedCriticalSeq = SEQ_NULL;
	}

	if (seq == m_uiOldestUnackedSeq)
	{
		if (pUnackedPacket->m_uiNextMissing != SEQ_NULL)
		{
			m_uiOldestUnackedSeq = pUnackedPacket->m_uiNextMissing;
			FV_ASSERT( (m_uiOldestUnackedSeq == SEQ_NULL) ||
					(m_kUnackedPackets[ m_uiOldestUnackedSeq ] != NULL) );
		}

		else
		{
			m_uiOldestUnackedSeq = SEQ_NULL;
			for (unsigned int i = SeqMask( seq+1 );
					i != m_uiSmallOutSeqAt;
					i = SeqMask( i+1 ))
			{
				if (m_kUnackedPackets[ i ])
				{
					m_uiOldestUnackedSeq = i;
					break;
				}
			}
		}
	}

	if (SeqLessThan( m_uiLastAck, seq ))
	{
		m_uiLastAck = seq;
	}

	if (m_uiLastMissing != SEQ_NULL &&
		SeqMask( m_uiLastMissing - seq ) < m_uiWindowSize)
	{
		FvNetSeqNum preLook = SEQ_NULL;
		UnackedPacket * pPreLookRR = NULL;

		FvNetSeqNum look = m_uiFirstMissing;
		UnackedPacket * pLookRR = m_kUnackedPackets[look];
		while (look != SEQ_NULL && look != seq)
		{
			preLook = look;
			pPreLookRR = pLookRR;

			look = pLookRR->m_uiNextMissing;
			pLookRR = m_kUnackedPackets[look];
		}

		if (pPreLookRR == NULL)
			m_uiFirstMissing = pLookRR->m_uiNextMissing;
		else
			pPreLookRR->m_uiNextMissing = pLookRR->m_uiNextMissing;

		if (seq == m_uiLastMissing)
			m_uiLastMissing = preLook;

		if (this->IsInternal())
		{
			FV_DEBUG_MSG( "FvNetChannel::DelResendTimer( %s ): "
					"Got ack for missing packet #%d inside window #%d\n",
				this->c_str(), (int)seq, (int)m_uiSmallOutSeqAt );
		}
	}
	else if (seq != SeqMask( m_uiSmallOutSeqAt - m_uiWindowSize ))
	{
		FvNetSeqNum nextNewMissing = SEQ_NULL;
		FvNetSeqNum oldLastMissing = m_uiLastMissing;
		const FvUInt32 windowMask = m_uiWindowSize - 1;

		for (FvNetSeqNum look = SeqMask( seq-1 );
			(look & windowMask) != ((m_uiSmallOutSeqAt-1) & windowMask);
			look = SeqMask( look-1 ))
		{
			UnackedPacket * pLookRR = m_kUnackedPackets[look];

			if (pLookRR == NULL)
				break;

			pLookRR->m_uiNextMissing = nextNewMissing;

			if (nextNewMissing == SEQ_NULL)
				m_uiLastMissing = look;

			nextNewMissing = look;
		}

		if (nextNewMissing != SEQ_NULL)
		{
			if (this->IsInternal())
			{
				FV_DEBUG_MSG( "FvNetChannel::DelResendTimer( %s ): "
					"Ack for #%d inside window #%d created missing packets "
					"back to #%d\n",
					this->c_str(), (int)seq,
					(int)m_uiSmallOutSeqAt, (int)nextNewMissing );
			}

			if (m_uiFirstMissing == SEQ_NULL)
			{
				m_uiFirstMissing = nextNewMissing;
			}
			else
			{
				m_kUnackedPackets[ oldLastMissing ]->m_uiNextMissing =
					nextNewMissing;
			}
		}
	}
	else
	{
		FV_ASSERT( m_uiFirstMissing == SEQ_NULL ||
			m_kUnackedPackets[ m_uiFirstMissing ] );
	}

	delete pUnackedPacket;
	m_kUnackedPackets[ seq ] = NULL;

	FV_ASSERT( m_uiFirstMissing == SEQ_NULL || m_kUnackedPackets[ m_uiFirstMissing ] );
	FV_ASSERT( (m_uiFirstMissing == SEQ_NULL) == (m_uiLastMissing == SEQ_NULL) );
	FV_ASSERT( (m_uiFirstMissing == SEQ_NULL) ||
			(m_uiFirstMissing == m_uiLastMissing) ||
			(m_kUnackedPackets[ m_uiFirstMissing ]->m_uiNextMissing <= m_uiLastMissing) );
	FV_ASSERT( m_uiFirstMissing <= m_uiLastMissing );
	FV_ASSERT( m_uiOldestUnackedSeq == SEQ_NULL ||
			m_kUnackedPackets[ m_uiOldestUnackedSeq ] );

	while (!m_kOverflowPackets.empty() &&
			(m_kUnackedPackets[ m_uiSmallOutSeqAt ] == NULL))
	{
		FvNetSeqNum currSeqNum = m_kOverflowPackets.front()->seq();
		FV_ASSERT( currSeqNum == m_uiSmallOutSeqAt );

		m_kUnackedPackets[ m_uiSmallOutSeqAt ] = m_kOverflowPackets.front();

		m_uiSmallOutSeqAt = SeqMask( m_uiSmallOutSeqAt + 1 );

		m_kOverflowPackets.pop_front();
		if (m_uiOldestUnackedSeq == SEQ_NULL)
		{
			m_uiOldestUnackedSeq = currSeqNum;
		}
		this->SendUnacked( *m_kUnackedPackets[ currSeqNum ] );

	}

	return true;
}


void FvNetChannel::CheckResendTimers()
{
	if (m_bHasRemoteFailed)
	{
		FV_WARNING_MSG( "FvNetChannel::CheckResendTimers( %s ): "
			"Not doing anything due to remote process failure\n",
			this->c_str() );

		return;
	}

	bool resentMissing = false;
	FvNetSeqNum seq = m_uiFirstMissing;

	while (seq != SEQ_NULL)
	{
		UnackedPacket & missing = *m_kUnackedPackets[ seq ];

		FvNetSeqNum nextMissing = missing.m_uiNextMissing;

		if (SeqLessThan( missing.m_uiLastSentAtOutSeq, m_uiLastAck ))
		{
			this->Resend( seq );

			resentMissing = true;
		}

		seq = nextMissing;
	}

	if (resentMissing)
	{
		return;
	}

	if (m_uiOldestUnackedSeq != SEQ_NULL)
	{
		FvUInt64 now = Timestamp();
		FvUInt64 thresh = std::max( m_uiRoundTripTime*2, m_uiMinInactivityResendDelay );
		FvUInt64 lastReliableSendTime = this->LastReliableSendOrResendTime();

		FvNetSeqNum seq = m_uiOldestUnackedSeq;

		while (SeqLessThan( seq, m_uiSmallOutSeqAt ) && m_kUnackedPackets[ seq ])
		{
			UnackedPacket & unacked = *m_kUnackedPackets[ seq ];

			if (now - unacked.m_uiLastSentTime > thresh)
			{
				if (this->nub().IsVerbose())
				{
					FV_WARNING_MSG( "Channel::checkResendTimers( %s ): "
						"Resending unacked packet #%u due to inactivity "
						"(Packet %.3fs, Channel %.3fs, RTT %.3fs)\n",
						this->c_str(),
						unacked.m_spPacket->GetSeq(),
						(now - unacked.m_uiLastSentTime) / StampsPerSecondD(),
						(now - lastReliableSendTime) / StampsPerSecondD(),
						m_uiRoundTripTime / StampsPerSecondD() );
				}

				this->Resend( seq );
			}

			seq = SeqMask( seq + 1 );
		}
	}
}


void FvNetChannel::Resend( FvNetSeqNum seq )
{
	++m_uiNumPacketsResent;

	UnackedPacket & unacked = *m_kUnackedPackets[ seq ];

	if (this->IsExternal() &&
		!unacked.m_spPacket->HasFlags( FvNetPacket::FLAG_IS_FRAGMENT ) &&
		(m_kUnackedPackets[ m_uiSmallOutSeqAt ] == NULL)) // Not going to overflow
	{
		if (this->Bundle().piggyback(
				seq, unacked.m_kReliableOrders, unacked.m_spPacket.GetObject() ))
		{
			this->DelResendTimer( seq );
			return;
		}
	}

	if (this->IsInternal())
	{
		/*FV_ERROR_MSG( "Channel::resend( %s ): Resending #%d (outSeqAt #%d)\n",
		  this->c_str(), seq, smallOutSeqAt_ );*/
	}

	this->SendUnacked( unacked );
}


void FvNetChannel::SendUnacked( UnackedPacket & unacked )
{
	m_pkNub->SendPacket( m_kAddr, unacked.m_spPacket.Get(), this, true );

	unacked.m_uiLastSentAtOutSeq = m_uiSmallOutSeqAt;
	unacked.m_bWasResent = true;

	FvUInt64 now = Timestamp();
	unacked.m_uiLastSentTime = now;
	m_uiLastReliableResendTime = now;
}


std::pair< FvNetPacket*, bool > FvNetChannel::QueueAckForPacket( FvNetPacket *p, FvNetSeqNum seq,
	const FvNetAddress & srcAddr )
{
	if (SeqMask( seq ) != seq)
	{
		FV_ERROR_MSG( "FvNetChannel::QueueAckForPacket( %s ): "
			"Got out-of-range incoming seq #%u (inSeqAt: #%u)\n",
			this->c_str(), seq, m_uiInSeqAt );

		return std::make_pair( (FvNetPacket*)NULL, false );
	}

	if (m_kAddr != srcAddr)
	{
		if (m_bShouldAutoSwitchToSrcAddr)
		{
			if (SeqLessThan( p->GetChannelVersion(), m_uiVersion ))
			{
				FV_WARNING_MSG( "FvNetChannel::QueueAckForPacket( %s ): "
					"Dropping packet from old addr %s (v%u < v%u)\n",
					this->c_str(), srcAddr.c_str(),
					p->GetChannelVersion(), m_uiVersion );

				return std::make_pair( (FvNetPacket*)NULL, true );
			}

			else
			{
				m_uiVersion = p->GetChannelVersion();
				this->addr( srcAddr );
			}
		}
		else
		{
			FV_ERROR_MSG( "FvNetChannel::QueueAckForPacket( %s ): "
				"Got packet #%u from wrong source address: %s\n",
				this->c_str(), seq, srcAddr.c_str() );

			return std::make_pair( (FvNetPacket*)NULL, false );
		}
	}

	else if (m_bShouldAutoSwitchToSrcAddr &&
		SeqLessThan( m_uiVersion, p->GetChannelVersion() ))
	{
		FV_WARNING_MSG( "FvNetChannel::QueueAckForPacket( %s ): "
			"Updating to v%d->%d without changing address\n",
			this->c_str(), m_uiVersion, p->GetChannelVersion() );

		m_uiVersion = p->GetChannelVersion();
	}

	int acksOnPacket = m_pkBundle->AddAck( seq );

	//! add by Uman, 20100305, 问题记录:及时回ack的问题
	//! 当channel的窗口溢出时,发送ack的packet会加入重发队列(溢出队列)而不发送
	//! 导致不能及时回ack,发送端会反复重发未回ack的包,引起发送端溢出,恶性循环
	//! 会重复出现Discarding already-seen packet消息和Resending unacked packet消息
	//! 当channel通信采用regular模式时,会出现该问题,如:A和B通过regular的channel通信
	//! 当B卡住一会重新处理时,先处理定时器,如果定时器里有很多发包,可能导致B的发送窗口溢出
	//! 然后处理收包,收到包的回包都放到溢出队列里而不能及时发送
	//! A在B卡住的时间里也不断发包,因为B卡住不能收包也不能回ack,所以A的发送窗口也比较满或溢出
	//! 在B缓过来后,A收到B发送的包(定时器中发的包)回的ack都放到溢出队列也不能及时回ack
	//! A和B都收不到ack,发送窗口不能前移,导致双方都不断的resend和discard包,该channel就不能通信了
	//! 应改为当窗口溢出时,通过发送非可靠的包(不用加入溢出队列,可以直接发送)及时回ack
	//! 然后需要控制regular channel的发包频率,应该定时发包,而不能调用send即时发送,减少定时器对网络传输平稳性的影响

	if (m_iPushUnsentAcksThreshold &&
			(acksOnPacket >= m_iPushUnsentAcksThreshold))
	{
		if (this->nub().IsVerbose())
		{
			FV_DEBUG_MSG( "FvNetChannel::QueueAckForPacket( %s ): "
					"Pushing %d unsent ACKs due to inactivity\n",
				this->c_str(), acksOnPacket );
		}

		this->Send();
	}

	if (seq == m_uiInSeqAt)
	{
		m_uiInSeqAt = SeqMask( m_uiInSeqAt+1 );

		FvNetPacket * pPrev = p;
		FvNetPacket * pBufferedPacket = m_kBufferedReceives[ m_uiInSeqAt ].GetObject();

		while (pBufferedPacket != NULL)
		{
			pPrev->Chain( pBufferedPacket );
			m_kBufferedReceives[ m_uiInSeqAt ] = NULL;
			--m_uiNumBufferedReceives;

			pPrev = pBufferedPacket;
			m_uiInSeqAt = SeqMask( m_uiInSeqAt+1 );
			pBufferedPacket = m_kBufferedReceives[ m_uiInSeqAt ].GetObject();
		}

		return std::make_pair( p, true );
	}

	if (SeqLessThan( seq, m_uiInSeqAt ))
	{
		if (this->nub().IsVerbose())
		{
			FV_DEBUG_MSG( "FvNetChannel::QueueAckForPacket( %s ): "
					"Discarding already-seen packet #%d below inSeqAt #%d\n",
				this->c_str(), (int)seq, (int)m_uiInSeqAt );
		}

		this->nub().IncNumDuplicatePacketsReceived();

		return std::make_pair( (FvNetPacket*)NULL, true );
	}

	if (SeqMask(seq - m_uiInSeqAt) > m_uiWindowSize)
	{
		FV_WARNING_MSG( "FvNetChannel::QueueAckForPacket( %s ): "
				"Sequence number #%d is way out of window #%d!\n",
			this->c_str(), (int)seq, (int)m_uiInSeqAt );

		return std::make_pair( (FvNetPacket*)NULL, true );
	}

	FvNetPacketPtr & rpBufferedPacket = m_kBufferedReceives[ seq ];

	if (rpBufferedPacket != NULL)
	{
		FV_DEBUG_MSG( "FvNetChannel::QueueAckForPacket( %s ): "
			"Discarding already-buffered packet #%d\n",
			this->c_str(), (int)seq );
	}
	else
	{
		rpBufferedPacket = p;
		++m_uiNumBufferedReceives;

		FV_DEBUG_MSG( "FvNetChannel::QueueAckForPacket( %s ): "
			"Buffering packet #%d above #%d\n",
			this->c_str(), (int)seq, (int)m_uiInSeqAt );
	}

	return std::make_pair( (FvNetPacket*)NULL, true );
}


void FvNetChannel::IsAnonymous( bool anonymous )
{
	m_bIsAnonymous = anonymous;

	if (m_bIsAnonymous)
	{
		m_pkNub->KeepAliveChannels().AddIfNecessary( *this );
	}
	else
	{
		m_pkNub->KeepAliveChannels().DelIfNecessary( *this );
	}

	this->IsIrregular( m_bIsAnonymous );
}


void FvNetChannel::ResendCriticals()
{
	if (m_uiUnackedCriticalSeq == SEQ_NULL)
	{
		FV_WARNING_MSG( "Channel::resendCriticals( %s ): "
			"Called with no unacked criticals!\n",
			this->c_str() );

		return;
	}

	for (FvNetSeqNum seq = m_uiOldestUnackedSeq;
		 seq != SeqMask( m_uiUnackedCriticalSeq + 1 );
		 seq = SeqMask( seq + 1 ))
	{
		if (m_kUnackedPackets[ seq ])
		{
			this->Resend( seq );
		}
	}
}


bool FvNetChannel::ShouldSendFirstReliablePacket() const
{
	return this->IsInternal() &&
		(m_uiNumReliablePacketsSent == 0) &&
		(m_uiSmallOutSeqAt == 0);
}


void FvNetChannel::ShouldAutoSwitchToSrcAddr( bool b )
{
	m_bShouldAutoSwitchToSrcAddr = b;
	FV_ASSERT( !m_bShouldAutoSwitchToSrcAddr || this->IsIndexed() );
}


const char * FvNetChannel::c_str() const
{
	static char dodgyString[ 40 ];

	int length = m_kAddr.WriteToString( dodgyString, sizeof( dodgyString ) );

	if (this->IsIndexed())
	{
		length += FvSNPrintf( dodgyString + length, sizeof( dodgyString ) - length,
				"/%d", m_iID );
	}

	if (m_bIsCondemned)
	{
		length += FvSNPrintf( dodgyString + length, sizeof( dodgyString ) - length,
				"!" );
	}

	return dodgyString;
}


void FvNetChannel::ClearBundle()
{
	if (!m_pkBundle)
	{
		m_pkBundle = new FvNetBundle( m_spFilter ? m_spFilter->MaxSpareSize() : 0, this );
	}
	else
	{
		m_pkBundle->Clear();
	}

	if (this->IsIndexed())
	{
		m_pkBundle->m_spFirstPacket->EnableFlags( FvNetPacket::FLAG_INDEXED_CHANNEL );
	}

	if (this->ShouldSendFirstReliablePacket())
	{
		m_pkBundle->m_spFirstPacket->EnableFlags( FvNetPacket::FLAG_CREATE_CHANNEL );
	}

	if (m_pkBundlePrimer)
	{
		m_pkBundlePrimer->PrimeBundle( *m_pkBundle );
	}
}


void FvNetChannel::BundlePrimer( FvNetBundlePrimer & primer )
{
	m_pkBundlePrimer = &primer;

	if (m_pkBundle->NumMessages() == 0)
	{
		primer.PrimeBundle( *m_pkBundle );
	}
}


int FvNetChannel::HandleTimeout( FvNetTimerID, void * arg )
{
	switch (reinterpret_cast<FvUIntPtr>( arg ))
	{
		case TIMEOUT_INACTIVITY_CHECK:
		{
			if (Timestamp() - m_uiLastReceivedTime > m_uiInactivityExceptionPeriod)
			{
				throw NubExceptionWithAddress( FV_NET_REASON_INACTIVITY, m_kAddr );
			}
			break;
		}

		case TIMEOUT_CHANNEL_PUSH:
		{
			if (this->IsEstablished())
			{
				this->Send();
			}
			break;
		}
	}

	return 0;
}


void FvNetChannel::Reset( const FvNetAddress & newAddr, bool warnOnDiscard )
{
	if (newAddr == m_kAddr)
	{
		return;
	}

	if (this->HasUnackedPackets())
	{
		int numUnacked = 0;

		for (unsigned int i = 0; i < m_kUnackedPackets.size(); i++)
		{
			if (m_kUnackedPackets[i])
			{
				++numUnacked;
				delete m_kUnackedPackets[i];
				m_kUnackedPackets[i] = NULL;
			}
		}

		while (!m_kOverflowPackets.empty())
		{
			++numUnacked;
			delete m_kOverflowPackets.front();
			m_kOverflowPackets.pop_front();
		}

		if (warnOnDiscard && numUnacked > 0)
		{
			FV_WARNING_MSG( "FvNetChannel::Reset( %s ): "
				"Forgetting %d unacked packet(s)\n",
				this->c_str(), numUnacked );
		}
	}

	if (m_uiNumBufferedReceives > 0)
	{
		if (warnOnDiscard)
		{
			FV_WARNING_MSG( "FvNetChannel::Reset( %s ): "
				"Discarding %u buffered packet(s)\n",
				this->c_str(), m_uiNumBufferedReceives );
		}

		for (unsigned int i=0;
			 i < m_kBufferedReceives.size() && m_uiNumBufferedReceives > 0;
			 i++)
		{
			if (m_kBufferedReceives[i] != NULL)
			{
				m_kBufferedReceives[i] = NULL;
				--m_uiNumBufferedReceives;
			}
		}
	}

	if (m_spFragments)
	{
		if (warnOnDiscard)
		{
			FV_WARNING_MSG( "FvNetChannel::Reset( %s ): "
				"Forgetting %d unprocessed packets in the fragment chain\n",
				this->c_str(), m_spFragments->m_spChain->ChainLength() );
		}

		m_spFragments = NULL;
	}

	m_uiInSeqAt = 0;
	m_uiSmallOutSeqAt = 0;
	m_uiLargeOutSeqAt = 0;
	m_uiLastReceivedTime = ::Timestamp();
	m_uiLastAck = SeqMask( m_uiSmallOutSeqAt - 1 );
	m_uiFirstMissing = SEQ_NULL;
	m_uiLastMissing = SEQ_NULL;
	m_uiOldestUnackedSeq = SEQ_NULL;
	m_uiLastReliableSendTime = 0;
	m_uiLastReliableResendTime = 0;
	m_uiRoundTripTime =
		this->IsInternal() ? StampsPerSecond() / 10 : StampsPerSecond();
	m_bHasRemoteFailed = false;
	m_uiUnackedCriticalSeq = SEQ_NULL;
	m_bWantsFirstPacket = false;
	m_bShouldDropNextSend = false;
	m_uiNumPacketsSent = 0;
	m_uiNumPacketsReceived = 0;
	m_uiNumBytesSent = 0;
	m_uiNumBytesReceived = 0;
	m_uiNumPacketsResent = 0;
	m_uiNumReliablePacketsSent = 0;

	if (this->IsIndexed())
	{
		m_uiVersion = SeqMask( m_uiVersion + 1 );
	}

	this->ClearBundle();

	m_pkNub->IrregularChannels().DelIfNecessary( *this );
	m_pkNub->KeepAliveChannels().DelIfNecessary( *this );

	if (m_kInactivityTimerID != FV_NET_TIMER_ID_NONE)
	{
		m_pkNub->CancelTimer( m_kInactivityTimerID );
		m_kInactivityTimerID = FV_NET_TIMER_ID_NONE;
	}

	if (this->IsEstablished())
	{
		m_bWantsFirstPacket = true;
	}

	this->addr( newAddr );

	m_pkNub->CancelRequestsFor( this );

	if (this->IsEstablished() && m_pkBundlePrimer)
	{
		this->BundlePrimer( *m_pkBundlePrimer );
	}
}


void FvNetChannel::ConfigureFrom( const FvNetChannel & other )
{
	this->IsIrregular( other.IsIrregular() );
	this->ShouldAutoSwitchToSrcAddr( other.ShouldAutoSwitchToSrcAddr() );
	this->PushUnsentAcksThreshold( other.PushUnsentAcksThreshold() );

	FV_ASSERT( m_eTraits == other.m_eTraits );
	FV_ASSERT( m_uiMinInactivityResendDelay == other.m_uiMinInactivityResendDelay );
}


void FvNetChannel::SwitchNub( FvNetNub * pDestNub )
{
	m_pkNub->IrregularChannels().DelIfNecessary( *this );
	m_pkNub->KeepAliveChannels().DelIfNecessary( *this );
	m_pkNub->DeregisterChannel( *this );

	m_pkNub = pDestNub;

	m_pkNub->RegisterChannel( *this );
	m_kIrregularIter = m_pkNub->IrregularChannels().End();
	m_pkNub->IrregularChannels().AddIfNecessary( *this );
	m_kKeepAliveIter = m_pkNub->KeepAliveChannels().End();
	m_pkNub->KeepAliveChannels().AddIfNecessary( *this );
}


void FvNetChannel::StartInactivityDetection( float period, float checkPeriod )
{
	if (m_kInactivityTimerID != FV_NET_TIMER_ID_NONE)
	{
		m_pkNub->CancelTimer( m_kInactivityTimerID );
	}

	m_uiInactivityExceptionPeriod = FvUInt64( period * StampsPerSecond() );
	m_uiLastReceivedTime = Timestamp();

	m_kInactivityTimerID = m_pkNub->RegisterTimer( int( checkPeriod * 1000000 ),
									this, (void *)TIMEOUT_INACTIVITY_CHECK );
}


bool FvNetChannel::HasPacketBeenAcked( FvNetSeqNum seq ) const
{
	if (SeqMask( (m_uiSmallOutSeqAt-1) - seq ) >= m_uiWindowSize)
	{
		return true;
	}
	else
	{
		return m_kUnackedPackets[ seq ] == NULL;
	}
}


int FvNetChannel::LatestAckedPacketAge() const
{
	for (FvUInt32 look = 0; look < m_uiWindowSize; look++)
	{
		FvNetSeqNum seq = SeqMask((m_uiSmallOutSeqAt-1) - look);

		if (m_kUnackedPackets[ seq ] == NULL)
		{
			return (int)look;
		}
	}

	return (int)m_uiWindowSize + m_kOverflowPackets.size();
}


void FvNetChannel::IsIrregular( bool isIrregular )
{
	m_bIsIrregular = isIrregular;

	m_pkNub->IrregularChannels().AddIfNecessary( *this );
}


FvNetSeqNum FvNetChannel::UseNextSequenceID()
{
	FvNetSeqNum	retSeq = m_uiLargeOutSeqAt;
	m_uiLargeOutSeqAt = SeqMask( m_uiLargeOutSeqAt + 1 );

	if (this->IsInternal())
	{
		int usage = this->SendWindowUsage();
		int & threshold = this->SendWindowWarnThreshold();

		if (usage > threshold)
		{
			FV_WARNING_MSG( "FvNetChannel::UseNextSequenceID( %s ): "
							"Send window backlog is now %d packets, "
							"exceeded previous max of %d, "
							"critical size is %u\n",
						this->c_str(), usage, threshold, m_uiWindowSize );

			threshold = usage;
		}

		if (this->IsIndexed() &&
				(ms_pfSendWindowCallback != NULL) &&
				(usage > ms_iSendWindowCallbackThreshold))
		{
			(*ms_pfSendWindowCallback)( *this );
		}
	}

	return retSeq;
}


bool FvNetChannel::validateUnreliableSeqNum( const FvNetSeqNum seqNum )
{
	if (seqNum != SeqMask( seqNum ))
	{
		FV_WARNING_MSG( "FvNetChannel:ValidateUnreliableSeqNum: "
			"Invalid sequence number (%d).\n", seqNum );
		return false;
	}

	if (FvNetChannel::SeqLessThan( seqNum, m_uiUnreliableInSeqAt ) &&
			(m_uiUnreliableInSeqAt != SEQ_NULL))
	{
		FV_WARNING_MSG( "FvNetChannel:ValidateUnreliableSeqNum: Received an invalid "
			"seqNum (%d) on an unreliable channel. Last valid seqNum (%d)\n",
			seqNum, m_uiUnreliableInSeqAt );
		return false;
	}

	m_uiUnreliableInSeqAt = seqNum;
	return true;
}


void FvNetChannel::HasRemoteFailed( bool v )
{
	m_bHasRemoteFailed = v;

	if (m_bIsAnonymous)
	{
		FV_INFO_MSG( "Cleaning up dead anonymous channel to %s\n",
			this->c_str() );

		m_pkNub->DelAnonymousChannel( m_kAddr );
	}
}


void FvNetChannel::OnPacketReceived( int bytes )
{
	m_uiLastReceivedTime = Timestamp();
	++m_uiNumPacketsReceived;
	m_uiNumBytesReceived += bytes;
}


#if FV_ENABLE_WATCHERS
FvWatcherPtr FvNetChannel::GetWatcher()
{
	static FvDirectoryWatcherPtr pWatcher = NULL;

	if (pWatcher == NULL)
	{
		pWatcher = new FvDirectoryWatcher();

		FvNetChannel * pNull = NULL;

#define ADD_WATCHER( PATH, MEMBER )		\
		pWatcher->AddChild( #PATH, MakeWatcher( pNull->MEMBER ) );

		ADD_WATCHER( addr,				m_kAddr );
		ADD_WATCHER( packetsSent,		m_uiNumPacketsSent );
		ADD_WATCHER( packetsReceived,	m_uiNumPacketsReceived );
		ADD_WATCHER( bytesSent,			m_uiNumBytesSent );
		ADD_WATCHER( bytesReceived,		m_uiNumBytesReceived );
		ADD_WATCHER( packetsResent,		m_uiNumPacketsResent );
		ADD_WATCHER( reliablePacketsResent,		m_uiNumReliablePacketsSent );

		ADD_WATCHER( IsIrregular,		m_bIsIrregular );

		pWatcher->AddChild( "roundTripTime",
				MakeWatcher( *pNull, &FvNetChannel::RoundTripTimeInSeconds ) );
	}

	return pWatcher;
}
#endif // FV_ENABLE_WATCHERS


void FvNetChannel::SetSendWindowCallback( SendWindowCallback callback )
{
	ms_pfSendWindowCallback = callback;
}


void FvNetChannel::SendWindowCallbackThreshold( float threshold )
{
	ms_iSendWindowCallbackThreshold = int( threshold * INDEXED_CHANNEL_SIZE );
}


float FvNetChannel::SendWindowCallbackThreshold()
{
	return float(ms_iSendWindowCallbackThreshold)/INDEXED_CHANNEL_SIZE;
}


FvNetChannel::UnackedPacket::UnackedPacket( FvNetPacket * pPacket ) :
	m_spPacket( pPacket )
{}


FvNetChannel::UnackedPacket * FvNetChannel::UnackedPacket::initFromStream(
	FvBinaryIStream & data, FvUInt64 timeNow )
{
	FvNetPacketPtr pPacket = FvNetPacket::CreateFromStream( data, FvNetPacket::UNACKED_SEND );

	if (pPacket)
	{
		UnackedPacket * pInstance = new UnackedPacket( pPacket.GetObject() );

		data >> pInstance->m_uiLastSentAtOutSeq;

		pInstance->m_uiLastSentTime = timeNow;
		pInstance->m_bWasResent = false;
		pInstance->m_uiNextMissing = SEQ_NULL;

		return pInstance;
	}
	else
	{
		return NULL;
	}
}


void FvNetChannel::UnackedPacket::addToStream(
	UnackedPacket * pInstance, FvBinaryOStream & data )
{
	if (pInstance)
	{
		FvNetPacket::AddToStream( data, pInstance->m_spPacket.GetObject(),
			FvNetPacket::UNACKED_SEND );

		data << pInstance->m_uiLastSentAtOutSeq;
	}
	else
	{
		FvNetPacket::AddToStream( data, (FvNetPacket*)NULL, FvNetPacket::UNACKED_SEND );
	}
}


void FvNetChannelOwner::Addr( const FvNetAddress & addr )
{
	FV_ASSERT( m_pkChannel );

	if (this->Addr() == addr)
	{
		return;
	}

	FvNetNub & nub = m_pkChannel->nub();

	FvNetChannel * pNewChannel = FvNetChannel::get( nub, addr );

	pNewChannel->ConfigureFrom( *m_pkChannel );
	m_pkChannel->Condemn();

	m_pkChannel = pNewChannel;
}

void FvNetChannel::Dump(FvXMLSectionPtr spChannel)
{
	if(!spChannel)
		return;

	FvXMLSectionPtr spChild, spPk;
	FvUInt32 uiSendCount = this->SendWindowUsage();

	spChild = spChannel->NewSection("ID");
	if(spChild) spChild->SetInt(m_iID);
	spChild = spChannel->NewSection("Version");
	if(spChild) spChild->SetInt(m_uiVersion);
	spChild = spChannel->NewSection("bWantsFirstPacket");
	if(spChild) spChild->SetBool(m_bWantsFirstPacket);

	spChild = spChannel->NewSection("SendCount");
	if(spChild) spChild->SetInt(uiSendCount);
	spChild = spChannel->NewSection("SmallOutSeqAt");
	if(spChild) spChild->SetInt(m_uiSmallOutSeqAt);
	spChild = spChannel->NewSection("LargeOutSeqAt");
	if(spChild) spChild->SetInt(m_uiLargeOutSeqAt);
	spChild = spChannel->NewSection("OldestUnackedSeq");
	if(spChild) spChild->SetInt(m_uiOldestUnackedSeq);
	spChild = spChannel->NewSection("FirstMissing");
	if(spChild) spChild->SetInt(m_uiFirstMissing);
	spChild = spChannel->NewSection("LastMissing");
	if(spChild) spChild->SetInt(m_uiLastMissing);
	spChild = spChannel->NewSection("LastAck");
	if(spChild) spChild->SetInt(m_uiLastAck);

	FvXMLSectionPtr spUnackedPks = spChannel->NewSection("UnAckedPks");
	if(spUnackedPks)
	{
		FV_ASSERT( (uiSendCount == 0) || m_kUnackedPackets[ m_uiOldestUnackedSeq ] );

		for (FvUInt32 i = 0; i < std::min( uiSendCount, m_uiWindowSize ); ++i)
		{
			char idStr[16] = {0};
			sprintf_s(idStr, sizeof(idStr), "%d", (m_uiOldestUnackedSeq +i)&(m_kUnackedPackets.size() -1));
			spPk = spUnackedPks->NewSection(idStr);
			if(spPk)
			{
				UnackedPacket* pkUnAck = m_kUnackedPackets[ m_uiOldestUnackedSeq + i ];
				if(pkUnAck)
				{
					spChild = spPk->NewSection("Seq");
					if(spChild) spChild->SetInt(pkUnAck->seq());
					spChild = spPk->NewSection("LastSentAtOutSeq");
					if(spChild) spChild->SetInt(pkUnAck->m_uiLastSentAtOutSeq);
				}
			}
		}
	}

	FvXMLSectionPtr spOverflowPks = spChannel->NewSection("OverflowPks");
	if(spOverflowPks)
	{
		FV_ASSERT( m_kOverflowPackets.empty() ||
			(uiSendCount == m_uiWindowSize + m_kOverflowPackets.size()) );

		for (OverflowPackets::iterator iter = m_kOverflowPackets.begin();
			iter != m_kOverflowPackets.end();
			++iter)
		{
			spPk = spOverflowPks->NewSection("Packet");
			if(spPk)
			{
				UnackedPacket* pkUnAck = *iter;
				FV_ASSERT(pkUnAck);
				spChild = spPk->NewSection("Seq");
				if(spChild) spChild->SetInt(pkUnAck->seq());
				spChild = spPk->NewSection("LastSentAtOutSeq");
				if(spChild) spChild->SetInt(pkUnAck->m_uiLastSentAtOutSeq);
			}
		}
	}

	spChild = spChannel->NewSection("NumBufferedReceives");
	if(spChild) spChild->SetInt(m_uiNumBufferedReceives);
	spChild = spChannel->NewSection("UnackedCriticalSeq");
	if(spChild) spChild->SetInt(m_uiUnackedCriticalSeq);
	spChild = spChannel->NewSection("InSeqAt");
	if(spChild) spChild->SetInt(m_uiInSeqAt);

	FvXMLSectionPtr spBufferedPks = spChannel->NewSection("BufferedPks");
	if(spBufferedPks)
	{
		int numToSend = m_uiNumBufferedReceives;
		for (FvUInt32 i = 1; i < m_uiWindowSize && numToSend > 0; ++i)
		{
			const FvNetPacket* pkPk = m_kBufferedReceives[ m_uiInSeqAt + i ].GetObject();

			char idStr[16] = {0};
			sprintf_s(idStr, sizeof(idStr), "%d", (m_uiInSeqAt +i)&(m_kBufferedReceives.size() -1));
			spPk = spBufferedPks->NewSection(idStr);
			if(spPk)
			{
				if(pkPk)
				{
					spChild = spPk->NewSection("Seq");
					if(spChild) spChild->SetInt(pkPk->GetSeq());
				}
			}

			if (pkPk)
			{
				--numToSend;
			}
		}
	}

	FvXMLSectionPtr spFragments = spChannel->NewSection("Fragments");
	if(spFragments)
	{
		if (m_spFragments)
		{
			spChild = spFragments->NewSection("ChainLen");
			if(spChild) spChild->SetInt(m_spFragments->m_spChain->ChainLength());

			for (const FvNetPacket * p = m_spFragments->m_spChain.GetObject();
				p != NULL; p = p->Next())
			{
				spPk = spFragments->NewSection("Packet");
				if(spPk)
				{
					FV_ASSERT(p);
					spChild = spPk->NewSection("Seq");
					if(spChild) spChild->SetInt(p->GetSeq());
					spChild = spPk->NewSection("FragBegin");
					if(spChild) spChild->SetInt(p->FragBegin());
					spChild = spPk->NewSection("FragEnd");
					if(spChild) spChild->SetInt(p->FragEnd());
					spChild = spPk->NewSection("FirstRequestOffset");
					if(spChild) spChild->SetInt(p->FirstRequestOffset());
				}
			}
		}
	}
}