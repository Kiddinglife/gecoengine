//{future header message}
#if defined(DEFINE_INTERFACE_HERE) || defined(DEFINE_SERVER_HERE)
	#undef __FvCellAppInterface_H__
	#define SECOND_PASS
#endif


#ifndef __FvCellAppInterface_H__
#define __FvCellAppInterface_H__


#ifdef DEFINE_SERVER_HERE
#define CELL_TYPEDEF_MACRO( TYPE, NAME )									\
	typedef MessageHandler< TYPE, CellAppInterface::NAME##Args >			\
													TYPE##_##NAME##_Handler;
#else
#define CELL_TYPEDEF_MACRO( TYPE, NAME )
#endif

#include <FvNetInterfaceMinderMacros.h>
#include <FvEntityDescription.h>
#include <FvServerCommon.h>
#include <FvAnonymousChannelClient.h>
#include <FvNetInterfaceMinder.h>
#include <FvNetMsgTypes.h>


#ifndef SECOND_PASS
enum EntityReality
{
	GHOST_ONLY,
	REAL_ONLY,
	WITNESS_ONLY
};
#endif


#define FV_BEGIN_CELL_APP_MSG( NAME )										\
	CELL_TYPEDEF_MACRO( FvEntityManager, NAME )								\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			FvEntityManager_##NAME##_Handler,								\
			&FvEntityManager::NAME )

#define FV_VARLEN_CELL_APP_MSG( NAME ) 										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2,								\
			VarLenMessageHandler< FvEntityManager >,						\
			&FvEntityManager::NAME )

#define FV_RAW_CELL_APP_MSG( NAME ) 										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, RawMessageHandler<FvEntityManager>,\
			&FvEntityManager::NAME )

#define FV_RAW_CELL_APP_SIGNAL( NAME ) 										\
	HANDLER_STATEMENT( NAME, RawMessageHandler<FvEntityManager>, &FvEntityManager::NAME )	\
	MERCURY_EMPTY_MESSAGE( NAME, HANDLER_ARGUMENT( NAME ) )					\


#define FV_BEGIN_SPACE_MSG( NAME ) 											\
	CELL_TYPEDEF_MACRO( FvSpace, NAME )										\
	BEGIN_HANDLED_PREFIXED_MESSAGE( NAME,									\
			FvSpaceID,														\
			FvSpace_##NAME##_Handler,											\
			&FvSpace::NAME )													\

#define FV_VARLEN_SPACE_MSG( NAME ) 										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2,								\
			VarLenMessageHandler< FvSpace >,									\
			&FvSpace::NAME )

#define FV_BEGIN_CELL_MSG( NAME ) 											\
	CELL_TYPEDEF_MACRO( FvCell, NAME )										\
	BEGIN_HANDLED_PREFIXED_MESSAGE( NAME,									\
			FvSpaceID,														\
			FvCell_##NAME##_Handler,											\
			&FvCell::NAME )													\

#define FV_VARLEN_CELL_MSG( NAME ) 											\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, VarLenMessageHandler< FvCell >,\
			&FvCell::NAME )

#define FV_RAW_CELL_MSG( NAME ) 											\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, RawMessageHandler<FvCell>,	\
			&FvCell::NAME )

#define FV_VARLEN_SPACE_BY_ENTITY_MSG( NAME, METHOD_NAME ) 					\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2,								\
			SpaceVarLenByEntityMessageHandler, &FvSpace::METHOD_NAME )

#define FV_BEGIN_ENTITY_MSG( NAME, IS_REAL_ONLY ) 							\
	BEGIN_HANDLED_PREFIXED_MESSAGE( NAME, FvEntityID,						\
			CellEntityMessageHandler< CellAppInterface::NAME##Args >, 		\
			std::make_pair( &FvEntity::NAME, IS_REAL_ONLY ) )

#define FV_VARLEN_ENTITY_MSG( NAME, IS_REAL_ONLY )							\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, EntityVarLenMessageHandler,	\
			std::make_pair( &FvEntity::NAME, IS_REAL_ONLY) )

#define FV_VARLEN_ENTITY_METHOD_MSG( NAME, IS_REAL_ONLY )					\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, EntityVarLenMethodHandler,	\
			std::make_pair( &FvEntity::NAME, IS_REAL_ONLY) )

#define FV_RAW_VARLEN_ENTITY_MSG( NAME, IS_REAL_ONLY )						\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			RawEntityVarLenMessageHandler,									\
			std::make_pair( &FvEntity::NAME, IS_REAL_ONLY) )

#define FV_VARLEN_ENTITY_REQUEST( NAME, IS_REAL_ONLY )						\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, EntityVarLenRequestHandler,	\
			std::make_pair( &FvEntity::NAME, IS_REAL_ONLY) )

#define FV_CELL_APP_LISTENER_MSG( NAME ) 									\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			CellAppListenerMessageHandler,									\
			&FvEntityManager::NAME )

#define FV_VEHICLE_MSG( NAME )												\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
								 VehicleMessageHandler, &FvEntity::NAME )

#define FV_CELL_ENTITY_CALLBACK_MSG( NAME, IS_REAL_ONLY )					\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, 								\
			EntityCallBackHandler,											\
			std::make_pair( &FvEntity::NAME, IS_REAL_ONLY) )

#define FV_BEGIN_CELL_ENTITY_CALLBACK_EXCEPTION_MSG( NAME, IS_REAL_ONLY ) 	\
	BEGIN_HANDLED_PREFIXED_MESSAGE( NAME, FvEntityID,						\
			EntityCallBackExceptionHandler< CellAppInterface::NAME##Args >, \
			std::make_pair( &FvEntity::NAME, IS_REAL_ONLY ) )

#pragma pack( push, 1 )
BEGIN_MERCURY_INTERFACE( CellAppInterface )

	FV_ANONYMOUS_CHANNEL_CLIENT_MSG( DBInterface )

	FV_VARLEN_CELL_APP_MSG( AddCell )
		// FvSpaceID spaceID;

	FV_BEGIN_CELL_APP_MSG( Startup )
		FvNetAddress baseAppAddr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( Startup, x.baseAppAddr )
	MERCURY_OSTREAM( Startup, x.baseAppAddr )

	FV_BEGIN_CELL_APP_MSG( SetGameTime )
		FvTimeStamp		gameTime;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( SetGameTime, x.gameTime )
	MERCURY_OSTREAM( SetGameTime, x.gameTime )

	FV_CELL_APP_LISTENER_MSG( HandleCellAppMgrBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleCellAppMgrBirth, x.msg )
	MERCURY_OSTREAM( HandleCellAppMgrBirth, x.msg )

	FV_CELL_APP_LISTENER_MSG( HandleGlobalAppBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleGlobalAppBirth, x.msg )
	MERCURY_OSTREAM( HandleGlobalAppBirth, x.msg )

	FV_BEGIN_CELL_APP_MSG( HandleCellAppDeath )
		FvNetAddress addr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleCellAppDeath, x.addr )
	MERCURY_OSTREAM( HandleCellAppDeath, x.addr )

	FV_VARLEN_CELL_APP_MSG( HandleBaseAppDeath )

	FV_BEGIN_CELL_APP_MSG( ShutDown )
		bool isSigInt;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ShutDown, x.isSigInt )
	MERCURY_OSTREAM( ShutDown, x.isSigInt )

	FV_BEGIN_CELL_APP_MSG( ControlledShutDown )
		FvShutDownStage	stage;
		FvTimeStamp		shutDownTime;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ControlledShutDown, x.stage >> x.shutDownTime )
	MERCURY_OSTREAM( ControlledShutDown, x.stage << x.shutDownTime )

	FV_VARLEN_CELL_APP_MSG( CreateSpaceIfNecessary )				//! 不使用
	//	FvSpaceID			spaceID
	//	FvInt32			count
	//	count of:
	//	FvSpaceEntryID	entryID
	//	uint16			key;
	//	string			value;

	FV_VARLEN_CELL_APP_MSG( RunScript );

	FV_VARLEN_CELL_APP_MSG( SetSharedData );
	FV_VARLEN_CELL_APP_MSG( DelSharedData );

	FV_VARLEN_CELL_APP_MSG( AddGlobalBase )
	FV_VARLEN_CELL_APP_MSG( DelGlobalBase )

	FV_VARLEN_CELL_APP_MSG( AddGlobalMailBox )
	FV_VARLEN_CELL_APP_MSG( DelGlobalMailBox )

	FV_BEGIN_CELL_APP_MSG( SetBaseApp )
		FvNetAddress baseAppAddr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( SetBaseApp, x.baseAppAddr )
	MERCURY_OSTREAM( SetBaseApp, x.baseAppAddr )

	FV_RAW_CELL_APP_MSG( OnloadTeleportedEntity )

	// The arguments are as follows:
	//  FvEntityID			The id of the entity
	//  FvVector3			The position of the entity
	//  FvEntityTypeID		The type of the entity
	//  FvNetAddress	The address of the entity's owner
	//  Variable script state data
	FV_VARLEN_SPACE_MSG( CreateGhost )								//! 需要spaceID

	FV_VARLEN_SPACE_MSG( SpaceData )								//! 需要spaceID
	//	FvSpaceEntryID	entryID
	//	uint16			key;
	//	char[]			value;		// rest of message

	FV_VARLEN_SPACE_MSG( AllSpaceData )								//! 需要spaceID
	//	int				numEntries;
	//	numEntries of:
	//	FvSpaceEntryID	entryID;
	//	uint16			key;
	//	FvString		value;

	FV_VARLEN_SPACE_MSG( UpdateGeometry )							//! 需要spaceID

	#define SPACE_GEOMETRY_LOADED_BOOTSTRAP_FLAG	0x01
	FV_VARLEN_SPACE_MSG( SpaceGeometryLoaded )						//! 需要spaceID
	//	FvUInt8 flags;
	//	FvString lastGeometryPath

	FV_BEGIN_SPACE_MSG( ShutDownSpace )								//! 需要spaceID
		FvUInt8 info;	// Not used yet.
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ShutDownSpace, x.info )
	MERCURY_OSTREAM( ShutDownSpace, x.info )


	// FvEntity creation
	// The arguments are as follows:
	//	ChannelVersion 	The channel version
	//	bool			IsRestore flag
	// 	FvEntityID		The id for the new entity
	// 	FvVector3		The position of the new entity
	//	bool			IsOnGround flag
	// 	FvEntityTypeID	The type for the new entity
	// 	Variable script initialisation data
	// 	Variable real entity data
	FV_VARLEN_SPACE_MSG( CreateEntity )								//! 需要spaceID
	FV_VARLEN_SPACE_BY_ENTITY_MSG( CreateEntityNearEntity, CreateEntity )//! 需要EntityID

	FV_BEGIN_CELL_MSG( ShouldOffload )								//! 需要spaceID
		bool enable;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ShouldOffload, x.enable )
	MERCURY_OSTREAM( ShouldOffload, x.enable )

	FV_BEGIN_CELL_MSG( SetRetiringCell )							//! 需要spaceID
		bool isRetiring;
		bool isRemoved;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( SetRetiringCell, x.isRetiring >> x.isRemoved )
	MERCURY_OSTREAM( SetRetiringCell, x.isRetiring << x.isRemoved )

	FV_BEGIN_ENTITY_MSG( AvatarUpdateImplicit, REAL_ONLY )			//! 需要EntityID,经idx发送
		FvVector3		pos;
		FvDirection3	dir;
		FvUInt8			refNum;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( AvatarUpdateImplicit, x.pos >> x.dir >> x.refNum )
	MERCURY_OSTREAM( AvatarUpdateImplicit, x.pos << x.dir << x.refNum )

	// Brisk-track avatar update
	FV_BEGIN_ENTITY_MSG( AvatarUpdateExplicit, REAL_ONLY )			//! 需要EntityID,经idx发送
		FvSpaceID		spaceID;
		FvEntityID		vehicleID;
		FvCoord			pos;
		FvYawPitchRoll	dir;
		bool			onGround;
		FvUInt8			refNum;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( AvatarUpdateExplicit, x.spaceID >> x.vehicleID >> x.pos >> x.dir >> x.onGround >> x.refNum )
	MERCURY_OSTREAM( AvatarUpdateExplicit, x.spaceID << x.vehicleID << x.pos << x.dir << x.onGround << x.refNum )

	FV_BEGIN_ENTITY_MSG( AckPhysicsCorrection, REAL_ONLY )			//! 需要EntityID,经idx发送
		FvUInt8		dummy;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( AckPhysicsCorrection, x.dummy)
	MERCURY_OSTREAM( AckPhysicsCorrection, x.dummy)

	FV_VARLEN_ENTITY_MSG( EnableWitness, REAL_ONLY )				//! 需要EntityID,经idx发送

	FV_BEGIN_ENTITY_MSG( WitnessCapacity, WITNESS_ONLY )			//! 需要EntityID,经idx发送
		FvEntityID			witness;
		FvUInt32			bps;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( WitnessCapacity, x.witness >> x.bps )
	MERCURY_OSTREAM( WitnessCapacity, x.witness << x.bps )

	// requestEntityUpdate:
	//	FvEntityID	id;
	//	Array of event numbers;
	FV_VARLEN_ENTITY_MSG( RequestEntityUpdate, WITNESS_ONLY )		//! 需要EntityID,经idx发送

	FV_VEHICLE_MSG( VehicleMsg )									//! 需要EntityID

	// This is used by ghost entities to inform the real entity that it is being
	// witnessed.
	FV_BEGIN_ENTITY_MSG( Witnessed, REAL_ONLY )						//! 需要EntityID,经idx发送
		FvUInt16			cellIdx;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( Witnessed, x.cellIdx )
	MERCURY_OSTREAM( Witnessed, x.cellIdx )

	FV_VARLEN_ENTITY_REQUEST( WriteToDBRequest, REAL_ONLY )			//! 需要EntityID,经idx发送

	FV_BEGIN_ENTITY_MSG( DestroyEntity, REAL_ONLY )					//! 需要EntityID,经idx发送
		FvInt32				flags; // Currently not used.
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( DestroyEntity, x.flags )
	MERCURY_OSTREAM( DestroyEntity, x.flags )

	// Destined for ghost entity only
	FV_RAW_VARLEN_ENTITY_MSG( Onload, GHOST_ONLY )					//! 需要EntityID

	FV_BEGIN_ENTITY_MSG( GhostAvatarUpdate, GHOST_ONLY )			//! 需要EntityID
		FvVector3			pos;
		FvDirection3		dir;
		bool				isOnGround;
		FvVolatileNumber	updateNumber;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( GhostAvatarUpdate, x.pos >> x.dir >> x.isOnGround >> x.updateNumber )
	MERCURY_OSTREAM( GhostAvatarUpdate, x.pos << x.dir << x.isOnGround << x.updateNumber )

	FV_VARLEN_ENTITY_MSG( GhostHistoryEvent, GHOST_ONLY )			//! 需要EntityID

	FV_BEGIN_ENTITY_MSG( GhostSetReal, GHOST_ONLY )					//! 需要EntityID
		FvNetAddress	owner;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( GhostSetReal, x.owner )
	MERCURY_OSTREAM( GhostSetReal, x.owner )

	FV_BEGIN_ENTITY_MSG( GhostSetNextReal, GHOST_ONLY )				//! 需要EntityID
		FvNetAddress	nextRealAddr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( GhostSetNextReal, x.nextRealAddr )
	MERCURY_OSTREAM( GhostSetNextReal, x.nextRealAddr )

	FV_BEGIN_ENTITY_MSG( DelGhost, GHOST_ONLY )						//! 需要EntityID
		FvUInt8			dummy;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( DelGhost, x.dummy )
	MERCURY_OSTREAM( DelGhost, x.dummy )

	FV_BEGIN_ENTITY_MSG( GhostVolatileInfo, GHOST_ONLY )			//! 需要EntityID
		FvVolatileInfo	volatileInfo;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( GhostVolatileInfo, x.volatileInfo )
	MERCURY_OSTREAM( GhostVolatileInfo, x.volatileInfo )

	FV_VARLEN_ENTITY_MSG( GhostControllerExist, GHOST_ONLY )		//! 需要EntityID
	FV_VARLEN_ENTITY_MSG( GhostControllerUpdate, GHOST_ONLY )		//! 需要EntityID

	// for non-OtherClient data, see ghostOtherClientDataUpdate below
	FV_VARLEN_ENTITY_MSG( GhostedDataUpdate, GHOST_ONLY )			//! 需要EntityID
		// FvEventNumber (FvInt32) eventNumber
		// data for ghostDataUpdate

	// for OtherClient data
	FV_VARLEN_ENTITY_MSG( GhostedOtherClientDataUpdate, GHOST_ONLY )//! 需要EntityID
		// FvEventNumber (FvInt32) eventNumber
		// data for ghostOtherClientDataUpdate

	// The real entity uses this to query whether there are any entities
	// witnessing its ghost entities.
	FV_BEGIN_ENTITY_MSG( CheckGhostWitnessed, GHOST_ONLY )			//! 需要EntityID
		FvUInt8		dummy;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( CheckGhostWitnessed, x.dummy )
	MERCURY_OSTREAM( CheckGhostWitnessed, x.dummy )

	FV_BEGIN_ENTITY_MSG( AoiPriorityUpdate, GHOST_ONLY )			//! 需要EntityID
		float aoiPriority;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( AoiPriorityUpdate, x.aoiPriority )
	MERCURY_OSTREAM( AoiPriorityUpdate, x.aoiPriority )

	FV_BEGIN_ENTITY_MSG( VisibleUpdate, GHOST_ONLY )				//! 需要EntityID
		float fVisibility;
		FvUInt16 uiObjMask;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( VisibleUpdate, x.fVisibility >> x.uiObjMask )
	MERCURY_OSTREAM( VisibleUpdate, x.fVisibility << x.uiObjMask )

	// Message to run cell script.
	FV_VARLEN_ENTITY_METHOD_MSG( RunScriptMethod, REAL_ONLY )		//! 需要EntityID

	// Message to run base method via a cell mailbox
	FV_VARLEN_ENTITY_METHOD_MSG( CallBaseMethod, REAL_ONLY )		//! 需要EntityID

	// Message to run client method via a cell mailbox
	FV_VARLEN_ENTITY_METHOD_MSG( CallClientMethod, REAL_ONLY )		//! 需要EntityID

	FV_BEGIN_ENTITY_MSG( DelControlledBy, REAL_ONLY )				//! 需要EntityID,经idx发送
		FvEntityID deadController;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( DelControlledBy, x.deadController )
	MERCURY_OSTREAM( DelControlledBy, x.deadController )

	// FvCellApp's EntityChannelFinder uses this to forward base entity packets
	// from the ghost to the real
	FV_VARLEN_ENTITY_MSG( ForwardedBaseEntityPacket, REAL_ONLY )	//! 需要EntityID

	FV_CELL_ENTITY_CALLBACK_MSG( RPCCallBack, REAL_ONLY )

	FV_BEGIN_CELL_ENTITY_CALLBACK_EXCEPTION_MSG( RPCCallBackException, REAL_ONLY )
		FvUInt32	uiCBID;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( RPCCallBackException, x.uiCBID )
	MERCURY_OSTREAM( RPCCallBackException, x.uiCBID )

	FV_VARLEN_ENTITY_MSG( GMCmd, REAL_ONLY )

	// 128 to 254 are messages destined for our entities.
	// They all look like this:
	MERCURY_VARIABLE_MESSAGE( RunExposedMethod, 2, NULL )


	// -------------------------------------------------------------------------
	// FvWatcher messages
	// -------------------------------------------------------------------------

	// Message to forward watcher requests via
	FV_RAW_CELL_APP_MSG( CallWatcher )


END_MERCURY_INTERFACE()
#pragma pack(pop)

#undef CELL_TYPEDEF_MACRO
#undef SECOND_PASS

#endif // __FvCellAppInterface_H__
