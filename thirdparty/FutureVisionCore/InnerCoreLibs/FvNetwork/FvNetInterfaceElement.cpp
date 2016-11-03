#include "FvNetInterfaceElement.h"
#include "FvNetBundle.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 )

const FvNetInterfaceElement FvNetInterfaceElement::REPLY( "Reply",
	FV_NET_REPLY_MESSAGE_IDENTIFIER, FV_NET_VARIABLE_LENGTH_MESSAGE, 4 );


const float FvNetInterfaceElementWithStats::AVERAGE_BIAS = -2.f / (5 + 1);

int FvNetInterfaceElement::HeaderSize() const
{
	int headerLen = sizeof( FvNetMessageID );
	switch (m_uiLengthStyle)
	{
		case FV_NET_FIXED_LENGTH_MESSAGE:
			break;
		case FV_NET_VARIABLE_LENGTH_MESSAGE:
			headerLen += m_iLengthParam;
			break;
		default:
			headerLen = -1;
			break;
	}
	return headerLen;
}

int FvNetInterfaceElement::NominalBodySize() const
{
	return (m_uiLengthStyle == FV_NET_FIXED_LENGTH_MESSAGE) ? m_iLengthParam : 0;
}

class BundleDataPos
{
public:
	BundleDataPos( FvNetPacket * pPacket, void * pCurr ) :
		pPacket_( pPacket ),
		pCurr_( (FvUInt8*)pCurr )
	{
		FV_ASSERT( pPacket->Body() <= pCurr && pCurr < pPacket->Back() );
	}

	FvNetPacket * pPacket() const { return pPacket_; }
	FvUInt8 * pData() const	{ return pCurr_; }

	bool Advance( unsigned int distance )
	{
		if ((pPacket_ == NULL) || (pCurr_ == NULL))
			return false;

		int remainingLength = distance;
		FvUInt8 * pEnd = (FvUInt8*)pPacket_->Back();

		while (remainingLength >= (pEnd - pCurr_))
		{
			remainingLength -= (pEnd - pCurr_);
			pPacket_ = pPacket_->Next();
			if (pPacket_ == NULL)
			{
				FV_ERROR_MSG( "BundleDataPos::AdvancedBy: "
								"Ran out of packets.\n" );
				return false;
			}
			pCurr_ = (FvUInt8*)pPacket_->Body();
			pEnd = (FvUInt8*)pPacket_->Back();
		}
		pCurr_ += remainingLength;

		return true;
	}

private:
	FvNetPacket * pPacket_;
	FvUInt8 * pCurr_;
};

int FvNetInterfaceElement::SpecialCompressLength( void * data, int length,
		FvNetBundle & bundle ) const
{
	FV_WARNING_MSG( "FvNetInterfaceElement::CompressLength( %s ): "
		"length %d exceeds maximum of length format %d\n",
		this->c_str(), length, (1 << (m_iLengthParam*8)) - 1 );

	const int IDENTIFIER_SIZE = sizeof(FvUInt8);
	for (int i = IDENTIFIER_SIZE; i <= m_iLengthParam; ++i)
	{
		((FvUInt8*)data)[i] = 0xff;
	}

	FvNetPacket * pPacket = bundle.m_spFirstPacket.GetObject();
	while (pPacket &&
			!(pPacket->Body() <= data && data < pPacket->Back()))
	{
		pPacket = pPacket->Next();
	}

	FV_IF_NOT_ASSERT_DEV (pPacket != NULL)
	{
		FV_ERROR_MSG( "FvNetInterfaceElement::CompressLength( %s ): "
			"data not in any packets\n",
			this->c_str() );

		return -1;
	}

	BundleDataPos head( pPacket, data );
	head.Advance( IDENTIFIER_SIZE + m_iLengthParam );

	FvUInt32 localLength = length;
	FvUInt8 * pTail = (FvUInt8*)bundle.Reserve( sizeof( FvInt32 ) );

	for (int i = 0; i < int(sizeof( FvInt32 )); ++i)
	{
		*pTail = *head.pData();

		*head.pData() = (FvUInt8)localLength;
		localLength >>= 8;

		++pTail;

		bool advanceSuccessful = head.Advance( 1 );

		FV_IF_NOT_ASSERT_DEV (advanceSuccessful)
		{
			FV_ERROR_MSG( "FvNetInterfaceElement::CompressLength( %s ): "
				"head not in packets.\n",
				this->c_str() );

			return -1;
		}
	}

	return 0;
}

int FvNetInterfaceElement::CompressLength( void * data, int length,
		FvNetBundle & bundle ) const
{
	switch (m_uiLengthStyle)
	{
	case FV_NET_FIXED_LENGTH_MESSAGE:
		if (length != m_iLengthParam)
		{
			FV_CRITICAL_MSG( "FvNetInterfaceElement::CompressLength( %s ): "
				"Fixed length message has wrong length (%d instead of %d)\n",
				this->c_str(), length, m_iLengthParam );
		}
		break;

	case FV_NET_VARIABLE_LENGTH_MESSAGE:
	{
		if (length < 0)
		{
			FV_ERROR_MSG( "FvNetInterfaceElement::CompressLength( %s ): "
				"Possible overflow in length (%d bytes) for "
				"variable length message\n",
				this->c_str(), length );

			return -1;
		}

		char *pLen = ((char*)data) + 1;
		int len = length;
		bool oversize = false;

		switch (m_iLengthParam)
		{
			case 1:
				*(FvUInt8*)pLen = (FvUInt8)len;
				if (len >= 0xff) oversize = true;
				break;

			case 2:
				*(FvUInt16*)pLen = FV_HTONS( (FvUInt16)len );
				if (len >= 0xffff) oversize = true;
				break;

			case 3:
				FV_PACK3( pLen, len );
				if (len >= 0xffffff) oversize = true;
				break;

			case 4:
				*(FvUInt32*)pLen = FV_HTONL( (FvUInt32)len );
				break;

			default:
				FV_CRITICAL_MSG( "FvNetInterfaceElement::CompressLength( %s ): "
					"Unsupported variable length width: %d\n",
					this->c_str(), m_iLengthParam );
		}

		if (oversize)
		{
			return this->SpecialCompressLength( data, length, bundle );
		}
		break;
	}
	default:
		FV_ERROR_MSG( "FvNetInterfaceElement::CompressLength( %s ): "
			"Unrecognised length format %d\n",
			this->c_str(), (int)m_uiLengthStyle );

		return -1;
		break;
	}

	return 0;
}

int FvNetInterfaceElement::SpecialExpandLength( void * data, FvNetPacket * pPacket ) const
{
	const int IDENTIFIER_SIZE = sizeof( FvUInt8 );

	FV_WARNING_MSG( "FvNetInterfaceElement::ExpandLength( %s ): "
		"Received a message longer than normal length\n",
		this->c_str() );

	int len = 0;
	{
		BundleDataPos curr( pPacket, data );
		curr.Advance( IDENTIFIER_SIZE + m_iLengthParam );
		for (int i = 0; i < (int)sizeof(FvInt32); ++i)
		{
			len |= FvUInt32(*curr.pData()) << 8*i;
			if (!curr.Advance( 1 ))
			{
				FV_ERROR_MSG( "FvNetInterfaceElement::ExpandLength( %s ): "
					"Ran out of packets.\n",
					this->c_str() );

				return -1;
			}
		}
	}

	BundleDataPos head( pPacket, data );
	head.Advance( IDENTIFIER_SIZE + m_iLengthParam );
	BundleDataPos tail = head;

	bool tailAdvanceSuccess = tail.Advance( len );
	FV_IF_NOT_ASSERT_DEV (tailAdvanceSuccess)
	{
		FV_ERROR_MSG( "FvNetInterfaceElement::ExpandLength( %s ): "
			"Could not find tail.\n",
			this->c_str() );

		return -1;
	}

	for (int i = (int)sizeof( FvInt32 ) - 1; i >= 0; --i)
	{
		*head.pData() = *tail.pData();

		if (i > 0)
		{
			head.Advance( 1 );
			tailAdvanceSuccess = tail.Advance( 1 );
			FV_IF_NOT_ASSERT_DEV (tailAdvanceSuccess)
			{
				FV_ERROR_MSG( "FvNetInterfaceElement::ExpandLength( %s ): "
					"Ran out of tail.\n",
					this->c_str() );

				return -1;
			}
		}
	}

	return len;
}

int FvNetInterfaceElement::ExpandLength( void * data, FvNetPacket * pPacket ) const
{
	switch(m_uiLengthStyle)
	{
	case FV_NET_FIXED_LENGTH_MESSAGE:
		return m_iLengthParam;
		break;
	case FV_NET_VARIABLE_LENGTH_MESSAGE:
	{
		FvUInt8 *pLen = ((FvUInt8*)data) + sizeof( FvNetMessageID );
		FvUInt32 len = 0;

		switch (m_iLengthParam)
		{
			case 1: len = *(FvUInt8*)pLen; break;
			case 2: len = FV_NTOHS( *(FvUInt16*)pLen ); break;
			case 3: len = FV_UNPACK3( (const char*)pLen ); break;
			case 4: len = FV_NTOHL( *(FvUInt32*)pLen ); break;
			default:
				FV_CRITICAL_MSG( "FvNetInterfaceElement::ExpandLength( %s ): "
					"Unhandled variable message length: %d\n",
					this->c_str(), m_iLengthParam );
		}

		if ((int)len < 0)
		{
			FV_ERROR_MSG( "FvNetInterfaceElement::ExpandLength( %s ): "
				"Overflow in calculating length of variable message!\n",
				this->c_str() );

			return -1;
		}

		if (!this->CanHandleLength( len ))
		{
			return this->SpecialExpandLength( data, pPacket );
		}

		return len;
		break;
	}
	default:
		FV_ERROR_MSG( "FvNetInterfaceElement::ExpandLength( %s ): "
			"unrecognised length format %d\n",
			this->c_str(), (int)m_uiLengthStyle );

		break;
	}
	return -1;
}

const char * FvNetInterfaceElement::c_str() const
{
	static char buf[ 256 ];
	FvSNPrintf( buf, sizeof( buf ), "%s/%d", m_pcName, m_uiID );
	return buf;
}

#if FV_ENABLE_WATCHERS
FvWatcherPtr FvNetInterfaceElementWithStats::GetWatcher()
{
	static FvDirectoryWatcherPtr spWatcher = NULL;

	if (spWatcher == NULL)
	{
		FvNetInterfaceElementWithStats * pNULL = NULL;

		spWatcher = new FvDirectoryWatcher();

		spWatcher->AddChild( "name",
				MakeWatcher( pNULL->m_pcName ) );

		spWatcher->AddChild( "id",
				MakeWatcher( *pNULL, &FvNetInterfaceElementWithStats::IDAsInt ) );

		spWatcher->AddChild( "maxBytesReceived",
				MakeWatcher( pNULL->m_uiMaxBytesReceived ) );

		spWatcher->AddChild( "bytesReceived",
				MakeWatcher( pNULL->m_uiNumBytesReceived ) );

		spWatcher->AddChild( "messagesReceived",
				MakeWatcher( pNULL->m_uiNumMessagesReceived ) );

		spWatcher->AddChild( "avgMessageLength",
				MakeWatcher( *pNULL,
					&FvNetInterfaceElementWithStats::AvgMessageLength ) );

		spWatcher->AddChild( "avgBytesPerSecond",
				MakeWatcher( *pNULL,
					&FvNetInterfaceElementWithStats::AvgBytesReceivedPerSecond ) );
		spWatcher->AddChild( "avgMessagesPerSecond",
				MakeWatcher( *pNULL,
					&FvNetInterfaceElementWithStats::
						AvgMessagesReceivedPerSecond ) );
	}

	return spWatcher;
}
#endif
