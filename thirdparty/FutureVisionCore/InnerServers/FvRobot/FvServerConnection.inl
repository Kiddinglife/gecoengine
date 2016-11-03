
FV_INLINE
void FvServerConnection::SetInactivityTimeout( float seconds )
{
	m_fInactivityTimeout = seconds;
}

FV_INLINE FvUInt32 FvServerConnection::PacketsIn() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kPacketsIn;
}

FV_INLINE FvUInt32 FvServerConnection::PacketsOut() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kPacketsOut;
}

FV_INLINE FvUInt32 FvServerConnection::BitsIn() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kBitsIn;
}

FV_INLINE FvUInt32 FvServerConnection::BitsOut() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kBitsOut;
}

FV_INLINE FvUInt32 FvServerConnection::MessagesIn() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kMessagesIn;
}

FV_INLINE FvUInt32 FvServerConnection::MessagesOut() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kMessagesOut;
}

FV_INLINE void FvServerConnection::pTime( const double * pTime )
{
	m_pkTime = pTime;
}

FV_INLINE double FvServerConnection::AppTime() const
{
	return (m_pkTime != NULL) ? *m_pkTime : 0.f;
}

