
FV_INLINE void FvNetBundle::Reliable( FvNetReliableType currentMsgReliable )
{
	m_bMsgReliable = currentMsgReliable.IsReliable();
	m_bReliableDriver |= currentMsgReliable.IsDriver();
}


FV_INLINE bool FvNetBundle::isReliable() const
{
	return m_bReliableDriver;
}


FV_INLINE void * FvNetBundle::StartStructMessage( const FvNetInterfaceElement & ie,
	FvNetReliableType reliable )
{
	//FV_ASSERT( ie.lengthStyle == kFixedLengthMessage );
	StartMessage( ie, reliable );
	return QReserve( ie.LengthParam() );
}

FV_INLINE void * FvNetBundle::StartStructRequest( const FvNetInterfaceElement & ie,
	FvNetReplyMessageHandler * handler, void * arg,
	int timeout, FvNetReliableType reliable)
{
	//FV_ASSERT( ie.lengthStyle == kFixedLengthMessage );
	StartRequest( ie, handler, arg, timeout, reliable );
	return QReserve( ie.LengthParam() );
}

FV_INLINE int FvNetBundle::FreeBytesInPacket()
{
	return m_pkCurrentPacket->FreeSpace();
}

FV_INLINE bool FvNetBundle::HasDataFooters() const
{
	for (const FvNetPacket * p = m_spFirstPacket.GetObject(); p; p = p->Next())
	{
		if (p->HasFlags( FvNetPacket::FLAG_HAS_ACKS ) ||
			p->HasFlags( FvNetPacket::FLAG_HAS_PIGGYBACKS ))
		{
			return true;
		}
	}

	return false;
}

FV_INLINE void * FvNetBundle::Reserve( int nBytes )
{
	return QReserve( nBytes );
}

FV_INLINE void * FvNetBundle::QReserve( int nBytes )
{
	if (nBytes <= m_pkCurrentPacket->FreeSpace())
	{
		void * writePosition = m_pkCurrentPacket->Back();
		m_pkCurrentPacket->Grow( nBytes );
		return writePosition;
	}
	else
	{
		return this->Sreserve( nBytes );
	}
}


FV_INLINE
void FvNetBundle::AddBlob( const void * pBlob, int size )
{
	const char * pCurr = (const char *)pBlob;

	while (size > 0)
	{
		if (this->FreeBytesInPacket() == 0)
		{
			this->Sreserve( 0 );
		}

		int currSize = std::min( size, int( this->FreeBytesInPacket() ) );
		FV_ASSERT( currSize > 0 );

		memcpy( this->QReserve( currSize ), pCurr, currSize );
		size -= currSize;
		pCurr += currSize;
	}
}
