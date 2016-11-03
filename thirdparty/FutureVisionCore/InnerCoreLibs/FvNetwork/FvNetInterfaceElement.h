//{future header message}
#ifndef __FvNetInterfaceElement_H__
#define __FvNetInterfaceElement_H__

#include "FvNetwork.h"
#include "FvNetMisc.h"
#include "FvNetTypes.h"

#include <FvDebug.h>
#include <FvEMA.h>
#include <FvWatcher.h>
#include <FvGlobalMacros.h>

#include <vector>

class FvNetBundle;
class FvNetPacket;
class FvNetInputMessageHandler;

const char FV_NET_FIXED_LENGTH_MESSAGE = 0;

const char FV_NET_VARIABLE_LENGTH_MESSAGE = 1;

const char FV_NET_INVALID_MESSAGE = 2;

class FV_NETWORK_API FvNetInterfaceElement
{
public:
	FvNetInterfaceElement( const char * name = "", FvNetMessageID id = 0,
			FvInt8 lengthStyle = FV_NET_INVALID_MESSAGE, int lengthParam = 0,
			FvNetInputMessageHandler * pHandler = NULL ) :
		m_uiID( id ),
		m_uiLengthStyle( lengthStyle ),
		m_iLengthParam( lengthParam ),
		m_pcName( name ),
		m_pkHandler( pHandler )
	{}

   	void Set( const char * name, FvNetMessageID id, FvInt8 lengthStyle,
		int lengthParam )
	{
		m_uiID = id;
		m_uiLengthStyle = lengthStyle;
		m_iLengthParam = lengthParam;
		m_pcName = name;
	}

	int HeaderSize() const;
	int NominalBodySize() const;
	int CompressLength( void * header, int length, FvNetBundle & bundle ) const;
	int ExpandLength( void * header, FvNetPacket * pPacket ) const;
	bool CanHandleLength( int length ) const
	{
		return (m_uiLengthStyle != FV_NET_VARIABLE_LENGTH_MESSAGE) ||
			(m_iLengthParam >= int(sizeof( FvInt32 ))) ||
			(length < (1<<(8*m_iLengthParam)) - 1);
	}

	FvNetMessageID GetID() const				{ return m_uiID; }
	void SetID( FvNetMessageID id )				{ m_uiID = id; }

	FvInt8 LengthStyle() const			{ return m_uiLengthStyle; }
	int LengthParam() const				{ return m_iLengthParam; }

	const char *GetName() const			{ return m_pcName; }

	const char *c_str() const;

	FvNetInputMessageHandler *GetHandler() const { return m_pkHandler; }
	void SetHandler( FvNetInputMessageHandler * pHandler ) { m_pkHandler = pHandler; }

	static const FvNetInterfaceElement REPLY;

protected:
	int SpecialExpandLength( void * header, FvNetPacket * pPacket ) const;
	int SpecialCompressLength( void * header, int length,
			FvNetBundle & bundle ) const;

	FvNetMessageID m_uiID;
	FvInt8 m_uiLengthStyle;
	int	m_iLengthParam;
	const char *m_pcName;
	FvNetInputMessageHandler *m_pkHandler;
};

class FV_NETWORK_API FvNetInterfaceElementWithStats : public FvNetInterfaceElement
{
public:

	FvNetInterfaceElementWithStats():
		m_uiMaxBytesReceived( 0 ),
		m_uiNumBytesReceived( 0 ),
		m_uiNumMessagesReceived( 0 ),
		m_kAvgBytesReceivedPerSecond( AVERAGE_BIAS ),
		m_kAvgMessagesReceivedPerSecond( AVERAGE_BIAS )
	{
	}

	void MessageReceived( unsigned int msgLen )
	{
		++m_uiNumMessagesReceived;
		++m_kAvgMessagesReceivedPerSecond.Value();
		m_uiNumBytesReceived += msgLen;
		m_kAvgBytesReceivedPerSecond.Value() += msgLen;
		m_uiMaxBytesReceived = std::max( msgLen, m_uiMaxBytesReceived );
	}

	void Tick()
	{
		m_kAvgBytesReceivedPerSecond.Sample();
		m_kAvgMessagesReceivedPerSecond.Sample();
	}

	unsigned int MaxBytesReceived() const 		{ return m_uiMaxBytesReceived; }

	unsigned int NumBytesReceived() const		{ return m_uiNumBytesReceived; }

	unsigned int NumMessagesReceived() const	{ return m_uiNumMessagesReceived; }

	float AvgMessagesReceivedPerSecond() const
	{ return m_kAvgMessagesReceivedPerSecond.Average(); }

	float AvgBytesReceivedPerSecond() const
	{ return m_kAvgBytesReceivedPerSecond.Average(); }

	float AvgMessageLength() const
	{
		return m_uiNumMessagesReceived ?
			float( m_uiNumBytesReceived ) / float( m_uiNumMessagesReceived ):
			0.0f;
	}

	static FvWatcherPtr GetWatcher();

private:

	int IDAsInt() const				{ return this->GetID(); }

	unsigned int m_uiMaxBytesReceived;

	unsigned int m_uiNumBytesReceived;

	unsigned int m_uiNumMessagesReceived;

	FvAccumulatingEMA< unsigned int > m_kAvgBytesReceivedPerSecond;

	FvAccumulatingEMA< unsigned int > m_kAvgMessagesReceivedPerSecond;

	static const float AVERAGE_BIAS;
};


typedef std::vector< FvNetInterfaceElement > FvNetInterfaceElements;

const unsigned char FV_NET_REPLY_MESSAGE_IDENTIFIER = 0xFF;


#endif // __FvNetInterfaceElement_H__
