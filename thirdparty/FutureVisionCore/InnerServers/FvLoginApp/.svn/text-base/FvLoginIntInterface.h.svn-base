//{future header message}
#if defined( DEFINE_INTERFACE_HERE ) || defined( DEFINE_SERVER_HERE )
	#undef __FvLoginIntInterface_H__
#endif

#ifndef __FvLoginIntInterface_H__
#define __FvLoginIntInterface_H__

#include "FvLoginInterface.h"
#include <FvAnonymousChannelClient.h>

#define MF_LOGINAPP_INT_MSG( NAME )										\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,									\
		LoginAppStructMessageHandler< LoginIntInterface::NAME##Args >,	\
		&FvLoginApp::NAME )												\

#ifdef FV_SERVER
#include <FvReviverSubject.h>
#else
#define FV_REVIVER_PING_MSG()
#endif

#pragma pack(push,1)
BEGIN_MERCURY_INTERFACE( LoginIntInterface )

	FV_ANONYMOUS_CHANNEL_CLIENT_MSG( DBInterface )

	MERCURY_EMPTY_MESSAGE( ControlledShutDown, &g_kShutDownHandler )

	FV_REVIVER_PING_MSG()

END_MERCURY_INTERFACE()
#pragma pack(pop)

#endif // __FvLoginIntInterface_H__
