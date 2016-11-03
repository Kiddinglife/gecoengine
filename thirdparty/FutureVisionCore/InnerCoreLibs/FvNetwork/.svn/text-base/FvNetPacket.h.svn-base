//{future header message}
#ifndef __FvNetPacket_H__
#define __FvNetPacket_H__

#include "FvNetwork.h"
#include "FvNetMisc.h"

#include <FvBinaryStream.h>
#include <FvDebug.h>
#include <FvSmartpointer.h>

#define PACKET_MAX_SIZE 1472

class FvNetEndpoint;

struct FvNetDataField
{
	char *m_pcBeg;
	FvUInt16 m_uiLength;
};

class FvNetPacket;
typedef FvSmartPointer< FvNetPacket > FvNetPacketPtr;

class FV_NETWORK_API FvNetPacket : public FvReferenceCount
{
public:

	typedef FvUInt16 Flags;

	enum
	{
		FLAG_HAS_REQUESTS			= 0x0001,
		FLAG_HAS_PIGGYBACKS			= 0x0002,
		FLAG_HAS_ACKS				= 0x0004,
		FLAG_ON_CHANNEL				= 0x0008,
		FLAG_IS_RELIABLE			= 0x0010,
		FLAG_IS_FRAGMENT			= 0x0020,
		FLAG_HAS_SEQUENCE_NUMBER	= 0x0040,
		FLAG_INDEXED_CHANNEL		= 0x0080,
		FLAG_HAS_CHECKSUM			= 0x0100,
		FLAG_CREATE_CHANNEL			= 0x0200,
		KNOWN_FLAGS					= 0x03FF
	};

	typedef FvUInt8 AckCount;

	typedef FvUInt16 Offset;

	typedef FvUInt32 Checksum;

private:

	static const int MAX_SIZE;

public:
	static const int HEADER_SIZE = sizeof( Flags );

	static const int MAX_ACKS = (1 << (8 * sizeof( AckCount ))) - 1;

	static const int RESERVED_FOOTER_SIZE =
		sizeof( Offset ) + // FLAG_HAS_REQUESTS
		sizeof( AckCount ) + // FLAG_HAS_ACKS
		sizeof( FvNetSeqNum ) + // FLAG_HAS_SEQUENCE_NUMBER
		sizeof( FvNetSeqNum ) * 2 + // FLAG_IS_FRAGMENT
		sizeof( FvNetChannelID ) + sizeof( FvNetChannelVersion ) + // FLAG_INDEXED_CHANNEL
		sizeof( Checksum ); // FLAG_HAS_CHECKSUM

private:
	FvNetPacketPtr m_spNext;

	int m_iMsgEndOffset;

	int m_iFooterSize;

	int m_iExtraFilterSize;

	Offset m_uiFirstRequestOffset;

	Offset *m_puiLastRequestOffset;

	AckCount m_uiAcks;

	FvNetDataField m_kPiggyFooters;

	FvNetSeqNum m_uiSeq;

	FvNetChannelID m_iChannelID;

	FvNetChannelVersion m_uiChannelVersion;

	FvNetSeqNum m_uiFragBegin;
	FvNetSeqNum m_uiFragEnd;

	Checksum m_uiChecksum;

#ifdef _WIN32
	#pragma warning (push)
	#pragma warning (disable: 4200)
#endif
	char m_acData[PACKET_MAX_SIZE];
#ifdef _WIN32
	#pragma warning (pop)
#endif

public:
	FvNetPacket();

	FvNetPacket * Next()				{ return m_spNext.Get(); }
	const FvNetPacket * Next() const	{ return m_spNext.Get(); }

	void Chain( FvNetPacket * pPacket ) { m_spNext = pPacket; }
	int ChainLength() const;

	Flags GetFlags() const { return FV_NTOHS( *(Flags*)m_acData ); }
	bool HasFlags( Flags flags ) const { return (this->GetFlags() & flags) == flags; }
	void SetFlags( Flags flags ) { *(Flags*)m_acData = FV_HTONS( flags ); }
	void EnableFlags( Flags flags ) { *(Flags*)m_acData |= FV_HTONS( flags ); }
	void DisableFlags( Flags flags ) { *(Flags*)m_acData &= ~FV_HTONS( flags ); }

	char *Data() { return m_acData; }
	const char *Data() const { return m_acData; }

	const char *Body() const { return m_acData + HEADER_SIZE; }

	char *Back() { return m_acData + m_iMsgEndOffset; }

	int MsgEndOffset() const	{ return m_iMsgEndOffset; }
	int BodySize() const		{ return m_iMsgEndOffset - HEADER_SIZE; }
	int FooterSize() const		{ return m_iFooterSize; }
	int TotalSize() const		{ return m_iMsgEndOffset + m_iFooterSize; }

	void MsgEndOffset( int offset )		{ m_iMsgEndOffset = offset; }
	void Grow( int nBytes )				{ m_iMsgEndOffset += nBytes; }
	void Shrink( int nBytes )			{ m_iMsgEndOffset -= nBytes; }

	int FreeSpace() const
	{
		return MAX_SIZE -
			RESERVED_FOOTER_SIZE -
			m_iMsgEndOffset -
			m_iFooterSize -
			m_iExtraFilterSize;
	}

	void ReserveFooter( int nBytes ) { m_iFooterSize += nBytes; }
	void ReserveFilterSpace( int nBytes ) { m_iExtraFilterSize = nBytes; }

	AckCount GetNAcks() const { return m_uiAcks; }
	AckCount & GetNAcks() { return m_uiAcks; }

	FvNetDataField & PiggyFooters() { return m_kPiggyFooters; }

	FvNetSeqNum GetSeq() const { return m_uiSeq; }
	FvNetSeqNum & GetSeq() { return m_uiSeq; }

	FvNetChannelID GetChannelID() const { return m_iChannelID; }
	FvNetChannelID & GetChannelID() { return m_iChannelID; }

	FvNetChannelVersion GetChannelVersion() const { return m_uiChannelVersion; }
	FvNetChannelVersion & GetChannelVersion() { return m_uiChannelVersion; }

	Offset FirstRequestOffset() const { return m_uiFirstRequestOffset; }
	Offset & FirstRequestOffset() { return m_uiFirstRequestOffset; }
	void AddRequest( Offset messageStart, Offset nextRequestLink );

	FvNetSeqNum FragBegin() const { return m_uiFragBegin; }
	FvNetSeqNum & FragBegin() { return m_uiFragBegin; }

	FvNetSeqNum FragEnd() const { return m_uiFragEnd; }
	FvNetSeqNum & FragEnd() { return m_uiFragEnd; }

	Checksum & GetChecksum() { return m_uiChecksum; }

	bool ShouldCreateAnonymous() const
	{
		return this->HasFlags( FLAG_ON_CHANNEL ) &&
			this->HasFlags( FLAG_CREATE_CHANNEL ) &&
			!this->HasFlags( FLAG_INDEXED_CHANNEL );
	}

	template <class TYPE>
	bool StripFooter( TYPE & value )
	{
		if (this->BodySize() < int( sizeof( TYPE ) ))
		{
			return false;
		}

		m_iMsgEndOffset -= sizeof( TYPE );
		m_iFooterSize += sizeof( TYPE );

		switch( sizeof( TYPE ) )
		{
			case sizeof( FvUInt8 ):
				value = TYPE( *(TYPE*)this->Back() ); break;

			case sizeof( FvUInt16 ):
				value = TYPE( FV_NTOHS( *(TYPE*)this->Back() ) ); break;

			case sizeof( FvUInt32 ):
				value = TYPE( FV_NTOHL( *(TYPE*)this->Back() ) ); break;

			default:
				FV_CRITICAL_MSG( "Footers of size %d aren't supported",
					sizeof( TYPE ) );
		}

		return true;
	}


	template <class TYPE>
	void PackFooter( TYPE value )
	{
		m_iMsgEndOffset -= sizeof( TYPE );

		switch( sizeof( TYPE ) )
		{
			case sizeof( FvUInt8 ):
				*(TYPE*)this->Back() = value; break;

			case sizeof( FvUInt16 ):
				*(TYPE*)this->Back() = FV_HTONS( value ); break;

			case sizeof( FvUInt32 ):
				*(TYPE*)this->Back() = FV_HTONL( value ); break;

			default:
				FV_CRITICAL_MSG( "Footers of size %d aren't supported",
					sizeof( TYPE ) );
		}
	}

	int RecvFromEndpoint( FvNetEndpoint & ep, FvNetAddress & addr );

	enum
	{
		UNACKED_SEND,

		BUFFERED_RECEIVE,

		CHAINED_FRAGMENT
	};

	static void AddToStream( FvBinaryOStream & data, const FvNetPacket * pPacket,
		int state );

	static FvNetPacketPtr CreateFromStream( FvBinaryIStream & data, int state );

	static int MaxCapacity()
	{
		return MAX_SIZE - HEADER_SIZE - RESERVED_FOOTER_SIZE;
	}

	void DebugDump() const;
};

#endif // __FvNetPacket_H__
