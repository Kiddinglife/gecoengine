//{future header message}
#if defined( DEFINE_INTERFACE_HERE ) || defined( DEFINE_SERVER_HERE )
	#undef __FvBaseAppExtInterface_H__
#endif

#ifndef __FvBaseAppExtInterface_H__
#define __FvBaseAppExtInterface_H__

#undef FV_BEGIN_BASE_APP_MSG_WITH_HEADER
#define FV_BEGIN_BASE_APP_MSG_WITH_HEADER( NAME )							\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
		BaseAppMessageWithAddrAndHeaderHandler<BaseAppExtInterface::NAME##Args>,		\
		&FvEntityManager::NAME )													\

#undef FV_RAW_BASE_APP_MSG
#define FV_RAW_BASE_APP_MSG( NAME )											\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			BaseAppRawMessageHandler, &FvEntityManager::NAME )

#undef FV_BEGIN_BLOCKABLE_PROXY_MSG
#define FV_BEGIN_BLOCKABLE_PROXY_MSG( NAME )								\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			ProxyMessageHandler< BaseAppExtInterface::NAME##Args >,			\
			&FvProxy::NAME )													\

#undef FV_BEGIN_UNBLOCKABLE_PROXY_MSG
#define FV_BEGIN_UNBLOCKABLE_PROXY_MSG( NAME )								\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			NoBlockProxyMessageHandler< BaseAppExtInterface::NAME##Args >,	\
			&FvProxy::NAME )													\

#undef FV_VARLEN_BLOCKABLE_PROXY_MSG
#define FV_VARLEN_BLOCKABLE_PROXY_MSG( NAME )								\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2,								\
			ProxyVarLenMessageHandler< true >,								\
			&FvProxy::NAME )

#undef FV_VARLEN_UNBLOCKABLE_PROXY_MSG
#define FV_VARLEN_UNBLOCKABLE_PROXY_MSG( NAME )								\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2,								\
			ProxyVarLenMessageHandler< false >,								\
			&FvProxy::NAME )


#include <FvNetInterfaceMinderMacros.h>
#include <FvNetTypes.h>


#pragma pack( push, 1 )
BEGIN_MERCURY_INTERFACE( BaseAppExtInterface )

	FV_RAW_BASE_APP_MSG( BaseAppLogin )

	FV_BEGIN_BASE_APP_MSG_WITH_HEADER( Authenticate )
		FvSessionKey		key;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( Authenticate, x.key )
	MERCURY_OSTREAM( Authenticate, x.key )

	FV_BEGIN_BLOCKABLE_PROXY_MSG( AvatarUpdateImplicit )
		FvVector3		pos;
		FvDirection3	dir;
		FvUInt8			refNum;
	END_STRUCT_MESSAGE();
	MERCURY_ISTREAM( AvatarUpdateImplicit, x.pos >> x.dir >> x.refNum )
	MERCURY_OSTREAM( AvatarUpdateImplicit, x.pos << x.dir << x.refNum )

	FV_BEGIN_BLOCKABLE_PROXY_MSG( AvatarUpdateExplicit )
		FvSpaceID		spaceID;
		FvEntityID		vehicleID;
		FvCoord			pos;
		FvYawPitchRoll	dir;
		bool			onGround;
		FvUInt8			refNum;
	END_STRUCT_MESSAGE();
	MERCURY_ISTREAM( AvatarUpdateExplicit,
		x.spaceID >> x.vehicleID >> x.pos >> x.dir >> x.onGround >> x.refNum );
	MERCURY_OSTREAM( AvatarUpdateExplicit,
		x.spaceID << x.vehicleID << x.pos << x.dir << x.onGround << x.refNum );

	FV_BEGIN_BLOCKABLE_PROXY_MSG( AvatarUpdateWardImplicit )
		FvEntityID		ward;
		FvCoord			pos;
		FvYawPitchRoll	dir;
	END_STRUCT_MESSAGE();
	MERCURY_ISTREAM( AvatarUpdateWardImplicit, x.ward >> x.pos >> x.dir )
	MERCURY_OSTREAM( AvatarUpdateWardImplicit, x.ward << x.pos << x.dir )

	FV_BEGIN_BLOCKABLE_PROXY_MSG( AvatarUpdateWardExplicit )
		FvEntityID		ward;
		FvSpaceID		spaceID;
		FvEntityID		vehicleID;
		FvCoord			pos;
		FvYawPitchRoll	dir;
		bool			onGround;
	END_STRUCT_MESSAGE();
	MERCURY_ISTREAM( AvatarUpdateWardExplicit,
		x.ward >> x.spaceID >> x.vehicleID >> x.pos >> x.dir >> x.onGround );
	MERCURY_OSTREAM( AvatarUpdateWardExplicit,
		x.ward << x.spaceID << x.vehicleID << x.pos << x.dir << x.onGround );

	FV_BEGIN_BLOCKABLE_PROXY_MSG( AckPhysicsCorrection )
		FvUInt8 dummy;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( AckPhysicsCorrection, x.dummy )
	MERCURY_OSTREAM( AckPhysicsCorrection, x.dummy )

	FV_BEGIN_BLOCKABLE_PROXY_MSG( AckWardPhysicsCorrection )
		FvEntityID ward;
		FvUInt8 dummy;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( AckWardPhysicsCorrection, x.ward >> x.dummy )
	MERCURY_OSTREAM( AckWardPhysicsCorrection, x.ward << x.dummy )

	FV_VARLEN_BLOCKABLE_PROXY_MSG( RequestEntityUpdate )
	//	FvEntityID		id;
	//	EventNumber[]	lastEventNumbers;

	FV_BEGIN_BLOCKABLE_PROXY_MSG( EnableEntities )
		FvUInt8			dummy;
	END_STRUCT_MESSAGE();
	MERCURY_ISTREAM( EnableEntities, x.dummy )
	MERCURY_OSTREAM( EnableEntities, x.dummy )

	FV_BEGIN_UNBLOCKABLE_PROXY_MSG( RestoreClientAck )
		int				id;
	END_STRUCT_MESSAGE();
	MERCURY_ISTREAM( RestoreClientAck, x.id )
	MERCURY_OSTREAM( RestoreClientAck, x.id )

	FV_BEGIN_UNBLOCKABLE_PROXY_MSG( DisconnectClient )
		FvUInt8			reason;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( DisconnectClient, x.reason )
	MERCURY_OSTREAM( DisconnectClient, x.reason )

	FV_VARLEN_BLOCKABLE_PROXY_MSG( GMCmd )

	MERCURY_VARIABLE_MESSAGE( EntityMessage, 2, NULL )

END_MERCURY_INTERFACE()
#pragma pack( pop )

#endif // __FvBaseAppExtInterface_H__
