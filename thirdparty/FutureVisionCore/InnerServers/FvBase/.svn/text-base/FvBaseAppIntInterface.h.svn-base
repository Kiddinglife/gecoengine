//{future header message}
#if defined( DEFINE_INTERFACE_HERE ) || defined( DEFINE_SERVER_HERE )
	#undef __FvBaseAppIntInterface_H__
#endif

#ifndef __FvBaseAppIntInterface_H__
#define __FvBaseAppIntInterface_H__

#include <FvNetTypes.h>
#include <FvAnonymousChannelClient.h>


#undef FV_BEGIN_BASE_APP_MSG
#define FV_BEGIN_BASE_APP_MSG( NAME )										\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			BaseAppMessageHandler< BaseAppIntInterface::NAME##Args >,		\
			&FvEntityManager::NAME )										\

#undef FV_BEGIN_BASE_APP_MSG_WITH_ADDR
#define FV_BEGIN_BASE_APP_MSG_WITH_ADDR( NAME )								\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
		BaseAppMessageWithAddrHandler< BaseAppIntInterface::NAME##Args >,	\
		&FvEntityManager::NAME )													\

#undef FV_BEGIN_PROXY_MSG
#define FV_BEGIN_PROXY_MSG( NAME )											\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			NoBlockProxyMessageHandler< BaseAppIntInterface::NAME##Args >,	\
			&FvProxy::NAME )													\

#undef FV_BEGIN_BASE_MSG
#define FV_BEGIN_BASE_MSG( NAME )											\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			BaseMessageHandler< BaseAppIntInterface::NAME##Args >,			\
			&FvEntity::NAME )													\

#undef FV_BEGIN_BASE_MSG_WITH_ADDR
#define FV_BEGIN_BASE_MSG_WITH_ADDR( NAME )									\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			BaseMessageWithAddrHandler< BaseAppIntInterface::NAME##Args >,	\
			&FvEntity::NAME )													\


#define FV_VARLEN_BASE_APP_MSG( NAME ) 										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			BaseAppVarLenMessageHandler, &FvEntityManager::NAME )

#undef FV_RAW_BASE_APP_MSG
#define FV_RAW_BASE_APP_MSG( NAME )											\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			BaseAppRawMessageHandler, &FvEntityManager::NAME )

#define FV_BIG_RAW_BASE_APP_MSG( NAME )										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 4, 								\
			BaseAppRawMessageHandler, &FvEntityManager::NAME )

#define FV_VARLEN_BASE_MSG( NAME ) 											\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			BaseVarLenMessageHandler, &FvEntity::NAME )

#define FV_RAW_BASE_MSG( NAME ) 											\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			RawBaseMessageHandler, &FvEntity::NAME )

#define FV_RAW_BASE_METHOD_MSG( NAME ) 										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			MethodHandler, &FvEntity::NAME )

#define FV_VARLEN_PROXY_MSG( NAME ) 										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			ProxyVarLenMessageHandler<false>, &FvProxy::NAME )

#define FV_RAW_PROXY_MSG( NAME ) 											\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			RawProxyMessageHandler, &FvProxy::NAME )

#define FV_BASE_APP_LISTENER_MSG( NAME ) 									\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			BaseAppListenerMessageHandler,									\
			&FvEntityManager::NAME )

#define FV_VARLEN_BASE_ENTITY_MSG( NAME ) 									\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			BaseEntityVarLenMessageHandler, &FvEntity::NAME )

#define FV_BEGIN_BASE_ENTITY_CALLBACK_EXCEPTION_MSG( NAME )					\
	BEGIN_HANDLED_PREFIXED_MESSAGE( NAME, FvEntityID,						\
			BaseEntityCallBackExceptionHandler< BaseAppIntInterface::NAME##Args >,\
			&FvEntity::NAME )


#include <FvServerCommon.h>
#include <FvNetInterfaceMinderMacros.h>
#include <FvNetMsgTypes.h>


#pragma pack(push, 1)
BEGIN_MERCURY_INTERFACE( BaseAppIntInterface )

	FV_ANONYMOUS_CHANNEL_CLIENT_MSG( DBInterface )

	FV_RAW_BASE_APP_MSG( CreateBaseWithCellData )

	FV_RAW_BASE_APP_MSG( CreateBaseFromDB )

	FV_RAW_BASE_APP_MSG( LogOnAttempt )

	FV_VARLEN_BASE_APP_MSG( AddGlobalBase )
	FV_VARLEN_BASE_APP_MSG( DelGlobalBase )

	FV_VARLEN_BASE_APP_MSG( AddGlobalMailBox )
	FV_VARLEN_BASE_APP_MSG( DelGlobalMailBox )

	FV_RAW_BASE_APP_MSG( RunScript )

	FV_BASE_APP_LISTENER_MSG( HandleCellAppMgrBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleCellAppMgrBirth, x.msg )
	MERCURY_OSTREAM( HandleCellAppMgrBirth, x.msg )

	FV_BASE_APP_LISTENER_MSG( HandleBaseAppMgrBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleBaseAppMgrBirth, x.msg )
	MERCURY_OSTREAM( HandleBaseAppMgrBirth, x.msg )

	FV_BASE_APP_LISTENER_MSG( HandleGlobalAppBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleGlobalAppBirth, x.msg )
	MERCURY_OSTREAM( HandleGlobalAppBirth, x.msg )

	FV_VARLEN_BASE_APP_MSG( HandleCellAppDeath )

	FV_BEGIN_BASE_APP_MSG( Startup )
		bool				bootstrap;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( Startup, x.bootstrap )
	MERCURY_OSTREAM( Startup, x.bootstrap )

	FV_BEGIN_BASE_APP_MSG( ShutDown )
		bool	isSigInt;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ShutDown, x.isSigInt )
	MERCURY_OSTREAM( ShutDown, x.isSigInt )

	FV_RAW_BASE_APP_MSG( ControlledShutDown )

	FV_VARLEN_BASE_APP_MSG( SetCreateBaseInfo )

	FV_BEGIN_BASE_APP_MSG( OldSetBackupBaseApp )
		FvNetAddress	addr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( OldSetBackupBaseApp, x.addr )
	MERCURY_OSTREAM( OldSetBackupBaseApp, x.addr )

	FV_BEGIN_BASE_APP_MSG( OldStartBaseAppBackup )
		FvNetAddress	addr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( OldStartBaseAppBackup, x.addr )
	MERCURY_OSTREAM( OldStartBaseAppBackup, x.addr )

	FV_BEGIN_BASE_APP_MSG( OldStopBaseAppBackup )
		FvNetAddress	addr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( OldStopBaseAppBackup, x.addr )
	MERCURY_OSTREAM( OldStopBaseAppBackup, x.addr )

	FV_BIG_RAW_BASE_APP_MSG( OldBackupBaseEntities )

	FV_BEGIN_BASE_APP_MSG( OldBackupHeartbeat )
		FvNetAddress	addr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( OldBackupHeartbeat, x.addr )
	MERCURY_OSTREAM( OldBackupHeartbeat, x.addr )

	FV_BEGIN_BASE_APP_MSG( OldRestoreBaseApp )
		FvNetAddress	intAddr;
		FvNetAddress	extAddr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( OldRestoreBaseApp, x.intAddr >> x.extAddr )
	MERCURY_OSTREAM( OldRestoreBaseApp, x.intAddr << x.extAddr )

	FV_BEGIN_BASE_APP_MSG( StartBaseEntitiesBackup )
		FvNetAddress	realBaseAppAddr;
		FvUInt32				index;
		FvUInt32				hashSize;
		FvUInt32				prime;
		bool				isInitial;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( StartBaseEntitiesBackup, x.realBaseAppAddr >> x.index >> x.hashSize >> x.prime >> x.isInitial )
	MERCURY_OSTREAM( StartBaseEntitiesBackup, x.realBaseAppAddr << x.index << x.hashSize << x.prime << x.isInitial )

	FV_BEGIN_BASE_APP_MSG( StopBaseEntitiesBackup )
		FvNetAddress	realBaseAppAddr;
		FvUInt32				index;
		FvUInt32				hashSize;
		FvUInt32				prime;
		bool				isPending;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( StopBaseEntitiesBackup, x.realBaseAppAddr >> x.index >> x.hashSize >> x.prime >> x.isPending )
	MERCURY_OSTREAM( StopBaseEntitiesBackup, x.realBaseAppAddr << x.index << x.hashSize << x.prime << x.isPending )

	FV_BIG_RAW_BASE_APP_MSG( BackupBaseEntity )

	FV_BEGIN_BASE_APP_MSG_WITH_ADDR( StopBaseEntityBackup )
		FvEntityID entityID;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( StopBaseEntityBackup, x.entityID )
	MERCURY_OSTREAM( StopBaseEntityBackup, x.entityID )

	FV_RAW_BASE_APP_MSG( HandleBaseAppDeath )

	FV_RAW_BASE_APP_MSG( SetBackupBaseApps )

	FV_VARLEN_BASE_APP_MSG( SetSharedData )

	FV_VARLEN_BASE_APP_MSG( DelSharedData )

	FV_BEGIN_BASE_APP_MSG( SetClient )					//! 不使用
		FvEntityID			id;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( SetClient, x.id )
	MERCURY_OSTREAM( SetClient, x.id )

	FV_RAW_BASE_MSG( CurrentCell )						//! 需要加EntityID,cellappMgr/cell->base
	//	FvSpaceID			newSpaceID;
	//	FvNetAddress		newCellAddr;

	FV_RAW_BASE_APP_MSG( EmergencySetCurrentCell )		//! 不使用
	//	FvSpaceID				newSpaceID;
	//	FvNetAddress	newCellAddr;

	FV_BEGIN_PROXY_MSG( SendToClient )					//! 不使用
		FvUInt8				dummy; 
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( SendToClient, x.dummy )
	MERCURY_OSTREAM( SendToClient, x.dummy )

	FV_BEGIN_PROXY_MSG( TickSync )						//! 经idx发送,cell->base->client
		FvUInt8				tickByte;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( TickSync, x.tickByte )
	MERCURY_OSTREAM( TickSync, x.tickByte )

	FV_VARLEN_PROXY_MSG( CreateCellPlayer )				//! 经idx发送,cell->base->client

	FV_VARLEN_PROXY_MSG( SpaceData )					//! 经idx发送,cell->base->client
	//	FvEntityID		spaceID
	//	SpaceEntryID	entryID
	//	uint16			key;
	//	char[]			value;	

	FV_BEGIN_PROXY_MSG( EnterAoI )						//! 经idx发送,cell->base->client
		FvEntityID			id;
		FvIDAlias				idAlias;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( EnterAoI, x.id >> x.idAlias )
	MERCURY_OSTREAM( EnterAoI, x.id << x.idAlias )

	FV_BEGIN_PROXY_MSG( EnterAoIOnVehicle )				//! 经idx发送,cell->base->client
		FvEntityID			id;
		FvEntityID			vehicleID;
		FvIDAlias				idAlias;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( EnterAoIOnVehicle, x.id >> x.vehicleID >> x.idAlias )
	MERCURY_OSTREAM( EnterAoIOnVehicle, x.id << x.vehicleID << x.idAlias )

	FV_VARLEN_PROXY_MSG( LeaveAoI )						//! 经idx发送,cell->base->client
	//	FvEntityID		id;
	//	EventNumber[]	lastEventNumbers;

	FV_BEGIN_PROXY_MSG( SetVehicle )					//! 经idx发送,cell->base->client
		FvEntityID			passengerID;
		FvEntityID			vehicleID;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( SetVehicle, x.passengerID >> x.vehicleID )
	MERCURY_OSTREAM( SetVehicle, x.passengerID << x.vehicleID )

	FV_VARLEN_PROXY_MSG( CreateEntity )					//! 经idx发送,cell->base->client

	FV_VARLEN_PROXY_MSG( UpdateEntity )					//! 经idx发送,cell->base->client

	FV_BEGIN_PROXY_MSG( DetailedPosition )				//! 经idx发送,cell->base->client
		FvEntityID		id;
		FvPosition3D		position;
		FvDirection3		direction;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( DetailedPosition, x.id >> x.position >> x.direction )
	MERCURY_OSTREAM( DetailedPosition, x.id << x.position << x.direction )


	FV_BEGIN_PROXY_MSG( ForcedPosition )				//! 经idx发送,cell->base->client
		FvEntityID		id;
		FvSpaceID			spaceID;
		FvEntityID		vehicleID;
		FvPosition3D		position;
		FvDirection3		direction;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ForcedPosition, x.id >> x.spaceID >> x.vehicleID >> x.position >> x.direction )
	MERCURY_OSTREAM( ForcedPosition, x.id << x.spaceID << x.vehicleID << x.position << x.direction )

	FV_BEGIN_PROXY_MSG( ModWard )						//! 经idx发送,cell->base->client
		FvEntityID		id;
		bool			on;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ModWard, x.id >> x.on )
	MERCURY_OSTREAM( ModWard, x.id << x.on )


	FV_RAW_PROXY_MSG( CallClientMethod )				//! 需要加EntityID,others->base->client

	FV_VARLEN_BASE_MSG( BackupCellEntity )				//! 经idx发送,cell->base

	FV_VARLEN_BASE_MSG( WriteToDB )						//! 经idx发送,cell->base
	FV_VARLEN_BASE_MSG( CellEntityLost )				//! 经idx发送,cell->base

	FV_BEGIN_BASE_MSG( StartKeepAlive )					//! 不使用
		FvUInt32			interval;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( StartKeepAlive, x.interval )
	MERCURY_OSTREAM( StartKeepAlive, x.interval )

	FV_RAW_BASE_METHOD_MSG( CallBaseMethod )			//! 需要加EntityID,others->base

	FV_RAW_BASE_METHOD_MSG( CallCellMethod )			//! 需要加EntityID,others->base

	FV_VARLEN_BASE_ENTITY_MSG( RPCCallBack )

	FV_BEGIN_BASE_ENTITY_CALLBACK_EXCEPTION_MSG( RPCCallBackException )
		FvUInt32	uiCBID;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( RPCCallBackException, x.uiCBID )
	MERCURY_OSTREAM( RPCCallBackException, x.uiCBID )

	MERCURY_VARIABLE_MESSAGE( EntityMessage, 2, NULL )

	FV_RAW_BASE_APP_MSG( CallWatcher )

END_MERCURY_INTERFACE()
#pragma pack(pop)

#endif // __FvBaseAppIntInterface_H__
