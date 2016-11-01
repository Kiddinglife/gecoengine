//{future header message}
#if defined( DEFINE_INTERFACE_HERE ) || defined( DEFINE_SERVER_HERE )
	#undef __FvBaseAppManagerInterface_H__
#endif

#ifndef __FvBaseAppManagerInterface_H__
#define __FvBaseAppManagerInterface_H__

#define FV_BASE_APP_MGR_INTERFACE_HPP_FIRSTTIME

#ifndef FV_BASE_APP_MGR_INTERFACE_HPP_ONCE
#define FV_BASE_APP_MGR_INTERFACE_HPP_ONCE

#include <FvNetTypes.h>

#pragma pack( push, 1 )
struct FvBaseAppInitData
{
	FvInt32 id;
	FvTimeStamp time;
	bool isReady;
};
#pragma pack( pop )

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvBaseAppInitData)

#else
#undef FV_BASE_APP_MGR_INTERFACE_HPP_FIRSTTIME
#endif // FV_BASE_APP_MGR_INTERFACE_HPP_ONCE

#define FV_BEGIN_BASE_APP_MGR_MSG( NAME )									\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			BaseAppMgrMessageHandler< BaseAppMgrInterface::NAME##Args >,	\
			&FvBaseAppMgr::NAME )												\

#define FV_BEGIN_BASE_APP_MGR_MSG_WITH_ADDR( NAME )							\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
		BaseAppMgrMessageHandlerWithAddr< BaseAppMgrInterface::NAME##Args >,\
		&FvBaseAppMgr::NAME )													\

#define FV_RAW_BASE_APP_MGR_MSG( NAME )										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, BaseAppMgrRawMessageHandler,	\
			&FvBaseAppMgr::NAME )

#define FV_VARLEN_BASE_APP_MGR_MSG( NAME )										\
	MERCURY_HANDLED_VARIABLE_MESSAGE( NAME, 2, BaseAppMgrVarLenMessageHandler,	\
			&FvBaseAppMgr::NAME )

#define FV_BASE_APP_MGR_LISTENER_MSG( NAME ) 								\
	BEGIN_HANDLED_STRUCT_MESSAGE( NAME,										\
			BaseAppMgrListenerMessageHandler,								\
			&FvBaseAppMgr::NAME )


#ifdef DEFINE_SERVER_HERE
	#undef FV_BEGIN_RETURN_BASE_APP_MGR_MSG
	#define FV_BEGIN_RETURN_BASE_APP_MGR_MSG( NAME )						\
		typedef BaseAppMgrReturnMessageHandler<								\
							BaseAppMgrInterface::NAME##Args > _BLAH_##NAME;	\
		BEGIN_HANDLED_STRUCT_MESSAGE( NAME,									\
				_BLAH_##NAME,												\
				&FvBaseAppMgr::NAME )
#else
	#undef FV_BEGIN_RETURN_BASE_APP_MGR_MSG
	#define FV_BEGIN_RETURN_BASE_APP_MGR_MSG( NAME )							\
		BEGIN_HANDLED_STRUCT_MESSAGE( NAME, 0, 0 )
#endif

#include <FvServerCommon.h>
#include <FvAnonymousChannelClient.h>
#include <FvReviverSubject.h>
#include <FvNetInterfaceMinderMacros.h>

#pragma pack( push, 1 )
BEGIN_MERCURY_INTERFACE( BaseAppMgrInterface )

	FV_ANONYMOUS_CHANNEL_CLIENT_MSG( DBInterface )

#ifdef FV_BASE_APP_MGR_INTERFACE_HPP_FIRSTTIME
	enum CreateEntityError
	{
		CREATE_ENTITY_ERROR_NO_BASEAPPS = 1,
		CREATE_ENTITY_ERROR_BASEAPPS_OVERLOADED
	};
#endif
	
	MERCURY_HANDLED_VARIABLE_MESSAGE( CreateEntity,	2,						\
									CreateEntityIncomingHandler, NULL )
									
	FV_BEGIN_RETURN_BASE_APP_MGR_MSG( Add )
		FvNetAddress	addrForCells;
		FvNetAddress	addrForClients;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( Add, x.addrForCells >> x.addrForClients )
	MERCURY_OSTREAM( Add, x.addrForCells << x.addrForClients )

	FV_BEGIN_RETURN_BASE_APP_MGR_MSG( AddBackup )
		FvNetAddress	addr;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( AddBackup, x.addr )
	MERCURY_OSTREAM( AddBackup, x.addr )

	FV_RAW_BASE_APP_MGR_MSG( RecoverBaseApp )
		// FvNetAddress		addrForCells;
		// FvNetAddress		addrForClients;
		// FvNetAddress		backupAddress;
		// FvBaseAppID			id;
		// float				maxLoad;
		// string, MailBoxRef	globalBases; (0 to many)

	FV_RAW_BASE_APP_MGR_MSG( OldRecoverBackupBaseApp )
		//  FvNetAddress	addr;
		//  FvBaseAppID			id;
		//  float				maxLoad;
		//  FvNetAddress	backups; (0 to many)

	FV_BEGIN_BASE_APP_MGR_MSG_WITH_ADDR( Del )
		FvBaseAppID		id;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( Del, x.id )
	MERCURY_OSTREAM( Del, x.id )

	FV_BEGIN_BASE_APP_MGR_MSG_WITH_ADDR( InformOfLoad )
		float load;
		FvInt32 numBases;
		FvInt32 numProxies;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( InformOfLoad, x.load >> x.numBases >> x.numProxies )
	MERCURY_OSTREAM( InformOfLoad, x.load << x.numBases << x.numProxies )

	FV_BEGIN_BASE_APP_MGR_MSG( ShutDown )
		bool		shouldShutDownOthers;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ShutDown, x.shouldShutDownOthers )
	MERCURY_OSTREAM( ShutDown, x.shouldShutDownOthers )

	FV_BEGIN_BASE_APP_MGR_MSG( ControlledShutDown )
		FvShutDownStage stage;
		FvTimeStamp shutDownTime;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( ControlledShutDown, x.stage >> x.shutDownTime )
	MERCURY_OSTREAM( ControlledShutDown, x.stage << x.shutDownTime )

	FV_BASE_APP_MGR_LISTENER_MSG( HandleBaseAppDeath )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleBaseAppDeath, x.msg )
	MERCURY_OSTREAM( HandleBaseAppDeath, x.msg )

	FV_BASE_APP_MGR_LISTENER_MSG( HandleCellAppMgrBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleCellAppMgrBirth, x.msg )
	MERCURY_OSTREAM( HandleCellAppMgrBirth, x.msg )

	FV_BASE_APP_MGR_LISTENER_MSG( HandleBaseAppMgrBirth )
		InterfaceListenerMsg	msg;
	END_STRUCT_MESSAGE()
	MERCURY_ISTREAM( HandleBaseAppMgrBirth, x.msg )
	MERCURY_OSTREAM( HandleBaseAppMgrBirth, x.msg )

	FV_RAW_BASE_APP_MGR_MSG( HandleCellAppDeath )
	FV_RAW_BASE_APP_MGR_MSG( CreateBaseEntity )

	FV_RAW_BASE_APP_MGR_MSG( RegisterBaseGlobally )
	FV_RAW_BASE_APP_MGR_MSG( DeregisterBaseGlobally )

	FV_RAW_BASE_APP_MGR_MSG( RunScript );

	FV_RAW_BASE_APP_MGR_MSG( RequestHasStarted )

	FV_RAW_BASE_APP_MGR_MSG( InitData )
	
	FV_RAW_BASE_APP_MGR_MSG( Startup )

	FV_RAW_BASE_APP_MGR_MSG( CheckStatus )

	FV_RAW_BASE_APP_MGR_MSG( SpaceDataRestore )

	FV_VARLEN_BASE_APP_MGR_MSG( SetSharedData )
	FV_VARLEN_BASE_APP_MGR_MSG( DelSharedData )

	FV_RAW_BASE_APP_MGR_MSG( UseNewBackupHash )

	FV_RAW_BASE_APP_MGR_MSG( InformOfArchiveComplete )

	FV_REVIVER_PING_MSG()

END_MERCURY_INTERFACE()
#pragma pack(pop)

#endif // __FvBaseAppManagerInterface_H__
