#ifndef __FvCellAppManagerInterfaceOnce_H__
#define __FvCellAppManagerInterfaceOnce_H__

#include <FvNetTypes.h>
#include <FvIdGenerator.h>
#include <FvAnonymousChannelClient.h>
#include <FvVector4.h>

#pragma pack( push, 1 )
struct FvCellAppInitData
{
	FvInt32 id;					//!< ID of the new CellApp
	FvTimeStamp time;			//!< Current game time
	FvNetAddress baseAppAddr;	//!< Address of the BaseApp to talk to
	bool isReady;				//!< Flag indicating whether the server is ready
};
#pragma pack(pop)
FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvCellAppInitData)

typedef FvUInt8 SharedDataType;
const SharedDataType SHARED_DATA_TYPE_CELL_APP = 1;
const SharedDataType SHARED_DATA_TYPE_BASE_APP = 2;
const SharedDataType SHARED_DATA_TYPE_GLOBAL = 3;
const SharedDataType SHARED_DATA_TYPE_GLOBAL_FROM_BASE_APP = 4;

#endif//__FvCellAppManagerInterfaceOnce_H__



#if defined( DEFINE_INTERFACE_HERE ) || defined( DEFINE_SERVER_HERE )
	#undef __FvCellAppManagerInterface_H__
#endif

#ifndef __FvCellAppManagerInterface_H__
#define __FvCellAppManagerInterface_H__

#ifdef DEFINE_SERVER_HERE
#define SERVER_IS_DEFINED_HERE
#endif

#include <FvNetInterfaceMinderMacros.h>
#include <FvServerCommon.h>
#include <FvNetInterfaceMinder.h>
#include <FvReviverSubject.h>


#define FV_BEGIN_CELL_APP_MGR_MSG( NAME )									\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			CellAppMgrMessageHandler< CellAppMgrInterface::NAME##Args >,	\
			&FvCellAppMgr::NAME )												\

#define FV_BEGIN_CELL_APP_MGR_MSG_WITH_ADDR( NAME )							\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			CellAppMgrMessageWithAddrHandler<								\
					CellAppMgrInterface::NAME##Args >,						\
			&FvCellAppMgr::NAME )												\

#define FV_BEGIN_CELLAPP_MSG( NAME )										\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			CellAppMessageHandler< CellAppMgrInterface::NAME##Args >,		\
			&FvCellApp::NAME )												\

#define FV_VARLEN_CELLAPP_MSG( NAME )										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, CellAppVarLenMessageHandler,	\
			&FvCellApp::NAME )

#define FV_VARLEN_CELL_APP_MGR_MSG( NAME )									\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2,								\
			CellAppMgrVarLenMessageHandler,	&FvCellAppMgr::NAME )

#define FV_RAW_CELL_APP_MGR_MSG( NAME )										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, CellAppMgrRawMessageHandler,	\
			&FvCellAppMgr::NAME )

#define FV_CELL_APP_MGR_LISTENER_MSG( NAME ) 								\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			CellAppMgrListenerMessageHandler,								\
			&FvCellAppMgr::NAME )


#ifdef SERVER_IS_DEFINED_HERE
	#undef FV_BEGIN_RETURN_CELL_APP_MGR_MSG
	#undef SERVER_IS_DEFINED_HERE
	#define FV_BEGIN_RETURN_CELL_APP_MGR_MSG( RETURN_TYPE, NAME )			\
		typedef CellAppMgrReturnMessageHandler< RETURN_TYPE,				\
						CellAppMgrInterface::NAME##Args > _BLAH_##NAME; 	\
		BEGIN_HANDLED_STRUCT_MESSAGE( NAME,									\
				_BLAH_##NAME,												\
				&FvCellAppMgr::NAME )
#else
	#undef FV_BEGIN_RETURN_CELL_APP_MGR_MSG
	#define FV_BEGIN_RETURN_CELL_APP_MGR_MSG( RETURN_TYPE, NAME )			\
		BEGIN_HANDLED_STRUCT_MESSAGE( NAME, a, b )
#endif


// -----------------------------------------------------------------------------
// Section: Cell App Manager interface
// -----------------------------------------------------------------------------

#pragma pack(push, 1)
BEGIN_MERCURY_INTERFACE( CellAppMgrInterface )

	FV_ANONYMOUS_CHANNEL_CLIENT_MSG( DBInterface )

	// The arguments are the same as for Cell::createEntity.
	// It assumes that the first two arguments are:
	// 	EntityID		- The id of the new entity
	// 	Position3D		- The position of the new entity
	//
	FV_RAW_CELL_APP_MGR_MSG( CreateEntity )
	FV_RAW_CELL_APP_MGR_MSG( CreateEntityInNewSpace )

	FV_RAW_CELL_APP_MGR_MSG( PrepareForRestoreFromDB )

	FV_RAW_CELL_APP_MGR_MSG( Startup )

	FV_BEGIN_CELL_APP_MGR_MSG( ShutDown )
		bool isSigInt;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ShutDown, x.isSigInt )
	MERCURY_OSTREAM( ShutDown, x.isSigInt )

	FV_BEGIN_CELL_APP_MGR_MSG( ControlledShutDown )
		FvShutDownStage stage;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ControlledShutDown, x.stage )
	MERCURY_OSTREAM( ControlledShutDown, x.stage )

	FV_BEGIN_CELL_APP_MGR_MSG( ShouldOffload )
		bool enable;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ShouldOffload, x.enable )
	MERCURY_OSTREAM( ShouldOffload, x.enable )

	FV_VARLEN_CELL_APP_MGR_MSG( RunScript );

	FV_RAW_CELL_APP_MGR_MSG( AddApp );

	FV_VARLEN_CELL_APP_MGR_MSG( RecoverCellApp );

	FV_BEGIN_CELL_APP_MGR_MSG( DelApp )
		FvNetAddress addr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( DelApp, x.addr )
	MERCURY_OSTREAM( DelApp, x.addr )

	FV_BEGIN_CELL_APP_MGR_MSG( SetBaseApp )
		FvNetAddress addr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( SetBaseApp, x.addr )
	MERCURY_OSTREAM( SetBaseApp, x.addr )

	FV_CELL_APP_MGR_LISTENER_MSG( HandleCellAppMgrBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleCellAppMgrBirth, x.msg )
	MERCURY_OSTREAM( HandleCellAppMgrBirth, x.msg )

	FV_CELL_APP_MGR_LISTENER_MSG( HandleBaseAppMgrBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleBaseAppMgrBirth, x.msg )
	MERCURY_OSTREAM( HandleBaseAppMgrBirth, x.msg )

	FV_CELL_APP_MGR_LISTENER_MSG( HandleCellAppDeath )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleCellAppDeath, x.msg )
	MERCURY_OSTREAM( HandleCellAppDeath, x.msg )

	FV_VARLEN_CELL_APP_MGR_MSG( HandleBaseAppDeath )

	FV_BEGIN_CELL_APP_MGR_MSG_WITH_ADDR( AckCellAppDeath )
		FvNetAddress deadAddr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( AckCellAppDeath, x.deadAddr )
	MERCURY_OSTREAM( AckCellAppDeath, x.deadAddr )

	FV_BEGIN_RETURN_CELL_APP_MGR_MSG( double, GameTimeReading )
		double		gameTimeReadingContribution;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( GameTimeReading, x.gameTimeReadingContribution )
	MERCURY_OSTREAM( GameTimeReading, x.gameTimeReadingContribution )

	FV_RAW_CELL_APP_MGR_MSG( UpdateSpaceData )

	FV_BEGIN_CELL_APP_MGR_MSG( ShutDownSpace )
		FvSpaceID spaceID;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ShutDownSpace, x.spaceID )
	MERCURY_OSTREAM( ShutDownSpace, x.spaceID )

	FV_BEGIN_CELL_APP_MGR_MSG( AckBaseAppsShutDown )
		FvShutDownStage stage;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( AckBaseAppsShutDown, x.stage )
	MERCURY_OSTREAM( AckBaseAppsShutDown, x.stage )

	FV_RAW_CELL_APP_MGR_MSG( CheckStatus )


	FV_BEGIN_CELLAPP_MSG( InformOfLoad )
		float load;
		FvInt32	numEntities;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( InformOfLoad, x.load >> x.numEntities )
	MERCURY_OSTREAM( InformOfLoad, x.load << x.numEntities )

	FV_VARLEN_CELLAPP_MSG( UpdateBounds );

	FV_BEGIN_CELLAPP_MSG( ShutDownApp )
		FvInt8 dummy;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ShutDownApp, x.dummy )
	MERCURY_OSTREAM( ShutDownApp, x.dummy )

	FV_BEGIN_CELLAPP_MSG( AckCellAppShutDown )
		FvShutDownStage stage;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( AckCellAppShutDown, x.stage )
	MERCURY_OSTREAM( AckCellAppShutDown, x.stage )

	FV_REVIVER_PING_MSG()

	FV_VARLEN_CELL_APP_MGR_MSG( SetSharedData )
	FV_VARLEN_CELL_APP_MGR_MSG( DelSharedData )

	FV_VARLEN_CELL_APP_MGR_MSG( AddGlobalBase )
	FV_VARLEN_CELL_APP_MGR_MSG( DelGlobalBase )

END_MERCURY_INTERFACE()
#pragma pack(pop)

#endif //__FvCellAppManagerInterface_H__
