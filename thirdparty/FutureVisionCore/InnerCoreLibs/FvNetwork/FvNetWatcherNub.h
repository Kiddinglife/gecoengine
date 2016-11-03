//{future header message}
#ifndef __FvNetWatcherNub_H__
#define __FvNetWatcherNub_H__

#include "FvNetwork.h"
#include "FvNetEndpoint.h"

#include <FvMemoryStream.h>

struct FvNetWatcherRegistrationMsg
{
	int m_iVersion;	
	int m_iUID;
	int m_iMessage;
	int m_iID;
	char m_acABRV[32];
	char m_acName[64];
};

#ifdef _WIN32
	#pragma warning(disable: 4200)
#endif

struct FvNetWatcherDataMsg
{
	int m_iMessage;
	int	m_iCount;
	char m_acString[0];
};


enum FvNetWatcherMsg
{
	FV_NET_WATCHER_MSG_REGISTER = 0,
	FV_NET_WATCHER_MSG_DEREGISTER = 1,
	FV_NET_WATCHER_MSG_FLUSHCOMPONENTS = 2,

	FV_NET_WATCHER_MSG_GET = 16,
	FV_NET_WATCHER_MSG_SET = 17,
	FV_NET_WATCHER_MSG_TELL = 18,
	FV_NET_WATCHER_MSG_GET_WITH_DESC = 20,

	FV_NET_WATCHER_MSG_GET2 = 26,
	FV_NET_WATCHER_MSG_SET2 = 27,
	FV_NET_WATCHER_MSG_TELL2 = 28,
	FV_NET_WATCHER_MSG_SET2_TELL2 = 29,

	FV_NET_WATCHER_MSG_EXTENSION_START = 107
};


class FvNetWatcherPacketHandler;

class FV_NETWORK_API FvNetWatcherRequestHandler
{
public:
	virtual ~FvNetWatcherRequestHandler() {};

	virtual void ProcessWatcherGetRequest( 
			FvNetWatcherPacketHandler & packetHandler,
			const char * path,
		   	bool withDesc = false ) = 0;

	virtual void ProcessWatcherGet2Request( 
			FvNetWatcherPacketHandler & packetHandler,
			const char * path,
			FvUInt32 seqNum ) = 0;

	virtual void ProcessWatcherSetRequest( 
			FvNetWatcherPacketHandler & packetHandler,
			const char * path,
			const char * valueString ) = 0;

	virtual void ProcessWatcherSet2Request( 
			FvNetWatcherPacketHandler & packetHandler,
			char* & packet ) = 0;

	virtual void ProcessExtensionMessage( int messageID,
				char * data, int dataLen, const FvNetAddress & addr );
};

class FV_NETWORK_API FvNetWatcherNub
{
public:
	FvNetWatcherNub(FvUInt16 uiMachinePort = FV_NET_PORT_MACHINED);
	~FvNetWatcherNub();

	bool Init( const char * listeningInterface, FvUInt16 listeningPort );

	int RegisterWatcher( int id, const char * abrv, const char * longName,
			const char * listeningInterface = NULL,
			FvUInt16 listeningPort = 0 );
	int DeregisterWatcher();
	int ResetServer();

	void SetRequestHandler( FvNetWatcherRequestHandler * pWatcherRequestHandler );

	int GetSocketDescriptor();
	bool ReceiveRequest();

	FvNetEndpoint & Socket()		{ return m_kSocket; }

	bool AddReply( const char * identifier, const char * desc,
			const char * value );

	bool AddReply2( unsigned int seqNum, FvMemoryOStream & value );
	bool AddDirectoryEntry( FvMemoryOStream & value );

private:
	void NotifyMachineGuard();
	int WatcherControlMessage( int message, bool withid );

	int	m_iID;
	bool m_bRegistered;

	FvNetWatcherRequestHandler *m_pkWRHandler;

	bool m_bInsideReceiveRequest;

	char *m_pcRequestPacket;

	bool m_bIsInitialised;

	FvNetEndpoint m_kSocket;

	FvUInt16 m_uiMachinePort;

	char m_acABRV[32];
	char m_acName[64];
};

class FV_NETWORK_API FvNetStandardWatcherRequestHandler : public FvNetWatcherRequestHandler
{
public:
	FvNetStandardWatcherRequestHandler( FvNetWatcherNub & nub );
	virtual void ProcessWatcherGetRequest( FvNetWatcherPacketHandler
			& packetHandler, const char * path, bool withDesc = false );
	virtual void ProcessWatcherGet2Request( FvNetWatcherPacketHandler 
			& packetHandler, const char * path, FvUInt32 seqNum );
	virtual void ProcessWatcherSetRequest( FvNetWatcherPacketHandler & packetHandler, 
			const char * path, const char * valueString );
	virtual void ProcessWatcherSet2Request( FvNetWatcherPacketHandler & packetHandler, 
			char* & packet );
private:
	FvNetWatcherNub & m_kNub;
};

#endif // __FvNetWatcherNub_H__
