
FV_INLINE int FvNetChannel::WindowSize() const
{
	return m_uiWindowSize;
}

FV_INLINE const FvNetAddress & FvNetChannel::addr() const
{
	return m_kAddr;
}

FV_INLINE FvNetSeqNum FvNetChannel::SeqMask( FvNetSeqNum x )
{
	return x & SEQ_MASK;
}

FV_INLINE bool FvNetChannel::SeqLessThan( FvNetSeqNum a, FvNetSeqNum b )
{
	return SeqMask( a - b ) > SEQ_SIZE/2;
}
