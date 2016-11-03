//{future header message}
#ifndef __FvAnonymousChannelClient_H__
#define __FvAnonymousChannelClient_H__

#include <FvNetChannel.h>
#include <FvNetInterfaceMinder.h>

#include "FvServerCommon.h"

class FV_SERVERCOMMON_API FvAnonymousChannelClient : public FvNetInputMessageHandler
{
public:
	FvAnonymousChannelClient();
	~FvAnonymousChannelClient();

	bool Init( FvNetNub & nub,
		FvNetInterfaceMinder & interfaceMinder,
		const FvNetInterfaceElement & birthMessage,
		const char * componentName,
		int numRetries );

	FvNetChannelOwner *GetChannelOwner() const	{ return m_pkChannelOwner; }

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	FvNetChannelOwner *m_pkChannelOwner;
	FvString m_kInterfaceName;
};

#define FV_INIT_ANONYMOUS_CHANNEL_CLIENT( INSTANCE, NUB,					\
		CLIENT_INTERFACE, SERVER_INTERFACE, NUM_RETRIES )					\
	INSTANCE.Init( NUB,														\
		CLIENT_INTERFACE::gMinder,											\
		CLIENT_INTERFACE::SERVER_INTERFACE##Birth,							\
		#SERVER_INTERFACE,													\
		NUM_RETRIES )


#define FV_ANONYMOUS_CHANNEL_CLIENT_MSG( SERVER_INTERFACE )					\
	MERCURY_FIXED_MESSAGE( SERVER_INTERFACE##Birth,							\
		sizeof( InterfaceListenerMsg ),										\
		NULL /* Handler set by FV_INIT_ANONYMOUS_CHANNEL_CLIENT */ )

#endif // __FvAnonymousChannelClient_H__
