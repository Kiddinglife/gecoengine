//{future header message}
#if defined( DEFINE_INTERFACE_HERE ) || defined( DEFINE_SERVER_HERE )
	#undef __FvGlobalAppInterface_H__
#endif

#ifndef __FvGlobalAppInterface_H__
#define __FvGlobalAppInterface_H__

#include <FvNetTypes.h>


#define FV_GLOBAL_APP_LISTENER_MSG( NAME ) 									\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			GlobalAppListenerMessageHandler,								\
			&FvEntityManager::NAME )

#undef FV_GLOBAL_APP_MSG_WITH_ADDR
#define FV_GLOBAL_APP_MSG_WITH_ADDR( NAME )									\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			GlobalAppMessageWithAddrHandler<GlobalAppInterface::NAME##Args>,\
			&FvEntityManager::NAME )

#undef FV_RAW_GLOBAL_APP_MSG
#define FV_RAW_GLOBAL_APP_MSG( NAME )										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			GlobalAppRawMessageHandler,										\
			&FvEntityManager::NAME )

#undef FV_GLOBAL_ENTITY_METHOD_MSG
#define FV_GLOBAL_ENTITY_METHOD_MSG( NAME )									\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			GlobalEntityMethodHandler,										\
			&FvEntity::NAME )

#undef FV_GLOBAL_ENTITY_VARLEN_MSG
#define FV_GLOBAL_ENTITY_VARLEN_MSG( NAME )									\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			GlobalEntityVarLenMessageHandler,								\
			&FvEntity::NAME )

#undef FV_GLOBAL_ENTITY_CALLBACK_EXCEPTION_MSG
#define FV_GLOBAL_ENTITY_CALLBACK_EXCEPTION_MSG( NAME )						\
	BEGIN_HANDLED_PREFIXED_MESSAGE( NAME, FvEntityID,						\
			GlobalEntityCallBackExceptionMessage<GlobalAppInterface::NAME##Args>,\
			&FvEntity::NAME )



#include <FvServerCommon.h>
#include <FvNetInterfaceMinderMacros.h>
#include <FvNetMsgTypes.h>


#pragma pack(push, 1)
BEGIN_MERCURY_INTERFACE( GlobalAppInterface )

	FV_GLOBAL_APP_LISTENER_MSG( HandleCellAppMgrBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleCellAppMgrBirth, x.msg )
	MERCURY_OSTREAM( HandleCellAppMgrBirth, x.msg )

	FV_GLOBAL_APP_MSG_WITH_ADDR( Register )
		bool		bBaseApp;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( Register, x.bBaseApp )
	MERCURY_OSTREAM( Register, x.bBaseApp )

	FV_GLOBAL_APP_MSG_WITH_ADDR( Deregister )
		FvUInt8		dummy;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( Deregister, x.dummy )
	MERCURY_OSTREAM( Deregister, x.dummy )


	FV_RAW_GLOBAL_APP_MSG( PutEntityIDs )
	// ID ids[];

	FV_RAW_GLOBAL_APP_MSG( GetEntityIDs )
	// int numIDs;

	FV_GLOBAL_ENTITY_METHOD_MSG( CallGlobalMethod )		//! ÐèÒª¼ÓEntityID,others->global app

	FV_GLOBAL_ENTITY_VARLEN_MSG( RPCCallBack )

	FV_GLOBAL_ENTITY_CALLBACK_EXCEPTION_MSG( RPCCallBackException )
		FvUInt32	uiCBID;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( RPCCallBackException, x.uiCBID )
	MERCURY_OSTREAM( RPCCallBackException, x.uiCBID )

END_MERCURY_INTERFACE()
#pragma pack(pop)


#endif // __FvGlobalAppInterface_H__
