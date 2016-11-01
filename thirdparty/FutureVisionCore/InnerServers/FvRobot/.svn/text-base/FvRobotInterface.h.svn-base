//{future header message}
#if defined( DEFINE_INTERFACE_HERE ) || defined( DEFINE_SERVER_HERE )
	#undef __FvRobotInterface_H__
#endif

#ifndef __FvRobotInterface_H__
#define __FvRobotInterface_H__

#include "FvNetInterfaceMinderMacros.h"

#define MF_BOTS_MSG( NAME )												\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,									\
		BotsStructMessageHandler< BotsInterface::NAME##Args >,			\
		&FvRobotMainApp::NAME )												\


#pragma pack(push,1)
BEGIN_MERCURY_INTERFACE( BotsInterface )
	
END_MERCURY_INTERFACE()
#pragma pack(pop)

#endif//! __FvRobotInterface_H__
