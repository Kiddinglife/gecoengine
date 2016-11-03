//{future header message}
#ifndef __FvNetWatcherGlue_H__
#define __FvNetWatcherGlue_H__

////#define FV_ENABLE_WATCHERS 1
#if FV_ENABLE_WATCHERS

#include "FvNetwork.h"
#include "FvNetNub.h"
#include "FvNetChannel.h"
#include "FvNetWatcherNub.h"

#include <FvSingleton.h>

class FV_NETWORK_API FvNetWatcherGlue :
	public FvNetWatcherNub,
	public FvNetInputNotificationHandler,
	public FvSingleton< FvNetWatcherGlue >
{
public:
	FvNetWatcherGlue(FvUInt16 uiMachinePort = FV_NET_PORT_MACHINED);
	virtual ~FvNetWatcherGlue();

	virtual int HandleInputNotification( int fd );

private:
	FvNetStandardWatcherRequestHandler m_kHandler;
};

#endif // FV_ENABLE_WATCHERS

#endif // __FvNetWatcherGlue_H__
