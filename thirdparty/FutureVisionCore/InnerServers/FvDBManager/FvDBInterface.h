//{future header message}
#if defined( DEFINE_INTERFACE_HERE ) || defined( DEFINE_SERVER_HERE )
	#undef __FvDBInterface_H__
#endif

#ifndef __FvDBInterface_H__
#define __FvDBInterface_H__

#include <FvServerCommon.h>
#include <FvReviverSubject.h>
#include <FvNetInterfaceMinderMacros.h>


#define MF_RAW_DB_MSG( NAME )												\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2,								\
			DBVarLenMessageHandler, &FvDatabase::NAME );					\

#define MF_MEDIUM_RAW_DB_MSG( NAME )										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 3,								\
			DBVarLenMessageHandler, &FvDatabase::NAME );					\

#define MF_BEGIN_SIMPLE_DB_MSG( NAME )										\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			SimpleDBMessageHandler< DBInterface::NAME##Args >,				\
			&FvDatabase::NAME )												\

#define MF_BEGIN_RETURN_DB_MSG( NAME )										\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			ReturnDBMessageHandler< DBInterface::NAME##Args >,				\
			&FvDatabase::NAME )												\

#define FV_DB_LISTENER_MSG( NAME ) 											\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			DBListenerMessageHandler,										\
			&FvDatabase::NAME )

#pragma pack( push, 1 )
BEGIN_MERCURY_INTERFACE( DBInterface )

	FV_REVIVER_PING_MSG()

	FV_DB_LISTENER_MSG( HandleBaseAppMgrBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleBaseAppMgrBirth, x.msg )
	MERCURY_OSTREAM( HandleBaseAppMgrBirth, x.msg )

	MF_BEGIN_SIMPLE_DB_MSG( ShutDown )
		FvUInt8 dummy;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ShutDown, x.dummy )
	MERCURY_OSTREAM( ShutDown, x.dummy )

	MF_BEGIN_SIMPLE_DB_MSG( ControlledShutDown )
		FvShutDownStage stage;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ControlledShutDown, x.stage )
	MERCURY_OSTREAM( ControlledShutDown, x.stage )

	MF_BEGIN_SIMPLE_DB_MSG( CellAppOverloadStatus )
		bool anyOverloaded;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( CellAppOverloadStatus, x.anyOverloaded )
	MERCURY_OSTREAM( CellAppOverloadStatus, x.anyOverloaded )


	MF_RAW_DB_MSG( LogOn )
		// FvString logOnName
		// FvString password
		// FvNetAddress addrForProxy
		// bool offChannel
		// MD5::Digest digest

	MF_RAW_DB_MSG( LoadEntity )
		// FvEntityTypeID	entityTypeID;
		// FvEntityID entityID;
		// bool nameNotID;
		// nameNotID ? (FvString name) : (FvDatabaseID id );

	MF_MEDIUM_RAW_DB_MSG( WriteEntity )
		// FvInt8 flags; (cell? base? log off?)
		// FvEntityTypeID entityTypeID;
		// FvDatabaseID	databaseID;
		// properties

	MF_BEGIN_RETURN_DB_MSG( DeleteEntity )
		FvEntityTypeID	entityTypeID;
		FvDatabaseID	dbid;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( DeleteEntity, x.entityTypeID >> x.dbid )
	MERCURY_OSTREAM( DeleteEntity, x.entityTypeID << x.dbid )

	MF_RAW_DB_MSG( DeleteEntityByName )
		// FvEntityTypeID	entityTypeID;
		// FvString name;

	MF_BEGIN_RETURN_DB_MSG( LookupEntity )
		FvEntityTypeID	entityTypeID;
		FvDatabaseID    dbid;
		bool			offChannel;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( LookupEntity, x.entityTypeID >> x.dbid >> x.offChannel )
	MERCURY_OSTREAM( LookupEntity, x.entityTypeID << x.dbid << x.offChannel )

	MF_RAW_DB_MSG( LookupEntityByName )
		// FvEntityTypeID		entityTypeID;
		// FvString 		name;
		// bool				offChannel;

	MF_RAW_DB_MSG( LookupDBIDByName )
		// FvEntityTypeID	entityTypeID;
		// FvString name;

	MF_MEDIUM_RAW_DB_MSG( ExecuteRawCommand )
		// char[] command;

	MF_RAW_DB_MSG( PutIDs )
		// FvEntityID ids[];

	MF_RAW_DB_MSG( GetIDs )
		// int numIDs;

	MF_MEDIUM_RAW_DB_MSG( WriteSpaces )

	MF_BEGIN_SIMPLE_DB_MSG( WriteGameTime )
		FvTimeStamp		gameTime;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( WriteGameTime, x.gameTime )
	MERCURY_OSTREAM( WriteGameTime, x.gameTime )

	FV_DB_LISTENER_MSG( HandleDatabaseBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleDatabaseBirth, x.msg )
	MERCURY_OSTREAM( HandleDatabaseBirth, x.msg )

	MF_RAW_DB_MSG( HandleBaseAppDeath )

	MF_RAW_DB_MSG( CheckStatus )

	MF_RAW_DB_MSG( SecondaryDBRegistration );
	MF_RAW_DB_MSG( UpdateSecondaryDBs );
	MF_RAW_DB_MSG( GetSecondaryDBDetails );

END_MERCURY_INTERFACE()
#pragma pack( pop )

#endif // __FvDBInterface_H__
