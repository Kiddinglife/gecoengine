#include "FvNetBundle.h"
#include "FvNetChannel.h"

#include <FvConcurrency.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 )

FvNetBundle::FvNetBundle( FvUInt8 spareSize, FvNetChannel * pChannel ) :
	m_spFirstPacket( NULL ),
	m_pkCurrentPacket( NULL ),
	m_bFinalised( false ),
	m_uiExtraSize( spareSize ),
	m_pkChannel( pChannel )
{
	this->Clear( /* firstTime: */ true );
}

FvNetBundle::FvNetBundle( FvNetPacket * p ) :
	m_spFirstPacket( p ),
	m_pkCurrentPacket( p ),
	m_bFinalised( true ),
	m_uiExtraSize( 0 ),
	m_pkChannel( NULL )
{
	this->Clear( /* firstTime: */ true );
}


FvNetBundle::~FvNetBundle()
{
	this->Dispose();
}

void FvNetBundle::Clear( bool firstTime )
{
	if (!firstTime)
	{
		this->Dispose();
		m_bFinalised = false;
	}

	m_bReliableDriver = false;
	m_iReliableOrdersExtracted = 0;
	m_bIsCritical = false;
	m_pkCurIE = NULL;
	m_iMsgLen = 0;
	m_puiMsgBeg = NULL;
	m_uiMsgChunkOffset = 0;
	m_bMsgReliable = false;
	m_bMsgRequest = false;
	m_iNumMessages = 0;
	m_iNumReliableMessages = 0;

	if (m_spFirstPacket == NULL)
	{
		m_spFirstPacket = new FvNetPacket();
		this->StartPacket( m_spFirstPacket.GetObject() );
	}
}

void FvNetBundle::Dispose()
{
	m_spFirstPacket = NULL;
	m_pkCurrentPacket = NULL;

	m_kReplyOrders.clear();

	m_kReliableOrders.clear();

	for (Piggybacks::iterator iter = m_kPiggybacks.begin();
		 iter != m_kPiggybacks.end(); ++iter)
	{
		delete *iter;
	}

	m_kPiggybacks.clear();

	m_kAckOrders.clear();
}

bool FvNetBundle::IsEmpty() const
{
	bool hasData =
		m_iNumMessages > 0 ||
		this->IsMultiPacket() ||
		this->isReliable() ||
		this->HasDataFooters();

	return !hasData;
}

int FvNetBundle::Size() const
{
	int	total = 0;

	for (const FvNetPacket * p = m_spFirstPacket.GetObject(); p; p = p->Next())
	{
		total += p->TotalSize();
	}

	return total;
}

int FvNetBundle::SizeInPackets() const
{
	return m_spFirstPacket->ChainLength();
}

void FvNetBundle::StartMessage( const FvNetInterfaceElement & ie, FvNetReliableType reliable )
{
	FV_ASSERT( !m_pkCurrentPacket->HasFlags( FvNetPacket::FLAG_HAS_PIGGYBACKS ) );
	FV_ASSERT( ie.GetName() );

	this->EndMessage();
	m_pkCurIE = &ie;
	m_bMsgReliable = reliable.IsReliable();
	m_bMsgRequest = false;
	m_bIsCritical = (reliable == FV_NET_RELIABLE_CRITICAL);
	this->NewMessage();

	m_bReliableDriver |= reliable.IsDriver();
}

void FvNetBundle::StartRequest( const FvNetInterfaceElement & ie,
	FvNetReplyMessageHandler * handler,
	void * arg,
	int timeout,
	FvNetReliableType reliable )
{
	FV_ASSERT( handler );

	if (m_pkChannel && timeout != FV_NET_DEFAULT_REQUEST_TIMEOUT)
	{
		FV_WARNING_MSG( "FvNetBundle::StartRequest(%s): "
				"Non-default timeout set on a channel bundle\n",
			m_pkChannel->c_str() );
	}


	this->EndMessage();
	m_pkCurIE = &ie;
	m_bMsgReliable = reliable.IsReliable();
	m_bMsgRequest = true;
	m_bIsCritical = (reliable == FV_NET_RELIABLE_CRITICAL);

	FvNetReplyID * pReplyID = (FvNetReplyID *)this->NewMessage(
		sizeof( FvNetReplyID ) + sizeof( FvNetPacket::Offset ) );

	FvNetPacket::Offset messageStart = m_pkCurrentPacket->MsgEndOffset() -
		(ie.HeaderSize() +
			sizeof( FvNetReplyID ) + sizeof( FvNetPacket::Offset ));
	FvNetPacket::Offset nextRequestLink = m_pkCurrentPacket->MsgEndOffset() -
		sizeof( FvNetPacket::Offset );

	m_pkCurrentPacket->AddRequest( messageStart, nextRequestLink );

	ReplyOrder	ro = {handler, arg, timeout, pReplyID};
	m_kReplyOrders.push_back(ro);

	m_pkCurrentPacket->EnableFlags( FvNetPacket::FLAG_HAS_REQUESTS );

	m_bReliableDriver |= reliable.IsDriver();
}

void FvNetBundle::StartReply( FvNetReplyID id, FvNetReliableType reliable )
{
	this->EndMessage();
	m_pkCurIE = &FvNetInterfaceElement::REPLY;
	m_bMsgReliable = reliable.IsReliable();
	m_bMsgRequest = false;
	m_bIsCritical = (reliable == FV_NET_RELIABLE_CRITICAL);
	this->NewMessage();

	m_bReliableDriver |= reliable.IsDriver();

	(*this) << id;
}

int FvNetBundle::AddAck( FvNetSeqNum seq )
{
	this->ReserveFooter( sizeof( seq ), FvNetPacket::FLAG_HAS_ACKS );

	AckOrder ao = { m_pkCurrentPacket, seq };
	m_kAckOrders.push_back( ao );

	m_pkCurrentPacket->GetNAcks()++;
	m_pkCurrentPacket->EnableFlags( FvNetPacket::FLAG_HAS_ACKS );

	return m_pkCurrentPacket->GetNAcks();
}

bool FvNetBundle::IsOnExternalChannel() const
{
	return m_pkChannel && m_pkChannel->IsExternal();
}


void * FvNetBundle::Sreserve( int nBytes )
{
	this->EndPacket( /* multiple: */ true );
	this->StartPacket( new FvNetPacket() );

	void * writePosition = m_pkCurrentPacket->Back();
	m_pkCurrentPacket->Grow( nBytes );

	FV_ASSERT( m_pkCurrentPacket->FreeSpace() >= 0 );
	return writePosition;
}

void FvNetBundle::ReserveFooter( int nBytes, FvNetPacket::Flags flag )
{
	if ((nBytes > this->FreeBytesInPacket()) ||
		(flag == FvNetPacket::FLAG_HAS_ACKS &&
			m_pkCurrentPacket->GetNAcks() >= FvNetPacket::MAX_ACKS))
	{
		this->EndPacket( /* multiple: */ true );
		this->StartPacket( new FvNetPacket() );
	}

	m_pkCurrentPacket->ReserveFooter( nBytes );
}

void FvNetBundle::Finalise()
{
	if (m_bFinalised == true) return;
	m_bFinalised = true;

	if (m_puiMsgBeg == NULL && m_pkCurrentPacket->MsgEndOffset() != m_uiMsgChunkOffset)
	{
		FV_CRITICAL_MSG( "FvNetBundle::Finalise: "
			"data not part of message found at end of bundle!\n");
	}

	this->EndMessage();
	this->EndPacket( /* multiple: */ false );

	if (!m_bReliableDriver && this->IsOnExternalChannel())
		m_kReliableOrders.clear();

	bool isReliable =
		(m_kReliableOrders.size() || m_bMsgReliable || m_iNumReliableMessages > 0);

	if (isReliable || this->IsOnExternalChannel())
	{
		for (FvNetPacket * p = m_spFirstPacket.GetObject(); p; p = p->Next())
		{
			if (!p->HasFlags( FvNetPacket::FLAG_HAS_SEQUENCE_NUMBER ))
			{
				p->ReserveFooter( sizeof( FvNetSeqNum ) );
				p->EnableFlags( FvNetPacket::FLAG_HAS_SEQUENCE_NUMBER );
			}

			if (isReliable)
			{
				p->EnableFlags( FvNetPacket::FLAG_IS_RELIABLE );
			}
		}
	}
}

void FvNetBundle::StartPacket( FvNetPacket * p )
{
	FvNetPacket * prevPacket = m_pkCurrentPacket;

	if (prevPacket)
	{
		prevPacket->Chain( p );
	}

	m_pkCurrentPacket = p;
	m_pkCurrentPacket->ReserveFilterSpace( m_uiExtraSize );

	const FvNetPacket::Flags inheritFlags =
		FvNetPacket::FLAG_ON_CHANNEL |
		FvNetPacket::FLAG_IS_RELIABLE |
		FvNetPacket::FLAG_IS_FRAGMENT |
		FvNetPacket::FLAG_HAS_SEQUENCE_NUMBER |
		FvNetPacket::FLAG_INDEXED_CHANNEL;

	if (prevPacket)
	{
		m_pkCurrentPacket->SetFlags( prevPacket->GetFlags() & inheritFlags );
	}
	else
	{
		m_pkCurrentPacket->SetFlags( 0 );
	}

	m_pkCurrentPacket->MsgEndOffset( FvNetPacket::HEADER_SIZE );

	m_uiMsgChunkOffset = m_pkCurrentPacket->MsgEndOffset();
}

void FvNetBundle::EndPacket( bool multiple )
{
	if (multiple)
	{
		if (this->IsOnExternalChannel())
		{
			if (m_puiMsgBeg != NULL && m_bMsgReliable)
				this->AddReliableOrder();

			FvNetReliableOrder rgap = { NULL, 0, 0 };
			m_kReliableOrders.push_back( rgap );
		}

		m_pkCurrentPacket->EnableFlags( FvNetPacket::FLAG_HAS_SEQUENCE_NUMBER |
			FvNetPacket::FLAG_IS_FRAGMENT );
	}

	m_iMsgLen += m_pkCurrentPacket->MsgEndOffset() - m_uiMsgChunkOffset;
	m_uiMsgChunkOffset = FvUInt16( m_pkCurrentPacket->MsgEndOffset() );

	if (this->PacketFlags() & FvNetPacket::FLAG_HAS_REQUESTS)
	{
		m_pkCurrentPacket->ReserveFooter( sizeof( FvNetPacket::Offset ) );
	}

	if (this->PacketFlags() & FvNetPacket::FLAG_IS_FRAGMENT)
	{
		m_pkCurrentPacket->ReserveFooter( sizeof( FvNetSeqNum ) * 2 );
	}

	if (this->PacketFlags() & FvNetPacket::FLAG_HAS_SEQUENCE_NUMBER)
	{
		m_pkCurrentPacket->ReserveFooter( sizeof( FvNetSeqNum ) );
	}

	if (this->PacketFlags() & FvNetPacket::FLAG_HAS_ACKS)
	{
		m_pkCurrentPacket->ReserveFooter( sizeof( FvNetPacket::AckCount ) );
	}

	if (this->PacketFlags() & FvNetPacket::FLAG_INDEXED_CHANNEL)
	{
		m_pkCurrentPacket->ReserveFooter(
			sizeof( FvNetChannelID ) + sizeof( FvNetChannelVersion ) );
	}
}

void FvNetBundle::EndMessage()
{
	if (m_puiMsgBeg == NULL)
	{
		FV_ASSERT( m_pkCurrentPacket->MsgEndOffset() == FvNetPacket::HEADER_SIZE );
		return;
	}

	if (m_bMsgReliable)
	{
		if (this->IsOnExternalChannel())
		{
			this->AddReliableOrder();
		}

		m_bMsgReliable = false;
		m_bMsgRequest = false;
	}

	m_iMsgLen += m_pkCurrentPacket->MsgEndOffset() - m_uiMsgChunkOffset;
	m_uiMsgChunkOffset = FvNetPacket::Offset( m_pkCurrentPacket->MsgEndOffset() );

	m_pkCurIE->CompressLength( m_puiMsgBeg, m_iMsgLen, *this );

	m_puiMsgBeg = NULL;
}

char * FvNetBundle::NewMessage( int extra )
{
	int headerLen = m_pkCurIE->HeaderSize();
	if (headerLen == -1)
	{
		FV_CRITICAL_MSG( "FvNetBundle::newMessage: "
			"tried to add a message with an unknown length format %d\n",
			(int)m_pkCurIE->LengthStyle() );
	}

	++m_iNumMessages;

	if (m_bMsgReliable)
	{
		++m_iNumReliableMessages;
	}

	FvNetMessageID * pHeader = (FvNetMessageID *)this->QReserve( headerLen + extra );

	m_puiMsgBeg = (FvUInt8*)pHeader;
	m_uiMsgChunkOffset = FvNetPacket::Offset( m_pkCurrentPacket->MsgEndOffset() );

	*(FvNetMessageID*)pHeader = m_pkCurIE->GetID();

	m_iMsgLen = 0;
	m_iMsgExtra = extra;

	return (char *)(pHeader + headerLen);
}

void FvNetBundle::AddReliableOrder()
{
	FV_ASSERT( this->IsOnExternalChannel() );

	FvUInt8 * begInCur = (FvUInt8*)m_pkCurrentPacket->Data() + m_uiMsgChunkOffset;
	FvUInt8 * begInCurWithHeader = begInCur - m_iMsgExtra - m_pkCurIE->HeaderSize();

	if (m_puiMsgBeg == begInCurWithHeader)
		begInCur = begInCurWithHeader;

	FvNetReliableOrder rello = { begInCur,
							(FvUInt8*)m_pkCurrentPacket->Back() - begInCur,
							m_bMsgRequest };

	m_kReliableOrders.push_back( rello );
}

void FvNetBundle::ReliableOrders( FvNetPacket * p,
	const FvNetReliableOrder *& roBeg, const FvNetReliableOrder *& roEnd )
{
	if (!m_kReliableOrders.empty())
	{
		const int roSize = m_kReliableOrders.size();
		if (m_spFirstPacket == m_pkCurrentPacket)
		{
			FV_ASSERT( p == m_pkCurrentPacket );
			roBeg = &*m_kReliableOrders.begin();
			roEnd = roBeg + roSize;
		}
		else
		{
			if (p == m_spFirstPacket) m_iReliableOrdersExtracted = 0;

			roBeg = &*m_kReliableOrders.begin() + m_iReliableOrdersExtracted;
			const FvNetReliableOrder * roFirst = &*m_kReliableOrders.begin();
			for (roEnd = roBeg;
				roEnd != roFirst + roSize && roEnd->m_uiSegBegin != NULL;
				++roEnd) ;

			m_iReliableOrdersExtracted = (roEnd+1) - &*m_kReliableOrders.begin();
		}
	}
	else
	{
		roBeg = 0;
		roEnd = 0;
	}
}

bool FvNetBundle::piggyback( FvNetSeqNum seq, const FvReliableVector& reliableOrders,
	FvNetPacket *p )
{
	FvNetPacket::Flags flags =
		FvNetPacket::FLAG_HAS_SEQUENCE_NUMBER |
		FvNetPacket::FLAG_IS_RELIABLE |
		FvNetPacket::FLAG_ON_CHANNEL;

	FvNetPacket *origPacket = m_pkCurrentPacket;

	FvUInt16 totalSize =
		sizeof( FvNetPacket::Flags ) + sizeof( FvNetSeqNum ) + sizeof( FvInt16 );

	for (FvReliableVector::const_iterator it = reliableOrders.begin();
		 it != reliableOrders.end(); ++it)
	{
		totalSize += it->m_uiSegLength;

		if (it->m_uiSegPartOfRequest)
		{
			FV_WARNING_MSG( "Refused to piggyback request #%d\n", it->m_uiSegBegin[0] );
			return false;
		}
	}

	if (p->HasFlags( FvNetPacket::FLAG_HAS_PIGGYBACKS ))
	{
		flags |= FvNetPacket::FLAG_HAS_PIGGYBACKS;
		totalSize += p->PiggyFooters().m_uiLength;
	}

	if (totalSize > this->FreeBytesInPacket())
	{
		return false;
	}

	m_pkCurrentPacket->EnableFlags(
		FvNetPacket::FLAG_HAS_PIGGYBACKS |
		FvNetPacket::FLAG_IS_RELIABLE |
		FvNetPacket::FLAG_HAS_SEQUENCE_NUMBER );

	Piggyback *pPiggy = new Piggyback( p, flags, seq,
		/* len: */ totalSize - sizeof( FvInt16 ) );

	for (FvReliableVector::const_iterator it = reliableOrders.begin();
		 it != reliableOrders.end(); ++it)
	{
		pPiggy->m_kRVec.push_back( *it );
	}

	m_kPiggybacks.push_back( pPiggy );

	this->ReserveFooter( totalSize, FvNetPacket::FLAG_HAS_PIGGYBACKS );

	bool piggybackDoesNotAddPacket = origPacket == m_pkCurrentPacket;
	FV_ASSERT( piggybackDoesNotAddPacket );

	return true;
}


FvNetBundle::iterator FvNetBundle::Begin()
{
	return FvNetBundle::iterator( m_spFirstPacket.GetObject() );
}

FvNetBundle::iterator FvNetBundle::End()
{
	return FvNetBundle::iterator( NULL );
}

FvNetBundle::iterator::iterator( FvNetPacket * first ) :
	m_uiBodyEndOffset( 0 ),
	m_uiOffset( 0 ),
	m_uiDataOffset( 0 ),
	m_iDataLength( 0 ),
	m_pcDataBuffer( NULL )
{
	for (m_pkCursor = first; m_pkCursor != NULL; m_pkCursor = m_pkCursor->Next())
	{
		this->NextPacket();
		if (m_uiOffset < m_uiBodyEndOffset) break;
	}
}

FvNetBundle::iterator::iterator( const FvNetBundle::iterator & i ) :
	m_pkCursor( i.m_pkCursor ),
	m_uiBodyEndOffset( i.m_uiBodyEndOffset ),
	m_uiOffset( i.m_uiOffset ),
	m_uiDataOffset( i.m_uiDataOffset ),
	m_iDataLength( i.m_iDataLength ),
	m_pcDataBuffer( NULL )
{
	if (i.m_pcDataBuffer != NULL)
	{
		m_pcDataBuffer = new char[m_iDataLength];
		memcpy( m_pcDataBuffer, i.m_pcDataBuffer, m_iDataLength );
	}
}

FvNetBundle::iterator::~iterator()
{
	if (m_pcDataBuffer != NULL)
	{
		delete [] m_pcDataBuffer;
		m_pcDataBuffer = NULL;
	}
}


using namespace std;

const FvNetBundle::iterator & FvNetBundle::iterator::operator=(
	const FvNetBundle::iterator & i )
{
	if (this != &i)
	{
		this->~iterator();
		new (this)iterator( i );
	}
	return *this;
}


void FvNetBundle::iterator::NextPacket()
{
	m_uiNextRequestOffset = m_pkCursor->FirstRequestOffset();
	m_uiBodyEndOffset = m_pkCursor->MsgEndOffset();
	m_uiOffset = m_pkCursor->Body() - m_pkCursor->Data();
}


FvNetMessageID FvNetBundle::iterator::MsgID() const
{
	return *(FvNetMessageID*)(m_pkCursor->Data() + m_uiOffset);
}


FvNetUnpackedMessageHeader & FvNetBundle::iterator::Unpack( const FvNetInterfaceElement & ie )
{
	FvUInt16	msgbeg = m_uiOffset;

	if (int(m_uiOffset) + ie.HeaderSize() > int(m_uiBodyEndOffset))
	{
		FV_ERROR_MSG( "FvNetBundle::iterator::Unpack( %s ): "
			"Not enough data on stream at %d for header (%d bytes, needed %d)\n",
			ie.GetName(), (int)m_uiOffset, (int)m_uiBodyEndOffset - (int)m_uiOffset,
			ie.HeaderSize() );

		goto error;
	}

	m_kCurHeader.m_uiIdentifier = this->MsgID();
	m_kCurHeader.m_iLength = ie.ExpandLength( m_pkCursor->Data() + msgbeg, m_pkCursor );

	if (m_kCurHeader.m_iLength == -1)
	{
		FV_ERROR_MSG( "FvNetBundle::iterator::Unpack( %s ): "
			"Error unpacking header length at %d\n",
			ie.GetName(), (int)m_uiOffset );

		goto error;
	}

	msgbeg += ie.HeaderSize();

	if (m_uiNextRequestOffset != m_uiOffset)
	{
		m_kCurHeader.m_cFlags = 0;
	}
	else
	{
		if (int(msgbeg) + sizeof(int)+sizeof(FvUInt16) > int(m_uiBodyEndOffset))
		{
			FV_ERROR_MSG( "FvNetBundle::iterator::Unpack( %s ): "
				"Not enough data on stream at %hu for request ID and NRO "
				"(%hu left, needed %u)\n",
				ie.GetName(), m_uiOffset, m_uiBodyEndOffset - msgbeg,
				sizeof( int ) + sizeof( FvUInt16 ) );

			goto error;
		}

		m_kCurHeader.m_iReplyID = FV_NTOHL( *(FvNetReplyID*)(m_pkCursor->Data() + msgbeg) );
		msgbeg += sizeof(int);

		m_uiNextRequestOffset = FV_NTOHS( *(FvNetPacket::Offset*)(m_pkCursor->Data() + msgbeg) );
		msgbeg += sizeof(FvUInt16);

		m_kCurHeader.m_cFlags = FvNetPacket::FLAG_HAS_REQUESTS;
	}

	if ((int(msgbeg) + m_kCurHeader.m_iLength > int(m_uiBodyEndOffset)) &&
		(m_pkCursor->Next() == NULL))
	{
		FV_ERROR_MSG( "FvNetBundle::iterator::Unpack( %s ): "
			"Not enough data on stream at %d for payload (%d left, needed %d)\n",
			ie.GetName(), (int)m_uiOffset, (int)m_uiBodyEndOffset - msgbeg,
			m_kCurHeader.m_iLength );

		goto error;
	}

	m_uiDataOffset = msgbeg;
	m_iDataLength = m_kCurHeader.m_iLength;

	if (!ie.CanHandleLength( m_iDataLength ))
	{
		m_iDataLength += sizeof( FvInt32 );
	}

	return m_kCurHeader;

error:
	m_kCurHeader.m_cFlags = FvNetPacket::FLAG_IS_FRAGMENT;
	FV_ERROR_MSG( "FvNetBundle::iterator::Unpack: Got corrupted message header\n" );
	return m_kCurHeader;
}


const char * FvNetBundle::iterator::data()
{
	if (m_uiDataOffset + m_iDataLength <= m_uiBodyEndOffset)
	{
		return m_pkCursor->Data() + m_uiDataOffset;
	}

	FV_ASSERT( m_pkCursor->Next() != NULL );
	if (m_pkCursor->Next() == NULL) return NULL;
	FV_ASSERT( m_uiDataOffset <= m_uiBodyEndOffset );

	if (m_uiDataOffset == m_uiBodyEndOffset &&
		FvNetPacket::HEADER_SIZE + m_iDataLength <= m_pkCursor->Next()->MsgEndOffset())
	{
		return m_pkCursor->Next()->Body();
	}

	m_pcDataBuffer = new char[m_iDataLength];
	FvNetPacket *thisPack = m_pkCursor;
	FvUInt16 thisOff = m_uiDataOffset;
	FvUInt16 thisLen;
	for (int len = 0; len < m_iDataLength; len += thisLen)
	{
		if (thisPack == NULL)
		{
			FV_DEBUG_MSG( "FvNetBundle::iterator::data: "
				"Run out of packets after %d of %d bytes put in temp\n",
				len, m_iDataLength );
			return NULL;
		}
		thisLen = thisPack->MsgEndOffset() - thisOff;
		if (thisLen > m_iDataLength - len) thisLen = m_iDataLength - len;
		memcpy( m_pcDataBuffer + len, thisPack->Data() + thisOff, thisLen );
		thisPack = thisPack->Next();
		thisOff = FvNetPacket::HEADER_SIZE;
	}
	return m_pcDataBuffer;
}

void FvNetBundle::iterator::operator++(int)
{
	if (m_pcDataBuffer != NULL)
	{
		delete [] m_pcDataBuffer;
		m_pcDataBuffer = NULL;
	}

	int biggerOffset = int(m_uiDataOffset) + m_iDataLength;
	while (biggerOffset >= int(m_uiBodyEndOffset))
	{
		biggerOffset -= m_uiBodyEndOffset;

		m_pkCursor = m_pkCursor->Next();
		if (m_pkCursor == NULL) break;

		this->NextPacket();

		biggerOffset += m_uiOffset;
	}
	m_uiOffset = biggerOffset;
}


bool FvNetBundle::iterator::operator==(const iterator & x) const
{
	return m_pkCursor == x.m_pkCursor && (m_pkCursor == NULL || m_uiOffset == x.m_uiOffset);
}

bool FvNetBundle::iterator::operator!=(const iterator & x) const
{
	return m_pkCursor != x.m_pkCursor || (m_pkCursor != NULL && m_uiOffset != x.m_uiOffset);
}


const char * FvNetUnpackedMessageHeader::MsgName() const
{
	return m_pkNub ? m_pkNub->MsgName( m_uiIdentifier ) : "";
}


FvNetBundle & FvNetBundleSendingMap::operator[]( const FvNetAddress & addr )
{
	Channels::iterator iter = m_kChannels.find( addr );

	if (iter != m_kChannels.end())
	{
		return iter->second->Bundle();
	}
	else
	{
		FvNetChannel * pChannel = m_kNub.FindChannel( addr, true );
		m_kChannels[ addr ] = pChannel;
		return pChannel->Bundle();
	}
}


void FvNetBundleSendingMap::SendAll()
{
	for (Channels::iterator iter = m_kChannels.begin();
		 iter != m_kChannels.end(); ++iter)
	{
		iter->second->Send();
	}

	m_kChannels.clear();
}
