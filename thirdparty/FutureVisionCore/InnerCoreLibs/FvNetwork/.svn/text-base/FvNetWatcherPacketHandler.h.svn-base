//{future header message}
#ifndef __FvNetWatcherPacketHandler_H__
#define __FvNetWatcherPacketHandler_H__

////#define FV_ENABLE_WATCHERS 1

#if FV_ENABLE_WATCHERS

#include "FvNetEndpoint.h"
#include "FvNetWatcherNub.h"

#include <FvWatcher.h>
#include <FvWatcherPathRequest.h>

class FvWatcherPathRequest;
typedef std::vector<FvWatcherPathRequest *> FvPathRequestList;

class FvNetWatcherPacketHandler : public FvWatcherPathRequestNotification
{
public:

	enum WatcherProtocolVersion
	{
		WP_VERSION_UNKNOWN = 0, 
		WP_VERSION_1,
		WP_VERSION_2 
	};


	FvNetWatcherPacketHandler( FvNetEndpoint & socket, const sockaddr_in & addr,
		FvInt32 numPaths, WatcherProtocolVersion version, bool isSet = false );
	virtual ~FvNetWatcherPacketHandler();

	void Run();
	void CheckSatisfied();
	void SendReply();
	void DoNotDelete( bool shouldNotDelete );

	FvWatcherPathRequest *NewRequest( FvString & path );
	virtual void NotifyComplete( FvWatcherPathRequest & pathRequest,
		FvInt32 count );

private:

	static const FvString v1ErrorIdentifier;
	static const FvString v1ErrorPacketLimit;

	bool m_bCanDelete;

	FvNetEndpoint &m_kEndpoint;
	sockaddr_in m_kDestAddr;

	WatcherProtocolVersion m_kVersion;
	bool m_bIsSet;

	FvInt32 m_iOutgoingRequests;
	FvInt32 m_iAnsweredRequests;
	bool m_bIsExecuting;

	FvMemoryOStream m_kPacket;
	FvPathRequestList m_kPathRequestList;

	int m_iMaxPacketSize;
	bool m_bReachedPacketLimit;
};

#endif // FV_ENABLE_WATCHERS

#endif // __FvNetWatcherPacketHandler_H__
