#include "FvNetPacket.h"
#include "FvNetEndpoint.h"
#include "FvNetChannel.h"

#include <FvBinaryStream.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 );

const int FvNetPacket::MAX_SIZE = PACKET_MAX_SIZE;

FvNetPacket::FvNetPacket() :
	m_spNext( NULL ),
	m_iMsgEndOffset( 0 ),
	m_iFooterSize( 0 ),
	m_iExtraFilterSize( 0 ),
	m_uiFirstRequestOffset( 0 ),
	m_puiLastRequestOffset( NULL ),
	m_uiAcks( 0 ),
	m_uiSeq( FvNetChannel::SEQ_NULL ),
	m_iChannelID( FV_NET_CHANNEL_ID_NULL ),
	m_uiChannelVersion( 0 ),
	m_uiFragBegin( FvNetChannel::SEQ_NULL ),
	m_uiFragEnd( FvNetChannel::SEQ_NULL ),
	m_uiChecksum( 0 )
{
	m_kPiggyFooters.m_pcBeg = NULL;
}

int FvNetPacket::ChainLength() const
{
	int count = 1;

	for (const FvNetPacket * p = this->Next(); p != NULL; p = p->Next())
	{
		++count;
	}

	return count;
}

void FvNetPacket::AddRequest( Offset messageStart, Offset nextRequestLink )
{
	if (m_uiFirstRequestOffset == 0)
	{
		m_uiFirstRequestOffset = messageStart;
	}
	else
	{
		*m_puiLastRequestOffset = FV_HTONS( messageStart );
	}

	m_puiLastRequestOffset = (Offset*)(m_acData + nextRequestLink);

	*m_puiLastRequestOffset = 0;
}

int FvNetPacket::RecvFromEndpoint( FvNetEndpoint & ep, FvNetAddress & addr )
{
	int len = ep.RecvFrom( m_acData, MAX_SIZE,
		(u_int16_t*)&addr.m_uiPort, (u_int32_t*)&addr.m_uiIP );

	if (len >= 0)
	{
		this->MsgEndOffset( len );
	}

	return len;
}

void FvNetPacket::AddToStream( FvBinaryOStream & data, const FvNetPacket * pPacket,
	int state )
{
	data << FvUInt8( pPacket != NULL );

	if (pPacket)
	{
		if (state == UNACKED_SEND)
		{
			data.AppendString( pPacket->Data(), pPacket->TotalSize() );
		}

		else
		{
			data.AppendString( pPacket->Data(), pPacket->MsgEndOffset() );
		}

		data << pPacket->GetSeq() << pPacket->GetChannelID();

		if (state == CHAINED_FRAGMENT)
		{
			data << pPacket->FragBegin() << pPacket->FragEnd() <<
				pPacket->FirstRequestOffset();
		}
	}
}

FvNetPacketPtr FvNetPacket::CreateFromStream( FvBinaryIStream & data, int state )
{
	FvUInt8 hasPacket;
	data >> hasPacket;

	if (!hasPacket)
		return NULL;

	FvNetPacketPtr pPacket = new FvNetPacket();
	int length = data.ReadStringLength();

	memcpy( pPacket->m_acData, data.Retrieve( length ), length );
	pPacket->MsgEndOffset( length );
	data >> pPacket->GetSeq() >> pPacket->GetChannelID();

	if (state == CHAINED_FRAGMENT)
	{
		data >> pPacket->FragBegin() >> pPacket->FragEnd() >>
			pPacket->FirstRequestOffset();
	}

	return pPacket;
}

void FvNetPacket::DebugDump() const
{
	FV_DEBUG_MSG( "Packet length is %d\n", this->TotalSize() );

	int lineSize = 1024;
	char line[ 1024 ];
	char * s = line;

	for (long i=0; i < this->TotalSize(); i++)
	{
		FvSNPrintf( s, lineSize, "%02x ",
				(unsigned char)this->Data()[i] ); s += 3;
		lineSize -= 3;
		if (i > 0 && i % 20 == 19)
		{
			FV_DEBUG_MSG( "%s\n", line );
			s = line;
			lineSize = 1024;
		}
	}

	if (s != line)
	{
		FV_DEBUG_MSG( "%s\n", line );
	}
}

