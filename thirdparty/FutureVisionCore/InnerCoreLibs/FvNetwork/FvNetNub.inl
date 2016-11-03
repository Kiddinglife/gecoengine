
FV_INLINE FvNetNubException::FvNetNubException( FvNetReason reason ) :
	m_eReason(reason)
{}

FV_INLINE FvNetReason FvNetNubException::Reason() const
{
	return m_eReason;
}

FV_INLINE bool FvNetNubException::GetAddress( FvNetAddress & /*addr*/ ) const
{
	return false;
}

FV_INLINE NubExceptionWithAddress::NubExceptionWithAddress(
	FvNetReason reason, const FvNetAddress & addr ) :
		FvNetNubException(reason), m_kAddress(addr)
{
}

FV_INLINE bool NubExceptionWithAddress::GetAddress( FvNetAddress & addr ) const
{
	addr = m_kAddress;
	return true;
}

FV_INLINE int FvNetNub::Socket() const
{
	return m_kSocket;
}

FV_INLINE FvNetReason FvNetNub::ReceivedCorruptedPacket()
{
	++m_uiNumCorruptedPacketsReceived;
	return FV_NET_REASON_CORRUPTED_PACKET;
}

FV_INLINE FvNetTimerID FvNetNub::RegisterTimer( int microseconds,
	FvNetTimerExpiryHandler * handler, void * arg )
{
	return this->NewTimer( microseconds, handler, arg, true );
}

FV_INLINE FvNetTimerID FvNetNub::RegisterCallback( int microseconds,
	FvNetTimerExpiryHandler * handler, void * arg )
{
	return this->NewTimer( microseconds, handler, arg, false );
}

FV_INLINE void FvNetNub::SetLatency( float latencyMin, float latencyMax )
{
	m_iArtificialLatencyMin = int( latencyMin * 1000 );
	m_iArtificialLatencyMax = int( latencyMax * 1000 );
}

FV_INLINE void FvNetNub::SetLossRatio( float lossRatio )
{
	m_iArtificialDropPerMillion = int( lossRatio * 1000000 );
}

FV_INLINE void FvNetNub::SetPacketMonitor(FvNetPacketMonitor* pPacketMonitor)
{
	m_pkPacketMonitor = pPacketMonitor;
}

FV_INLINE unsigned int FvNetNub::NumPacketsReceived() const
{
	return m_uiNumPacketsReceived;
}

FV_INLINE unsigned int FvNetNub::NumMessagesReceived() const
{
	return m_uiNumMessagesReceived;
}

FV_INLINE unsigned int FvNetNub::NumBytesReceived() const
{
	return m_uiNumBytesReceived;
}

FV_INLINE unsigned int FvNetNub::NumOverheadBytesReceived() const
{
	return m_uiNumOverheadBytesReceived;
}

FV_INLINE bool FvNetNub::IsVerbose() const
{
	return m_bIsVerbose;
}

FV_INLINE void FvNetNub::IsVerbose( bool value )
{
	m_bIsVerbose = value;
}

FV_INLINE double FvNetNub::BytesSentPerSecondAvg() const
{
	return m_uiNumBytesSent / Delta();
}

FV_INLINE double FvNetNub::BytesSentPerSecondPeak() const
{
	return PeakCalculator( LVT_BytesSent, m_uiNumBytesSent, m_uiLastNumBytesSent );
}

FV_INLINE double FvNetNub::BytesReceivedPerSecondAvg() const
{
	return m_uiNumBytesReceived / Delta();
}

FV_INLINE double FvNetNub::BytesReceivedPerSecondPeak() const
{
	return PeakCalculator( LVT_BytesReceived, m_uiNumBytesReceived, m_uiLastNumBytesReceived );
}

FV_INLINE double FvNetNub::PacketsSentPerSecondAvg() const
{
	return m_uiNumPacketsSent / Delta();
}

FV_INLINE double FvNetNub::PacketsSentPerSecondPeak() const
{
	return PeakCalculator( LVT_PacketsSent, m_uiNumPacketsSent, m_uiLastNumPacketsSent );
}

FV_INLINE double FvNetNub::PacketsReceivedPerSecondAvg() const
{
	return m_uiNumPacketsReceived / Delta();
}

FV_INLINE double FvNetNub::PacketsReceivedPerSecondPeak() const
{
	return PeakCalculator( LVT_PacketsReceived, m_uiNumPacketsReceived, m_uiLastNumPacketsReceived );
}

FV_INLINE double FvNetNub::BundlesSentPerSecondAvg() const
{
	return m_uiNumBundlesSent / Delta();
}

FV_INLINE double FvNetNub::BundlesSentPerSecondPeak() const
{
	return PeakCalculator( LVT_BundlesSent, m_uiNumBundlesSent, m_uiLastNumBundlesSent );
}

FV_INLINE double FvNetNub::BundlesReceivedPerSecondAvg() const
{
	return m_uiNumBundlesReceived / Delta();
}

FV_INLINE double FvNetNub::BundlesReceivedPerSecondPeak() const
{
	return PeakCalculator( LVT_BundlesReceived, m_uiNumBundlesReceived, m_uiLastNumBundlesReceived );
}

FV_INLINE double FvNetNub::MessagesSentPerSecondAvg() const
{
	return m_uiNumMessagesSent / Delta();
}

FV_INLINE double FvNetNub::MessagesSentPerSecondPeak() const
{
	return PeakCalculator( LVT_MessagesSent, m_uiNumMessagesSent, m_uiLastNumMessagesSent );
}

FV_INLINE double FvNetNub::MessagesReceivedPerSecondAvg() const
{
	return m_uiNumMessagesReceived / Delta();
}

FV_INLINE double FvNetNub::MessagesReceivedPerSecondPeak() const
{
	return PeakCalculator( LVT_MessagesReceived, m_uiNumMessagesReceived, m_uiLastNumMessagesReceived );
}

FV_INLINE unsigned int FvNetNub::NumBytesReceivedForMessage( FvUInt8 id ) const
{
	return m_kInterfaceTable[id].NumBytesReceived();
}

FV_INLINE unsigned int FvNetNub::NumMessagesReceivedForMessage( FvUInt8 id ) const
{
	return m_kInterfaceTable[id].NumMessagesReceived();
}

FV_INLINE FvNetNub::MiniTimer::MiniTimer() :
	m_uiTotal( 0 ),
	m_uiLast( 0 ),
	m_uiMin( 1000000000 ),
	m_uiMax( 0 ),
	m_uiCount( 0 )
{
}

FV_INLINE void FvNetNub::MiniTimer::start()
{
	m_uiSofar = Timestamp();
}

FV_INLINE void FvNetNub::MiniTimer::stop( bool increment )
{
	m_uiLast += Timestamp() - m_uiSofar;
	if (increment)
	{
		if (m_uiLast > m_uiMax) m_uiMax = m_uiLast;
		if (m_uiLast < m_uiMin) m_uiMin = m_uiLast;
		m_uiTotal += m_uiLast;
		m_uiCount++;
		m_uiLast = 0;
	}
}

FV_INLINE void FvNetNub::MiniTimer::reset()
{
	*this = MiniTimer();
}

FV_INLINE double FvNetNub::MiniTimer::GetMinDurationSecs() const
{
	return double(m_uiMin)/StampsPerSecond();
}

FV_INLINE double FvNetNub::MiniTimer::GetMaxDurationSecs() const
{
	return double(m_uiMax)/StampsPerSecond();
}

FV_INLINE double FvNetNub::MiniTimer::GetAvgDurationSecs() const
{
	return (m_uiCount > 0) ? (double(m_uiTotal)/m_uiCount)/StampsPerSecond() : 0.0;
}

FV_INLINE FvNetNub::TransientMiniTimer::TransientMiniTimer(int resetPeriodSecs)
	: FvNetNub::MiniTimer()
	, m_uiResetPeriodStamp(resetPeriodSecs * StampsPerSecond())
	, m_uiResetTimeStamp(Timestamp())
{}

FV_INLINE void FvNetNub::TransientMiniTimer::start()
{
	if ((Timestamp() - m_uiResetTimeStamp) > m_uiResetPeriodStamp)
		reset();
	FvNetNub::MiniTimer::start();
}

FV_INLINE void FvNetNub::TransientMiniTimer::stop()
{
	FvNetNub::MiniTimer::stop(true);
}

FV_INLINE void FvNetNub::TransientMiniTimer::reset()
{
	m_uiResetTimeStamp = Timestamp();
	FvNetNub::MiniTimer::reset();
}

FV_INLINE double FvNetNub::TransientMiniTimer::GetElapsedResetSecs() const
{
	return double(Timestamp() - m_uiResetTimeStamp)/StampsPerSecond();
}

FV_INLINE double FvNetNub::TransientMiniTimer::GetCountPerSec() const
{
	return double(m_uiCount)/GetElapsedResetSecs();
}

FV_INLINE bool FvNetNub::FragmentedBundle::Key::operator==(const FvNetNub::FragmentedBundle::Key& kOther) const
{
	return (this->m_uiFirstFragment == kOther.m_uiFirstFragment) && (this->m_kAddr == kOther.m_kAddr);
}

FV_INLINE bool FvNetNub::FragmentedBundle::Key::operator<(const FvNetNub::FragmentedBundle::Key& kOther) const
{
	return (this->m_uiFirstFragment < kOther.m_uiFirstFragment) ||
		(this->m_uiFirstFragment == kOther.m_uiFirstFragment && this->m_kAddr < kOther.m_kAddr);
}

FV_INLINE void FvNetNub::PrepareToShutDown()
{
	m_kKeepAliveChannels.StopMonitoring( *this );
}
