//{future header message}
#ifndef __FvNetBundle_H__
#define __FvNetBundle_H__

#include "FvNetwork.h"
#include "FvNetMisc.h"
#include "FvNetPacket.h"
#include "FvNetInterfaceElement.h"

#include <FvPowerDefines.h>
#include <FvBinaryStream.h>

#include <map>

const bool MERCURY_DEFAULT_RELIABLE = true;

class FvNetEndpoint;


class FvNetBundle;
class FvNetInterfaceElement;
class FvNetReplyMessageHandler;
class FvNetChannel;

const int FV_NET_DEFAULT_REQUEST_TIMEOUT = 5000000;

class FV_NETWORK_API FvNetUnpackedMessageHeader
{
public:
	static const int NO_REPLY = -1;

	FvNetMessageID m_uiIdentifier;
	char m_cFlags;
	FvNetReplyID m_iReplyID;
	int	m_iLength;
	FvNetNub *m_pkNub;
	FvNetChannel *m_pkChannel;

	FvNetUnpackedMessageHeader() :
		m_uiIdentifier( 0 ), m_cFlags( 0 ),
		m_iReplyID( FV_NET_REPLY_ID_NONE ), m_iLength( 0 ), m_pkNub( NULL ), m_pkChannel( NULL )
	{}

	const char *MsgName() const;
};

class FvNetPacketMonitor
{
public:
	virtual ~FvNetPacketMonitor() {};

	virtual void PacketOut(const FvNetAddress& addr, const FvNetPacket& packet) = 0;

	virtual void PacketIn(const FvNetAddress& addr, const FvNetPacket& packet) = 0;
};

class FvNetReliableOrder
{
public:
	FvUInt8	*m_uiSegBegin;
	FvUInt16 m_uiSegLength;
	FvUInt16 m_uiSegPartOfRequest;
};

typedef std::vector<FvNetReliableOrder> FvReliableVector;

enum FvNetReliableTypeEnum
{
	FV_NET_RELIABLE_NO = 0,
	FV_NET_RELIABLE_DRIVER = 1,
	FV_NET_RELIABLE_PASSENGER = 2,
	FV_NET_RELIABLE_CRITICAL = 3
};

struct FvNetReliableType
{
	FvNetReliableType( FvNetReliableTypeEnum e ) : m_eReliableType( e ) { }
	FvNetReliableType( bool b ) : m_eReliableType( b ? FV_NET_RELIABLE_DRIVER : FV_NET_RELIABLE_NO ) { }

	bool IsReliable() const	{ return m_eReliableType != FV_NET_RELIABLE_NO; }

	bool IsDriver() const	{ return m_eReliableType & FV_NET_RELIABLE_DRIVER; }

	bool operator== (const FvNetReliableTypeEnum e) { return e == m_eReliableType; }

	FvNetReliableTypeEnum m_eReliableType;
};

class FV_NETWORK_API FvNetBundle : public FvBinaryOStream
{
public:
	FvNetBundle( FvUInt8 spareSize = 0, FvNetChannel * pChannel = NULL );

	FvNetBundle( FvNetPacket * packetChain );
	virtual ~FvNetBundle();

	void Clear( bool firstTime = false );

	bool IsEmpty() const;

	int Size() const;
	int SizeInPackets() const;
	bool IsMultiPacket() const { return m_spFirstPacket->Next() != NULL; }
	int FreeBytesInPacket();
	int NumMessages() const		{ return m_iNumMessages; }

	bool HasDataFooters() const;

	void StartMessage( const FvNetInterfaceElement & ie,
		FvNetReliableType reliable = MERCURY_DEFAULT_RELIABLE );

	void StartRequest( const FvNetInterfaceElement & ie,
		FvNetReplyMessageHandler * handler,
		void * arg = NULL,
		int timeout = FV_NET_DEFAULT_REQUEST_TIMEOUT,
		FvNetReliableType reliable = MERCURY_DEFAULT_RELIABLE );

	void StartReply( FvNetReplyID id, FvNetReliableType reliable = MERCURY_DEFAULT_RELIABLE );

	void *StartStructMessage( const FvNetInterfaceElement & ie,
		FvNetReliableType reliable = MERCURY_DEFAULT_RELIABLE );

	void *StartStructRequest( const FvNetInterfaceElement & ie,
		FvNetReplyMessageHandler * handler,
		void * arg = NULL,
		int timeout = FV_NET_DEFAULT_REQUEST_TIMEOUT,
		FvNetReliableType reliable = MERCURY_DEFAULT_RELIABLE );

	void Reliable( FvNetReliableType currentMsgReliabile );
	bool isReliable() const;

	bool IsCritical() const { return m_bIsCritical; }

	bool IsOnExternalChannel() const;

	FvNetChannel *pChannel() { return m_pkChannel; }

	virtual void *Reserve( int nBytes );
	virtual void AddBlob( const void * pBlob, int size );
	FV_INLINE void *QReserve( int nBytes );

	void ReliableOrders( FvNetPacket * p,
		const FvNetReliableOrder *& roBeg, const FvNetReliableOrder *& roEnd );

 	bool piggyback( FvNetSeqNum seq, const FvReliableVector& reliableOrders,
		FvNetPacket *p );

	class iterator
	{
	public:
		iterator(FvNetPacket * first);
		iterator(const iterator & i);
		~iterator();

		const iterator & operator=( const iterator & i );

		FvNetMessageID MsgID() const;

		FvNetUnpackedMessageHeader &Unpack( const FvNetInterfaceElement & ie );
		const char *data();

		void operator++(int);
		bool operator==(const iterator & x) const;
		bool operator!=(const iterator & x) const;
	private:
		void NextPacket();

		FvNetPacket *m_pkCursor;
		FvUInt16 m_uiBodyEndOffset;
		FvUInt16 m_uiOffset;
		FvUInt16 m_uiDataOffset;
		int	m_iDataLength;
		char *m_pcDataBuffer;

		FvUInt16 m_uiNextRequestOffset;

		FvNetUnpackedMessageHeader m_kCurHeader;
	};

	iterator Begin();
	iterator End();

	void Finalise();

public:	

	FvNetPacketPtr m_spFirstPacket;
	FvNetPacket	*m_pkCurrentPacket;
	bool m_bFinalised;
	bool m_bReliableDriver;
	FvUInt8	m_uiExtraSize;

	struct ReplyOrder
	{
		FvNetReplyMessageHandler *handler;

		void *arg;

		int microseconds;

		FvNetReplyID *pReplyID;
	};

	typedef std::vector<ReplyOrder>	ReplyOrders;

	ReplyOrders	m_kReplyOrders;

	FvReliableVector m_kReliableOrders;
	int	m_iReliableOrdersExtracted;

	bool m_bIsCritical;

	class Piggyback
	{
	public:
		Piggyback( FvNetPacket * pPacket,
				FvNetPacket::Flags flags,
				FvNetSeqNum seq,
				FvInt16 len ) :
			m_spPacket( pPacket ),
			m_uiFlags( flags ),
			m_uiSeq( seq ),
			m_iLen( len )
		{}

		FvNetPacketPtr m_spPacket;
		FvNetPacket::Flags m_uiFlags; 
		FvNetSeqNum m_uiSeq;
		FvInt16	m_iLen;
		FvReliableVector m_kRVec;
	};

	typedef std::vector< Piggyback* > Piggybacks;
	Piggybacks m_kPiggybacks;

	struct AckOrder
	{
		FvNetPacket	*p;
		FvNetSeqNum	forseq;
	};

	typedef std::vector<AckOrder> AckOrders;
	AckOrders m_kAckOrders;

private:
	FvNetChannel *m_pkChannel;

	FvNetInterfaceElement const *m_pkCurIE;
	int	m_iMsgLen;
	int	m_iMsgExtra;
	FvUInt8	*m_puiMsgBeg;
	FvUInt16 m_uiMsgChunkOffset;
	bool m_bMsgReliable;
	bool m_bMsgRequest;

	int	m_iNumMessages;
	int	m_iNumReliableMessages;

public:
	int AddAck( FvNetSeqNum seq );

private:
	void *Sreserve( int nBytes );
	void ReserveFooter( int nBytes, FvNetPacket::Flags flag );
	void Dispose();
	void StartPacket( FvNetPacket * p );
	void EndPacket(bool multiple);
	void EndMessage();
	char *NewMessage( int extra = 0 );
	void AddReliableOrder();

	FvNetPacket::Flags PacketFlags() const { return m_pkCurrentPacket->GetFlags(); }

	FvNetBundle( const FvNetBundle & );
	FvNetBundle & operator=( const FvNetBundle & );
};

class FvNetBundleSendingMap
{
public:
	FvNetBundleSendingMap( FvNetNub & nub ) : m_kNub( nub ) {}
	FvNetBundle & operator[]( const FvNetAddress & addr );
	void SendAll();

private:
	FvNetNub &m_kNub;

	typedef std::map< FvNetAddress, FvNetChannel* > Channels;
	Channels m_kChannels;
};

#include "FvNetBundle.inl"

#endif // __FvNetBundle_H__
