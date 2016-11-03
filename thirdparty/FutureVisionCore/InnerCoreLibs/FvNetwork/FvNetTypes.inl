
FV_INLINE FvNetAddress::FvNetAddress()
{
}

FV_INLINE FvNetAddress::FvNetAddress(FvUInt32 ipArg, FvUInt16 portArg) :
	m_uiIP(ipArg),
	m_uiPort(portArg),
	m_uiSalt(0)
{
}

FV_INLINE bool operator==(const FvNetAddress & a, const FvNetAddress & b)
{
	return (a.m_uiIP == b.m_uiIP) && (a.m_uiPort == b.m_uiPort);
}

FV_INLINE bool operator!=(const FvNetAddress & a, const FvNetAddress & b)
{
	return (a.m_uiIP != b.m_uiIP) || (a.m_uiPort != b.m_uiPort);
}

FV_INLINE bool operator<(const FvNetAddress & a, const FvNetAddress & b)
{
	return (a.m_uiIP < b.m_uiIP) || (a.m_uiIP == b.m_uiIP && (a.m_uiPort < b.m_uiPort));
}

FV_INLINE bool operator==(const FvEntityMailBoxRef & a, const FvEntityMailBoxRef & b)
{
	return (a.m_iID == b.m_iID) && (a.m_kAddr == b.m_kAddr) && (a.m_kAddr.m_uiSalt == b.m_kAddr.m_uiSalt);
}

FV_INLINE bool operator!=(const FvEntityMailBoxRef & a, const FvEntityMailBoxRef & b)
{
	return (a.m_iID != b.m_iID) || (a.m_kAddr != b.m_kAddr) || (a.m_kAddr.m_uiSalt != b.m_kAddr.m_uiSalt);
}

FV_INLINE FvCapabilities::FvCapabilities() :
	m_uiCaps( 0 )
{
}

FV_INLINE void FvCapabilities::Add( unsigned int cap )
{
	m_uiCaps |= 1 << cap;
}

FV_INLINE bool FvCapabilities::Has( unsigned int cap ) const
{
	return !!(m_uiCaps & (1 << cap));
}

FV_INLINE bool FvCapabilities::Empty() const
{
	return m_uiCaps == 0;
}

FV_INLINE bool FvCapabilities::Match( const FvCapabilities& on,
		const FvCapabilities& off ) const
{
	return (on.m_uiCaps & m_uiCaps ) == on.m_uiCaps &&
		(off.m_uiCaps & ~m_uiCaps) == off.m_uiCaps;
}

FV_INLINE bool FvCapabilities::MatchAny( const FvCapabilities& on,
		const FvCapabilities& off ) const
{
	return !!(on.m_uiCaps & m_uiCaps ) && (off.m_uiCaps & ~m_uiCaps) == off.m_uiCaps;
}

FV_INLINE bool operator==(const FvSpaceEntryID & a, const FvSpaceEntryID & b)
{
	return operator==( (FvNetAddress)a, (FvNetAddress)b ) && a.m_uiSalt == b.m_uiSalt;
}

FV_INLINE bool operator!=(const FvSpaceEntryID & a, const FvSpaceEntryID & b)
{
	return operator!=( (FvNetAddress)a, (FvNetAddress)b ) || a.m_uiSalt != b.m_uiSalt;
}

FV_INLINE bool operator<(const FvSpaceEntryID & a, const FvSpaceEntryID & b)
{
	return operator<( (FvNetAddress)a, (FvNetAddress)b ) ||
		(operator==( (FvNetAddress)a, (FvNetAddress)b ) && (a.m_uiSalt < b.m_uiSalt));
}

