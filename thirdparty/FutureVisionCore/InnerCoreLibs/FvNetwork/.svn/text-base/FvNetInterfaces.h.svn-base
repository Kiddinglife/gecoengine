//{future header message}
#ifndef __FvNetInterfaces_H__
#define __FvNetInterfaces_H__

#include "FvNetwork.h"
#include "FvNetMisc.h"

class FV_NETWORK_API FvNetInputMessageHandler
{
public:
	virtual ~FvNetInputMessageHandler() {};

	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data ) = 0;
};

class FV_NETWORK_API FvNetReplyMessageHandler
{
public:
	virtual ~FvNetReplyMessageHandler() {};

	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg ) = 0;

	virtual void HandleException( const FvNetNubException & exception,
		void * arg ) = 0;
};

class FV_NETWORK_API FvNetTimerExpiryHandler
{
public:
	virtual ~FvNetTimerExpiryHandler() {};

	virtual int HandleTimeout( FvNetTimerID id, void * arg ) = 0;
};

class FV_NETWORK_API FvNetInputNotificationHandler
{
public:
	virtual ~FvNetInputNotificationHandler() {};

	virtual int HandleInputNotification( int fd ) = 0;
};

class FV_NETWORK_API FvNetBundleFinishHandler
{
public:
	virtual ~FvNetBundleFinishHandler() {};

	virtual void OnBundleFinished() = 0;
};

class FV_NETWORK_API FvNetBundlePrimer
{
public:
	virtual ~FvNetBundlePrimer() {}

	virtual void PrimeBundle( FvNetBundle & bundle ) = 0;

	virtual int NumUnreliableMessages() const = 0;
};

#endif // __FvNetInterfaces_H__
