//{future header message}
#ifndef __FvNetChannel_H__
#define __FvNetChannel_H__

#include "FvNetwork.h"
#include "FvNetBundle.h"
#include "FvNetNub.h"
#include "FvNetPacketFilter.h"
#include "FvNetMisc.h"
#include "FvNetMessageFilter.h"

#include <FvSmartPointer.h>

#include <list>

class FvNetChannel;
class FvNetPacketFilter;
class FvNetReliableOrder;

class FvXMLSection;
typedef FvSmartPointer<FvXMLSection> FvXMLSectionPtr;

template <class T> class FvNetCircularArray
{
public:
	typedef FvNetCircularArray<T> OurType;

	FvNetCircularArray( unsigned int size ) : m_pkData( new T[size] ), m_uiMask( size-1 ) { }
	~FvNetCircularArray()	{ delete [] m_pkData; }

	unsigned int size() const	{ return m_uiMask+1; }

	const T & operator[]( unsigned int n ) const	{ return m_pkData[n&m_uiMask]; }
	T & operator[]( unsigned int n )				{ return m_pkData[n&m_uiMask]; }

private:
	FvNetCircularArray( const OurType & other );
	OurType & operator=( const OurType & other );

	T * m_pkData;
	unsigned int m_uiMask;
};

class FvNetChannelFinder
{
public:
	virtual ~FvNetChannelFinder() {};

	virtual FvNetChannel* Find( FvNetChannelID id, const FvNetPacket * pPacket,
		bool & rHasBeenHandled ) = 0;
};

class FV_NETWORK_API FvNetChannel : public FvNetTimerExpiryHandler, public FvReferenceCount
{
public:

	enum Traits
	{
		INTERNAL = 0,

		EXTERNAL = 1,
	};

	typedef void (*SendWindowCallback)( const FvNetChannel & );

	FvNetChannel( FvNetNub & nub, const FvNetAddress & address, Traits traits,
		float minInactivityResendDelay = 1.0,
		FvNetPacketFilterPtr pFilter = NULL, FvNetChannelID id = FV_NET_CHANNEL_ID_NULL );

	static FvNetChannel *get( FvNetNub & nub, const FvNetAddress & address );

private:
	virtual ~FvNetChannel();

public:
	static void StaticInit();

	void Condemn();
	bool IsCondemned() const { return m_bIsCondemned; }

	void Destroy();
	bool IsDestroyed() const { return m_bIsDestroyed; }

	bool IsDead() const
	{
		return this->IsCondemned() || this->IsDestroyed();
	}

	void InitFromStream( FvBinaryIStream & data, const FvNetAddress & addr );
	void AddToStream( FvBinaryOStream & data );

	FvNetNub &nub()				{ return *m_pkNub; }

	FV_INLINE const FvNetAddress & addr() const;
	void addr( const FvNetAddress & addr );

	FvNetBundle & Bundle();
	const FvNetBundle & Bundle() const;
	bool HasUnsentData() const;

	void Send( FvNetBundle * pBundle = NULL );
	void DelayedSend();

	void SendIfIdle();

	void SendEvery( int microseconds );

	void Reset( const FvNetAddress & newAddr, bool warnOnDiscard = true );

	void ConfigureFrom( const FvNetChannel & other );

	void SwitchNub( FvNetNub * pDestNub );

	void StartInactivityDetection( float inactivityPeriod,
			float checkPeriod = 1.f );

	FvUInt64 LastReceivedTime() const		{ return m_uiLastReceivedTime; }

	int WindowSize() const;
	// sending:
	bool HasPacketBeenAcked( FvNetSeqNum seq ) const;
	// TODO: Remove this
	int EarliestUnackedPacketAge() const	{ return this->SendWindowUsage(); }
	int LatestAckedPacketAge() const;

	FvNetPacketFilterPtr pFilter() const { return m_spFilter; }
	void pFilter(FvNetPacketFilterPtr pFilter) { m_spFilter = pFilter; }

	bool IsIrregular() const	{ return m_bIsIrregular; }
	void IsIrregular( bool isIrregular );

	bool HasRemoteFailed() const { return m_bHasRemoteFailed; }
	void HasRemoteFailed( bool v );

	bool AddResendTimer( FvNetSeqNum seq, FvNetPacket * p,
		const FvNetReliableOrder * roBeg, const FvNetReliableOrder * roEnd );
	bool DelResendTimer( FvNetSeqNum seq );
	void CheckResendTimers();
	void Resend( FvNetSeqNum seq );
	FvUInt64 RoundTripTime() const { return m_uiRoundTripTime; }
	double RoundTripTimeInSeconds() const
		{ return m_uiRoundTripTime/::StampsPerSecondD(); }

	std::pair< FvNetPacket*, bool > QueueAckForPacket(
		FvNetPacket * p, FvNetSeqNum seq, const FvNetAddress & srcAddr );

	bool IsAnonymous() const { return m_bIsAnonymous; }
	void IsAnonymous( bool v );

	bool IsOwnedByNub() const { return m_bIsAnonymous || m_bIsCondemned; }

	bool HasUnackedCriticals() const { return m_uiUnackedCriticalSeq != SEQ_NULL; }
	void ResendCriticals();

	bool WantsFirstPacket() const { return m_bWantsFirstPacket; }
	void GotFirstPacket() { m_bWantsFirstPacket = false; }
	bool ShouldSendFirstReliablePacket() const;

	void DropNextSend() { m_bShouldDropNextSend = true; }

	Traits GetTraits() const { return m_eTraits; }
	bool IsExternal() const { return m_eTraits == EXTERNAL; }
	bool IsInternal() const { return m_eTraits == INTERNAL; }

	bool ShouldAutoSwitchToSrcAddr() const { return m_bShouldAutoSwitchToSrcAddr; }
	void ShouldAutoSwitchToSrcAddr( bool b );

	FvNetSeqNum UseNextSequenceID();
	void OnPacketReceived( int bytes );

	const char * c_str() const;

	FvNetChannelID id() const	{ return m_iID; }

	FvNetChannelVersion version() const { return m_uiVersion; }
	void version( FvNetChannelVersion v ) { m_uiVersion = v; }

	bool IsIndexed() const	{ return m_iID != FV_NET_CHANNEL_ID_NULL; }
	bool IsEstablished() const { return m_kAddr.m_uiIP != 0; }

	void ClearBundle();
	void BundlePrimer( FvNetBundlePrimer & primer );

	FvNetNub::FragmentedBundlePtr pFragments() { return m_spFragments; }
	void pFragments( FvNetNub::FragmentedBundlePtr pFragments ) { m_spFragments = pFragments; }

	static FvNetSeqNum SeqMask( FvNetSeqNum x );
	static bool SeqLessThan( FvNetSeqNum a, FvNetSeqNum b );

	static const FvNetSeqNum SEQ_SIZE = 0x10000000U;
	static const FvNetSeqNum SEQ_MASK = SEQ_SIZE-1;
	static const FvNetSeqNum SEQ_NULL = SEQ_SIZE;

	static FvWatcherPtr GetWatcher();

	bool HasUnackedPackets() const	{ return m_uiOldestUnackedSeq != SEQ_NULL; }

	int SendWindowUsage() const
	{
		return this->HasUnackedPackets() ?
			SeqMask( m_uiLargeOutSeqAt - m_uiOldestUnackedSeq ) : 0;
	}

	static void SetSendWindowCallback( SendWindowCallback callback );
	static float SendWindowCallbackThreshold();
	static void SendWindowCallbackThreshold( float threshold );

	int PushUnsentAcksThreshold() const { return m_iPushUnsentAcksThreshold; }
	void PushUnsentAcksThreshold( int i ) { m_iPushUnsentAcksThreshold = i; }

	FvUInt32 NumPacketsSent() const		{ return m_uiNumPacketsSent; }

	FvUInt32 NumPacketsReceived() const	{ return m_uiNumPacketsReceived; }

	FvUInt32 NumBytesSent() const		{ return m_uiNumBytesSent; }

	FvUInt32 NumBytesReceived() const	{ return m_uiNumBytesReceived; }

	FvUInt32 NumPacketsResent() const	{ return m_uiNumPacketsResent; }

	FvUInt32 NumReliablePacketsSent() const { return m_uiNumReliablePacketsSent; }

	FvUInt64 LastReliableSendTime() const { return m_uiLastReliableSendTime; }

	FvUInt64 LastReliableSendOrResendTime() const
	{
		return std::max( m_uiLastReliableSendTime, m_uiLastReliableResendTime );
	}

	void pMessageFilter( FvNetMessageFilter * pMessageFilter )
	{
		m_spMessageFilter = pMessageFilter;
	}

	FvNetMessageFilterPtr pMessageFilter()
	{
		return m_spMessageFilter;
	}

	bool validateUnreliableSeqNum( const FvNetSeqNum seqNum );


	static void SetExternalMaxOverflowPackets( FvUInt16 maxPackets )
	{
		FvNetChannel::ms_uiMaxOverflowPackets[ 0 ] = maxPackets;
	}

	static FvUInt16 GetExternalMaxOverflowPackets()
	{
		return FvNetChannel::ms_uiMaxOverflowPackets[ 0 ];
	}

	static void SetInternalMaxOverflowPackets( FvUInt16 maxPackets )
	{
		FvNetChannel::ms_uiMaxOverflowPackets[ 1 ] = maxPackets;
	}

	static FvUInt16 GetInternalMaxOverflowPackets()
	{
		return FvNetChannel::ms_uiMaxOverflowPackets[ 1 ];
	}

	static void SetIndexedMaxOverflowPackets( FvUInt16 maxPackets )
	{
		FvNetChannel::ms_uiMaxOverflowPackets[ 2 ] = maxPackets;
	}

	static FvUInt16 GetIndexedMaxOverflowPackets()
	{
		return FvNetChannel::ms_uiMaxOverflowPackets[ 2 ];
	}

	static bool ms_bAssertOnMaxOverflowPackets;

	static bool AssertOnMaxOverflowPackets()
	{
		return FvNetChannel::ms_bAssertOnMaxOverflowPackets;
	}

	static void AssertOnMaxOverflowPackets( bool shouldAssert )
	{
		FvNetChannel::ms_bAssertOnMaxOverflowPackets = shouldAssert;
	}

	void Dump(FvXMLSectionPtr spSection);

private:
	enum TimeOutType
	{
		TIMEOUT_INACTIVITY_CHECK,
		TIMEOUT_CHANNEL_PUSH
	};

	virtual int HandleTimeout( FvNetTimerID, void * );

	FvNetNub *m_pkNub;
	Traits m_eTraits;

	FvNetChannelID	m_iID;
	FvNetTimerID m_kChannelPushTimerID;
	FvNetTimerID m_kInactivityTimerID;

	FvUInt64 m_uiInactivityExceptionPeriod;

	FvNetChannelVersion m_uiVersion;

	FvUInt64 m_uiLastReceivedTime;

	FvNetPacketFilterPtr m_spFilter;
	FvNetAddress m_kAddr;
	FvNetBundle *m_pkBundle;

	FvUInt32 m_uiWindowSize;

	FvNetSeqNum m_uiSmallOutSeqAt;
	FvNetSeqNum m_uiLargeOutSeqAt;

	FvNetSeqNum	m_uiFirstMissing;

	FvNetSeqNum m_uiLastMissing;

	FvNetSeqNum m_uiOldestUnackedSeq;

	FvUInt64 m_uiLastReliableSendTime;

	FvUInt64 m_uiLastReliableResendTime;

	FvUInt64 m_uiRoundTripTime;

	FvUInt64 m_uiMinInactivityResendDelay;

	FvNetSeqNum m_uiUnreliableInSeqAt;

	class UnackedPacket
	{
	public:
		UnackedPacket( FvNetPacket * pPacket = NULL );

		FvNetSeqNum seq() const	{ return m_spPacket->GetSeq(); }

		FvNetPacketPtr m_spPacket;

		FvNetSeqNum	m_uiNextMissing;

		FvNetSeqNum m_uiLastSentAtOutSeq;

		FvUInt64 m_uiLastSentTime;

		bool m_bWasResent;

		FvReliableVector m_kReliableOrders;

		static UnackedPacket * initFromStream(
			FvBinaryIStream & data, FvUInt64 timeNow );

		static void addToStream(
			UnackedPacket * pInstance, FvBinaryOStream & data );
	};

	FvNetCircularArray< UnackedPacket * > m_kUnackedPackets;

	typedef std::list< UnackedPacket * > OverflowPackets;
	OverflowPackets m_kOverflowPackets;

	bool m_bHasSeenOverflowWarning;
	void AddOverflowPacket( UnackedPacket *pPacket );

	static unsigned int ms_uiMaxOverflowPackets[3];

	unsigned int GetMaxOverflowPackets() const
	{
		if (this->IsExternal())
		{
			return ms_uiMaxOverflowPackets[ 0 ];
		}

		return ms_uiMaxOverflowPackets[ 1 + !this->IsIndexed() ];
	}

	void SendUnacked( UnackedPacket & unacked );

	FvNetSeqNum m_uiInSeqAt;

	FvNetCircularArray< FvNetPacketPtr > m_kBufferedReceives;
	FvUInt32 m_uiNumBufferedReceives;

	FvNetNub::FragmentedBundlePtr m_spFragments;

	FvUInt32 m_uiLastAck;

	friend class FvNetIrregularChannels;
	FvNetIrregularChannels::iterator m_kIrregularIter;

	friend class FvNetKeepAliveChannels;
	FvNetKeepAliveChannels::iterator m_kKeepAliveIter;

	bool m_bIsIrregular;

	bool m_bIsCondemned;

	bool m_bIsDestroyed;

	FvNetBundlePrimer *m_pkBundlePrimer;

	bool m_bHasRemoteFailed;

	bool m_bIsAnonymous;

	FvNetSeqNum	m_uiUnackedCriticalSeq;

	int m_iPushUnsentAcksThreshold;

	bool m_bShouldAutoSwitchToSrcAddr;

	bool m_bWantsFirstPacket;

	bool m_bShouldDropNextSend;

	static int ms_iSendWindowWarnThresholds[2];

	int &SendWindowWarnThreshold()
	{
		return ms_iSendWindowWarnThresholds[ this->IsIndexed() ];
	}

	static int ms_iSendWindowCallbackThreshold;
	static SendWindowCallback ms_pfSendWindowCallback;

	FvUInt32 m_uiNumPacketsSent;
	FvUInt32 m_uiNumPacketsReceived;
	FvUInt32 m_uiNumBytesSent;
	FvUInt32 m_uiNumBytesReceived;
	FvUInt32 m_uiNumPacketsResent;
	FvUInt32 m_uiNumReliablePacketsSent;

	FvNetMessageFilterPtr m_spMessageFilter;
};


typedef FvSmartPointer< FvNetChannel > FvNetChannelPtr;

class FV_NETWORK_API FvNetChannelOwner
{
public:
	FvNetChannelOwner( FvNetNub & nub, const FvNetAddress & address = FvNetAddress::NONE,
			FvNetChannel::Traits traits = FvNetChannel::INTERNAL,
			float minInactivityResendDelay = 1.0,
			FvNetPacketFilterPtr pFilter = NULL ) :
		m_pkChannel( traits == FvNetChannel::INTERNAL ?
			FvNetChannel::get( nub, address ) :
			new FvNetChannel( nub, address, traits,
				minInactivityResendDelay, pFilter ) )
	{
		FV_ASSERT( traits == FvNetChannel::EXTERNAL ||
			(minInactivityResendDelay == 1.0 && pFilter == NULL) );
	}

	~FvNetChannelOwner()
	{
		m_pkChannel->Condemn();
		m_pkChannel = NULL;
	}

	FvNetBundle & Bundle()				{ return m_pkChannel->Bundle(); }
	const FvNetAddress & Addr() const	{ return m_pkChannel->addr(); }
	const char * c_str() const		{ return m_pkChannel->c_str(); }
	void Send( FvNetBundle * pBundle = NULL ) { m_pkChannel->Send( pBundle ); }

	FvNetChannel &Channel()					{ return *m_pkChannel; }
	const FvNetChannel &Channel() const		{ return *m_pkChannel; }

	void Addr( const FvNetAddress & addr );

#if FV_ENABLE_WATCHERS
	static FvWatcherPtr pWatcher()
	{
		return new FvBaseDereferenceWatcher( FvNetChannel::GetWatcher() );
	}
#endif

private:
	FvNetChannel * m_pkChannel;
};

class FV_NETWORK_API FvNetChannelSender
{
public:
	FvNetChannelSender( FvNetChannel & channel ) :
		m_kChannel( channel )
	{}

	~FvNetChannelSender()
	{
		if (m_kChannel.IsIrregular())
		{
			m_kChannel.DelayedSend();
		}
	}

	FvNetBundle &Bundle() { return m_kChannel.Bundle(); }
	FvNetChannel &Channel() { return m_kChannel; }

private:
	FvNetChannel &m_kChannel;
};

#include "FvNetChannel.inl"

#endif // __FvNetChannel_H__
