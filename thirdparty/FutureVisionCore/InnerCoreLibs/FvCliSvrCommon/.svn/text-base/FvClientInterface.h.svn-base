//{future header message}
#if defined(DEFINE_INTERFACE_HERE) || defined(DEFINE_SERVER_HERE)
	#undef __FvClientInterface_H__
#endif


#ifndef __FvClientInterface_H__
#define __FvClientInterface_H__

#include <FvNetInterfaceMinderMacros.h>
#include <FvNetMsgTypes.h>
#include <FvNetTypes.h>
#include <FvMD5.h>

#define FV_BEGIN_INTERFACE					BEGIN_MERCURY_INTERFACE
#define FV_END_INTERFACE					END_MERCURY_INTERFACE
#define FV_END_MSG							END_STRUCT_MESSAGE

#define FV_BEGIN_MSG						BEGIN_STRUCT_MESSAGE

#define FV_BEGIN_HANDLED_PREFIXED_MSG		BEGIN_HANDLED_PREFIXED_MESSAGE
#define FV_BEGIN_HANDLED_MSG				BEGIN_HANDLED_STRUCT_MESSAGE



#define FV_BEGIN_CLIENT_MSG( NAME )											\
	FV_BEGIN_HANDLED_MSG( NAME,												\
			ClientMessageHandler< ClientInterface::NAME##Args >,			\
			&FvServerConnection::NAME )										\

#define FV_VARLEN_CLIENT_MSG( NAME )										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2,								\
			ClientVarLenMessageHandler,	&FvServerConnection::NAME )

#define FV_VARLEN_WITH_ADDR_CLIENT_MSG( NAME )								\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2,								\
			ClientVarLenWithAddrMessageHandler,	&FvServerConnection::NAME )


#pragma pack(push, 1)
FV_BEGIN_INTERFACE( ClientInterface )

	FV_BEGIN_CLIENT_MSG( Authenticate )
		FvUInt32			key;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( Authenticate, x.key )
	MERCURY_OSTREAM( Authenticate, x.key )

	FV_BEGIN_CLIENT_MSG( BandwidthNotification )
		FvUInt32			bps;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( BandwidthNotification, x.bps )
	MERCURY_OSTREAM( BandwidthNotification, x.bps )

	FV_BEGIN_CLIENT_MSG( UpdateFrequencyNotification )
		FvUInt8				hertz;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( UpdateFrequencyNotification, x.hertz )
	MERCURY_OSTREAM( UpdateFrequencyNotification, x.hertz )

	FV_BEGIN_CLIENT_MSG( TickSync )
	FvUInt8				tickByte;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( TickSync, x.tickByte )
	MERCURY_OSTREAM( TickSync, x.tickByte )

	FV_BEGIN_CLIENT_MSG( SetGameTime )
		FvTimeStamp			gameTime;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( SetGameTime, x.gameTime )
	MERCURY_OSTREAM( SetGameTime, x.gameTime )

	FV_BEGIN_CLIENT_MSG( ResetEntities )
		bool			keepPlayerOnBase;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ResetEntities, x.keepPlayerOnBase )
	MERCURY_OSTREAM( ResetEntities, x.keepPlayerOnBase )

	FV_VARLEN_CLIENT_MSG( CreateBasePlayer )
	FV_VARLEN_CLIENT_MSG( CreateCellPlayer )

	FV_VARLEN_CLIENT_MSG( SpaceData )
	//	FvEntityID		spaceID
	//	SpaceEntryID	entryID
	//	FvUInt16			key;
	//	char[]			value;		// rest of message

	FV_VARLEN_CLIENT_MSG( CreateEntity )
	FV_VARLEN_CLIENT_MSG( UpdateEntity )

	FV_BEGIN_CLIENT_MSG( EnterAoI )
		FvEntityID			id;
		FvIDAlias			idAlias;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( EnterAoI, x.id >> x.idAlias )
	MERCURY_OSTREAM( EnterAoI, x.id << x.idAlias )

	FV_BEGIN_CLIENT_MSG( EnterAoIOnVehicle )
		FvEntityID			id;
		FvEntityID			vehicleID;
		FvIDAlias			idAlias;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( EnterAoIOnVehicle,
		x.id >> x.vehicleID >> x.idAlias )
	MERCURY_OSTREAM( EnterAoIOnVehicle,
		x.id << x.vehicleID << x.idAlias )

	FV_VARLEN_CLIENT_MSG( LeaveAoI )
	//	FvEntityID		id;
	//	EventNumber[]	lastEventNumbers;	// rest

	FV_BEGIN_CLIENT_MSG( SetVehicle )
		FvEntityID			passengerID;
		FvEntityID			vehicleID;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( SetVehicle,
		x.passengerID >> x.vehicleID )
	MERCURY_OSTREAM( SetVehicle,
		x.passengerID << x.vehicleID )

	FV_BEGIN_CLIENT_MSG( DetailedPosition )
		FvEntityID		id;
		FvVector3			position;
		FvDirection3		direction;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( DetailedPosition, x.id >> x.position >> x.direction )
	MERCURY_OSTREAM( DetailedPosition, x.id << x.position << x.direction )

	FV_BEGIN_CLIENT_MSG( ForcedPosition )
		FvEntityID		id;
		FvSpaceID			spaceID;
		FvEntityID		vehicleID;
		FvPosition3D		position;
		FvDirection3		direction;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ForcedPosition,
		x.id >> x.spaceID >> x.vehicleID >> x.position >> x.direction )
	MERCURY_OSTREAM( ForcedPosition,
		x.id << x.spaceID << x.vehicleID << x.position << x.direction )

	FV_BEGIN_CLIENT_MSG( ControlEntity )
		FvEntityID		id;
		bool			on;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ControlEntity, x.id >> x.on )
	MERCURY_OSTREAM( ControlEntity, x.id << x.on )

	FV_BEGIN_CLIENT_MSG( LoggedOff )
		FvUInt8	reason;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( LoggedOff, x.reason )
	MERCURY_OSTREAM( LoggedOff, x.reason )

	FV_VARLEN_CLIENT_MSG( RPCCallBack )

	FV_BEGIN_CLIENT_MSG( RPCCallBackException )
		FvUInt8	id;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( RPCCallBackException, x.id )
	MERCURY_OSTREAM( RPCCallBackException, x.id )

	MERCURY_VARIABLE_MESSAGE( EntityMessage, 2, NULL )

FV_END_INTERFACE()
#pragma pack( pop )

#define CLIENT_INTERFACE_HPP_ONCE

#endif // __FvClientInterface_H__
