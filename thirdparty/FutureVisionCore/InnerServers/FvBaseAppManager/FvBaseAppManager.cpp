#ifdef _WIN32
#pragma warning (disable : 4355)
#endif


#include "FvBaseAppManager.h"
#include "FvBaseApp.h"
#include "FvBaseAppManagerInterface.h"
#include "FvWatcherForwardingBaseApp.h"

#include <../FvCell/FvCellAppInterface.h>
#include <../FvCellAppManager/FvCellAppManagerInterface.h>
#include <FvMemoryStream.h>
#include <FvTimestamp.h>
#include <../FvBase/FvBaseAppIntInterface.h>
#include <../FvLoginApp/FvLoginIntInterface.h>
#include <../FvDBManager/FvDBInterface.h>
#include <FvNetWatcherGlue.h>
#include <FvNetPortMap.h>
#include <FvServerConfig.h>
#include <FvReviverSubject.h>
//#include <FvStreamHelper.h>
#include <FvTimeKeeper.h>
#include <FvDocWatcher.h>

#include <signal.h>
#include <limits>

FV_DECLARE_DEBUG_COMPONENT( 0 )

FV_SINGLETON_STORAGE( FvBaseAppMgr )


void IntSignalHandler( int /*sigNum*/ )
{
	FvBaseAppMgr * pMgr = FvBaseAppMgr::pInstance();

	if (pMgr != NULL)
	{
		pMgr->ShutDown( false );
	}
}

bool SendSignalViaMachined( const FvNetAddress& dest, int sigNum,
	FvNetNub& nub )
{
	FvNetSignalMessage sm;
	sm.m_uiSignal = sigNum;
	sm.m_uiPort = dest.m_uiPort;
	sm.m_uiParam = sm.PARAM_USE_PORT;

	FvNetEndpoint tempEP;
	tempEP.Socket( SOCK_DGRAM );

	if (tempEP.Good() && tempEP.Bind() == 0)
	{
		sm.SendTo( tempEP, htons( FV_MACHINE_PORT ), dest.m_uiIP );
		return true;
	}

	return false;
}


FvBaseAppMgr::FvBaseAppMgr( FvNetNub & nub )
:m_kNub( nub )
,m_kCellAppMgr( m_kNub )
,m_iLastBaseAppID( 0 )
,m_bAllowNewBaseApps( true )
,m_uiTime( 0 )
,m_pkTimeKeeper( NULL )
,m_iUpdateHertz( DEFAULT_GAME_UPDATE_HERTZ )
,m_fBaseAppOverloadLevel( 1.f )
,m_fCreateBaseRatio( 4.f )
,m_iUpdateCreateBaseInfoPeriod( 1 )
,m_kBestBaseAppAddr( 0, 0 )
,m_bIsRecovery( false )
,m_bHasInitData( false )
,m_bHasStarted( false )
,m_bShouldShutDownOthers( false )
,m_bShouldHardKillDeadBaseApps( true )
,m_bOnlyUseBackupOnSameMachine( false )
,m_bUseNewStyleBackup( true )
,m_bShutDownServerOnBadState( FvServerConfig::Get( "shutDownServerOnBadState", true ) )
,m_bShutDownServerOnBaseAppDeath( FvServerConfig::Get( "shutDownServerOnBaseAppDeath", false ) )
,m_bIsProduction( FvServerConfig::Get( "production", false ) )
,m_kDeadBaseAppAddr( FvNetAddress::NONE )
,m_uiArchiveCompleteMsgCounter( 0 )
,m_uiShutDownTime( 0 )
,m_eShutDownStage( SHUTDOWN_NONE )
,m_uiBaseAppTimeoutPeriod( 0 )
,m_uiBaseAppOverloadStartTime( 0 )
,m_iLoginsSinceOverload( 0 )
,m_iAllowOverloadLogins( 10 )
,m_bHasMultipleBaseAppMachines( false )
{
	FvServerConfig::Update( "gameUpdateHertz", m_iUpdateHertz );

	m_kCellAppMgr.Channel().IsIrregular( true );

	float timeSyncPeriodInSeconds =
			FvServerConfig::Get( "baseAppMgr/timeSyncPeriod", 60.f );
	m_iSyncTimePeriod =
		int( floorf( timeSyncPeriodInSeconds * m_iUpdateHertz + 0.5f ) );

	FvServerConfig::Update( "baseAppMgr/baseAppOverloadLevel", m_fBaseAppOverloadLevel);

	FvServerConfig::Update( "baseAppMgr/createBaseRatio", m_fCreateBaseRatio );
	float updateCreateBaseInfoInSeconds =
		FvServerConfig::Get( "baseAppMgr/updateCreateBaseInfoPeriod", 5.f );
	m_iUpdateCreateBaseInfoPeriod =
		int( floorf( updateCreateBaseInfoInSeconds * m_iUpdateHertz + 0.5f ) );

	FvServerConfig::Update( "baseAppMgr/hardKillDeadBaseApps",
			m_bShouldHardKillDeadBaseApps );
	FvServerConfig::Update( "baseAppMgr/onlyUseBackupOnSameMachine",
			m_bOnlyUseBackupOnSameMachine );
	FvServerConfig::Update( "baseAppMgr/useNewStyleBackup", m_bUseNewStyleBackup );

	if (!m_bUseNewStyleBackup)
	{
		FV_ERROR_MSG( "Old-style BaseApp backups are no longer supported. "
			"Using new-style backup instead.\n" );

		m_bUseNewStyleBackup = true;
	}

	float baseAppTimeout = 5.f;
	FvServerConfig::Update( "baseAppMgr/baseAppTimeout", baseAppTimeout );
	m_uiBaseAppTimeoutPeriod = FvInt64( StampsPerSecondD() * baseAppTimeout );

	m_uiAllowOverloadPeriod = FvUInt64( StampsPerSecondD() *
			FvServerConfig::Get( "baseAppMgr/overloadTolerancePeriod", 5.f ) );
	FvServerConfig::Update( "baseAppMgr/overloadLogins", m_iAllowOverloadLogins );

	FV_INFO_MSG( "\n---- Base App Manager ----\n" );
	FV_INFO_MSG( "Address          = %s\n", m_kNub.Address().c_str() );
	FV_INFO_MSG( "Time Sync Period = %d\n", m_iSyncTimePeriod );
}

FvBaseAppMgr::~FvBaseAppMgr()
{
	if (m_bShouldShutDownOthers)
	{
		BaseAppIntInterface::ShutDownArgs	baseAppShutDownArgs = { false };

		{
			FvBaseApps::iterator iter = m_kBaseApps.begin();
			while (iter != m_kBaseApps.end())
			{
				iter->second->Bundle() << baseAppShutDownArgs;
				iter->second->Send();

				++iter;
			}
		}

		{
			FvBackupBaseApps::iterator iter = m_kBackupBaseApps.begin();
			while (iter != m_kBackupBaseApps.end())
			{
				iter->second->Bundle() << baseAppShutDownArgs;
				iter->second->Send();

				++iter;
			}
		}


		if (m_kCellAppMgr.Channel().IsEstablished())
		{
			FvNetBundle	& bundle = m_kCellAppMgr.Bundle();
			CellAppMgrInterface::ShutDownArgs cellAppmgrShutDownArgs = { false };
			bundle << cellAppmgrShutDownArgs;
			m_kCellAppMgr.Send();
		}
	}

	m_kNub.ProcessUntilChannelsEmpty();

	if (m_pkTimeKeeper)
	{
		delete m_pkTimeKeeper;
		m_pkTimeKeeper = NULL;
	}
}

bool FvBaseAppMgr::Init(int argc, char * argv[])
{
	if (m_kNub.Socket() == -1)
	{
		FV_ERROR_MSG( "Failed to create Nub on internal interface.\n" );
		return false;
	}

	FvReviverSubject::Instance().Init( &m_kNub, "baseAppMgr" );

	for (int i = 0; i < argc; ++i)
	{
		if (strcmp( argv[i], "-recover" ) == 0)
		{
			m_bIsRecovery = true;
			break;
		}
	}

	FV_INFO_MSG( "isRecovery = %s\n", m_bIsRecovery ? "True" : "False" );

	if (m_bIsProduction)
	{
		FV_INFO_MSG( "BaseAppMgr::init: Production mode enabled\n" );
	}

	this->Nub().RegisterDeathListener( BaseAppMgrInterface::HandleBaseAppDeath,
		"BaseAppIntInterface" );

	if (!FV_INIT_ANONYMOUS_CHANNEL_CLIENT( m_kDBMgr, m_kNub,
				BaseAppMgrInterface, DBInterface, 0 ))
	{
		FV_INFO_MSG( "BaseAppMgr::init: Database not ready yet.\n" );
	}

	BaseAppMgrInterface::RegisterWithNub( m_kNub );

	FvNetReason reason =
		BaseAppMgrInterface::RegisterWithMachined( m_kNub, 0 );

	if (reason != FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG("BaseAppMgr::init: Unable to register with nub. "
				"Is machined running?\n");
		return false;
	}

	{
		m_kNub.RegisterBirthListener( BaseAppMgrInterface::HandleCellAppMgrBirth,
			"CellAppMgrInterface" );

		FvNetAddress cellAppMgrAddr;
		reason = m_kNub.FindInterface( "CellAppMgrInterface", 0, cellAppMgrAddr );

		if (reason == FV_NET_REASON_SUCCESS)
		{
			m_kCellAppMgr.Addr( cellAppMgrAddr );
		}
		else if (reason == FV_NET_REASON_TIMER_EXPIRED)
		{
			FV_INFO_MSG( "BaseAppMgr::init: CellAppMgr not ready yet.\n" );
		}
		else
		{
			FV_ERROR_MSG( "BaseAppMgr::init: "
				"Failed to find CellAppMgr interface: %s\n",
				NetReasonToString( (FvNetReason &)reason ) );

			return false;
		}

		m_kNub.RegisterBirthListener(
			BaseAppMgrInterface::HandleBaseAppMgrBirth,
			"BaseAppMgrInterface" );
	}

	signal( SIGINT, ::IntSignalHandler );
#ifndef _WIN32
	signal( SIGHUP, ::IntSignalHandler );
#endif //ndef _WIN32

	FV_INIT_WATCHER_DOC( "baseappmgr" );

	FV_REGISTER_WATCHER( 0, "baseappmgr",
		"Base App Manager", "baseAppMgr", m_kNub );

	this->AddWatchers();

	return true;
}


FvBaseApp * FvBaseAppMgr::FindBaseApp( const FvNetAddress & addr )
{
	FvBaseApps::iterator it = m_kBaseApps.find( addr );
	if (it != m_kBaseApps.end())
		return it->second.Get();
	else
		return NULL;
}


FvBaseApp * FvBaseAppMgr::FindBestBaseApp() const
{
	const FvBaseApp * pBest = NULL;

	float lowestLoad = 99999.f;
	FvBaseAppMgr::FvBaseApps::const_iterator iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		float currLoad = iter->second->Load();

		if (currLoad < lowestLoad)
		{
			lowestLoad = currLoad;
			pBest = iter->second.Get();
		}

		iter++;
	}

	return const_cast< FvBaseApp * >( pBest );
}

FvBackupBaseApp * FvBaseAppMgr::FindBestBackup( const FvBaseApp & baseApp ) const
{
	if (m_kBackupBaseApps.empty())
	{
		return NULL;
	}

	if (m_bOnlyUseBackupOnSameMachine)
	{
		const FvBackupBaseApp * pBest = NULL;
		float bestLoad = std::numeric_limits< float >::max();

		FvNetAddress baseAppIPAddr( baseApp.Addr().m_uiIP, 0 );
		FvBackupBaseApps::const_iterator pCandidate =
			m_kBackupBaseApps.lower_bound( baseAppIPAddr );
		while ( (pCandidate != m_kBackupBaseApps.end()) &&
				(pCandidate->first.m_uiIP == baseApp.Addr().m_uiIP) )
		{
			float candidateLoad = pCandidate->second->Load();
			if (candidateLoad < bestLoad)
			{
				pBest = pCandidate->second.Get();
				bestLoad = candidateLoad;
			}
			++pCandidate;
		}
		return const_cast<FvBackupBaseApp *>( pBest );
	}

	FvBackupBaseApps::const_iterator iter = m_kBackupBaseApps.begin();
	const FvBackupBaseApp * pBest = iter->second.Get();
	++iter;

	while (iter != m_kBackupBaseApps.end())
	{
		const FvBackupBaseApp * pCurr = iter->second.Get();

		bool isBestOn = (pBest->Addr().m_uiIP == baseApp.Addr().m_uiIP);
		bool isCurrOn = (pCurr->Addr().m_uiIP == baseApp.Addr().m_uiIP);

		if (isCurrOn == isBestOn)
		{
			float currLoad = pCurr->Load();

			if (currLoad < pBest->Load())
			{
				pBest = pCurr;
			}
		}
		else if (!isCurrOn)
		{
			pBest = pCurr;
		}

		++iter;
	}

	return const_cast<FvBackupBaseApp *>( pBest );
}


int FvBaseAppMgr::NumBases() const
{
	int count = 0;

	FvBaseAppMgr::FvBaseApps::const_iterator iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		count += iter->second->NumBases();

		iter++;
	}

	return count;
}


int FvBaseAppMgr::NumProxies() const
{
	int count = 0;

	FvBaseAppMgr::FvBaseApps::const_iterator iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		count += iter->second->NumProxies();

		iter++;
	}

	return count;
}

float FvBaseAppMgr::MinBaseAppLoad() const
{
	float load = 2.f;

	FvBaseAppMgr::FvBaseApps::const_iterator iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		load = std::min( load, iter->second->Load() );

		++iter;
	}

	return load;
}

float FvBaseAppMgr::AvgBaseAppLoad() const
{
	float load = 0.f;

	FvBaseAppMgr::FvBaseApps::const_iterator iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		load += iter->second->Load();

		++iter;
	}

	return m_kBaseApps.empty() ? 0.f : load/m_kBaseApps.size();
}

float FvBaseAppMgr::MaxBaseAppLoad() const
{
	float load = 0.f;

	FvBaseAppMgr::FvBaseApps::const_iterator iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		load = std::max( load, iter->second->Load() );

		++iter;
	}

	return load;
}

FvBaseAppID FvBaseAppMgr::GetNextID()
{
	bool foundNext = false;

	while (!foundNext)
	{
		m_iLastBaseAppID = (m_iLastBaseAppID+1) & 0x0FFFFFFF;

		foundNext = true;
	}

	return m_iLastBaseAppID;
}

void FvBaseAppMgr::SendToBaseApps( const FvNetInterfaceElement & ifElt,
	FvMemoryOStream & args, const FvBaseApp * pExclude,
	FvNetReplyMessageHandler * pHandler )
{
	for (FvBaseApps::iterator it = m_kBaseApps.begin(); it != m_kBaseApps.end(); ++it)
	{
		FvBaseApp & baseApp = *it->second;

		if (pExclude == &baseApp)
			continue;

		FvNetBundle & bundle = baseApp.Bundle();

		if (!pHandler)
			bundle.StartMessage( ifElt );
		else
			bundle.StartRequest( ifElt, pHandler );

		bundle.AddBlob( args.Data(), args.Size() );

		baseApp.Send();
	}
}

void FvBaseAppMgr::SendToBackupBaseApps( const FvNetInterfaceElement & ifElt,
	FvMemoryOStream & args, const FvBackupBaseApp * pExclude,
	FvNetReplyMessageHandler * pHandler )
{
	for (FvBackupBaseApps::iterator it = m_kBackupBaseApps.begin();
		 it != m_kBackupBaseApps.end(); ++it)
	{
		FvBackupBaseApp & backup = *it->second;

		if (pExclude == &backup)
			continue;

		FvNetBundle & bundle = backup.Bundle();

		if (!pHandler)
			bundle.StartMessage( ifElt );
		else
			bundle.StartRequest( ifElt, pHandler );

		bundle.AddBlob( args.Data(), args.Size() );

		backup.Send();
	}
}

void FvBaseAppMgr::SendToCellAppMgr(const FvNetInterfaceElement& ifElt, FvMemoryOStream& args)
{
	if(!m_kCellAppMgr.Channel().IsEstablished())
	{
		FV_WARNING_MSG("%s, CellAppMgr isNot ready\n", __FUNCTION__);
		return;
	}

	FvNetBundle& bundle = m_kCellAppMgr.Bundle();
	bundle.StartMessage( ifElt );
	bundle.AddBlob( args.Data(), args.Size() );
	m_kCellAppMgr.Send();
}


void FvBaseAppMgr::AddWatchers()
{
//	FvWatcher * pRoot = &FvWatcher::RootWatcher();
//
//	FV_WATCH( "numBaseApps", *this, &FvBaseAppMgr::NumBaseApps );
//	FV_WATCH( "numBackupBaseApps", *this, &FvBaseAppMgr::NumBackupBaseApps );
//
//	FV_WATCH( "numBases", *this, &FvBaseAppMgr::NumBases );
//	FV_WATCH( "numProxies", *this, &FvBaseAppMgr::NumProxies );
//
//	FV_WATCH( "config/shouldShutDownOthers", m_bShouldShutDownOthers );
//
//	FV_WATCH( "config/createBaseRatio", m_fCreateBaseRatio );
//	FV_WATCH( "config/updateCreateBaseInfoPeriod",
//			m_iUpdateCreateBaseInfoPeriod );
//
//	FV_WATCH( "baseAppLoad/min", *this, &FvBaseAppMgr::MinBaseAppLoad );
//	FV_WATCH( "baseAppLoad/average", *this, &FvBaseAppMgr::AvgBaseAppLoad );
//	FV_WATCH( "baseAppLoad/max", *this, &FvBaseAppMgr::MaxBaseAppLoad );
//
//	FV_WATCH( "config/baseAppOverloadLevel", m_fBaseAppOverloadLevel );
//
//	FvWatcher * pBaseAppWatcher = FvBaseApp::MakeWatcher();
//
//	pRoot->AddChild( "baseApps", new FvMapWatcher<FvBaseApps>( m_kBaseApps ) );
//	pRoot->AddChild( "baseApps/*", pBaseAppWatcher );
//
//	pRoot->AddChild( "backups", new FvMapWatcher<FvBaseApps>( m_kBaseApps ) );
//	pRoot->AddChild( "backups/*", pBaseAppWatcher );
//
//	FV_WATCH( "lastBaseAppIDAllocated", m_iLastBaseAppID );
//
//	//pRoot->AddChild( "nub", FvNetNub::pWatcher(), &m_kNub );
//
//	//pRoot->AddChild( "cellAppMgr", FvNetChannel::pWatcher(),
//	//	&m_kCellAppMgr.Channel() );
//
//	//pRoot->AddChild( "forwardTo", new FvBaseAppForwardingWatcher() );
//
//#if 0
//	pRoot->AddChild( "dbMgr", FvNetChannel::pWatcher(),
//		&this->dbMgr().Channel() );
//#endif
//
//	FV_WATCH( "command/runScriptSingle", *this,
//			FV_WRITE_ACCESSOR( std::string, FvBaseAppMgr, RunScriptSingle ) );
//	FV_WATCH( "command/runScriptAll", *this,
//			FV_WRITE_ACCESSOR( std::string, FvBaseAppMgr, RunScriptAll ) );
}


int FvBaseAppMgr::HandleTimeout( FvNetTimerID /*id*/, void * arg )
{
	if ((m_uiShutDownTime != 0) && (m_uiShutDownTime == m_uiTime))
		return 0;

	switch (reinterpret_cast<FvUIntPtr>( arg ))
	{
		case TIMEOUT_GAME_TICK:
		{
			++m_uiTime;

			if (m_uiTime % m_iSyncTimePeriod == 0)
			{
				m_pkTimeKeeper->SynchroniseWithMaster();
			}

			this->CheckForDeadBaseApps();

			if (m_uiTime % m_iUpdateCreateBaseInfoPeriod == 0)
			{
				this->UpdateCreateBaseInfo();
			}

			{
				FvBaseApp * pBest = this->FindBestBaseApp();

				if ((pBest != NULL) &&
					(m_kBestBaseAppAddr != pBest->Addr()) &&
					m_kCellAppMgr.Channel().IsEstablished())
				{
					m_kBestBaseAppAddr = pBest->Addr();
					FvNetBundle & bundle = m_kCellAppMgr.Bundle();
					bundle.StartMessage( CellAppMgrInterface::SetBaseApp );
					bundle << m_kBestBaseAppAddr;
					m_kCellAppMgr.Send();
				}
			}
		}
		break;
	}

	return 0;
}


void FvBaseAppMgr::CheckForDeadBaseApps()
{
	FvUInt64 currTime = ::Timestamp();
	FvUInt64 lastHeardTime = 0;
	FvBaseApps::iterator iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		lastHeardTime = std::max( lastHeardTime,
				iter->second->Channel().LastReceivedTime() );
		++iter;
	}

	const FvUInt64 timeSinceAnyHeard = currTime - lastHeardTime;

	iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		FvBaseApp * pApp = iter->second.Get();

		if (pApp->HasTimedOut( currTime,
								m_uiBaseAppTimeoutPeriod,
								timeSinceAnyHeard ))
		{
			FV_INFO_MSG( "BaseAppMgr::checkForDeadBaseApps: %s has timed out.\n",
					pApp->Addr().c_str() );
			this->HandleBaseAppDeath( pApp->Addr() );
			return;
		}

		iter++;
	}
}


void FvBaseAppMgr::InformOfLoad( const BaseAppMgrInterface::InformOfLoadArgs & args,
	   const FvNetAddress & addr )
{
	FvBaseApps::iterator iter = m_kBaseApps.find( addr );

	if (iter != m_kBaseApps.end())
	{
		iter->second->UpdateLoad( args.load, args.numBases, args.numProxies );
	}
	else
	{
		FvBackupBaseApps::iterator backupIter = m_kBackupBaseApps.find( addr );

		if (backupIter != m_kBackupBaseApps.end())
		{
			FV_ASSERT( (args.numBases == 0) && (args.numProxies == 0) );
			backupIter->second->UpdateLoad( args.load );
		}
		else
		{
			FV_ERROR_MSG( "BaseAppMgr::informOfLoad: No BaseApp with address %s\n",
					(char *) addr );
		}
	}
}


class CreateBaseReplyHandler : public FvNetReplyMessageHandler
{
public:
	CreateBaseReplyHandler( const FvNetAddress & srcAddr,
		 	FvNetReplyID replyID,
			const FvNetAddress & externalAddr ) :
		m_kSrcAddr( srcAddr ),
		m_iReplyID( replyID ),
		m_kExternalAddr( externalAddr )
	{
	}

private:
	void HandleMessage(const FvNetAddress& /*srcAddr*/,
		FvNetUnpackedMessageHeader& /*header*/,
		FvBinaryIStream& data, void * /*arg*/)
	{
		FvEntityMailBoxRef ref;
		data >> ref;

		FvNetChannelSender sender( FvBaseAppMgr::GetChannel( m_kSrcAddr ) );
		FvNetBundle & bundle = sender.Bundle();

		bundle.StartReply( m_iReplyID );

		if (ref.m_kAddr.m_uiIP != 0)
		{
			bundle << m_kExternalAddr;
			bundle << ref;
			bundle.Transfer( data, data.RemainingLength() );
		}
		else
		{
			bundle << FvNetAddress( 0, 0 );
			bundle << "Unable to create base";
		}

		delete this;
	}

	void HandleException(const FvNetNubException& ne, void* /*arg*/)
	{
		FvNetChannelSender sender( FvBaseAppMgr::GetChannel( m_kSrcAddr ) );
		FvNetBundle & bundle = sender.Bundle();

		FvNetAddress addr;

		addr.m_uiIP = 0;
		addr.m_uiPort = 0;

		bundle.StartReply( m_iReplyID );
		bundle << addr;
		bundle << NetReasonToString( ne.Reason() );

		delete this;
	}

	FvNetAddress 	m_kSrcAddr;
	FvNetReplyID	m_iReplyID;
	FvNetAddress 	m_kExternalAddr;
};


class ForwardingReplyHandler : public FvNetReplyMessageHandler
{
public:
	ForwardingReplyHandler( const FvNetAddress & srcAddr,
			FvNetReplyID replyID ) :
		m_kSrcAddr( srcAddr ),
		m_iReplyID( replyID )
	{}

private:
	void HandleMessage(const FvNetAddress& /*srcAddr*/,
		FvNetUnpackedMessageHeader& header,
		FvBinaryIStream& data, void * /*arg*/)
	{
		FvNetChannelSender sender( FvBaseAppMgr::GetChannel( m_kSrcAddr ) );
		FvNetBundle & bundle = sender.Bundle();

		bundle.StartReply( m_iReplyID );

		this->PrependData( bundle, data );

		bundle.Transfer( data, data.RemainingLength() );

		delete this;
	}

	void HandleException(const FvNetNubException& ne, void* /*arg*/)
	{
		FV_ERROR_MSG( "ForwardingReplyHandler::handleException: reason %d\n",
				ne.Reason() );
		delete this;
	}

	virtual void PrependData( FvNetBundle & bundle,
								FvBinaryIStream & data ) {};

	FvNetAddress 	m_kSrcAddr;
	FvNetReplyID	m_iReplyID;
};


class CreateEntityIncomingHandler : public FvNetInputMessageHandler
{
public:
	CreateEntityIncomingHandler( void * /*arg*/ ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
	{
		FvBaseAppMgr & baseAppMgr = FvBaseAppMgr::Instance();

		FvNetAddress baseAppAddr( 0, 0 );

		FvBaseApp * pBest = baseAppMgr.FindBestBaseApp();

		if (pBest == NULL)
		{
			FV_ERROR_MSG( "BaseAppMgr::createEntity: Could not find a BaseApp.\n");
			baseAppAddr.m_uiPort =
					BaseAppMgrInterface::CREATE_ENTITY_ERROR_NO_BASEAPPS;

			FvNetChannelSender sender( FvBaseAppMgr::GetChannel( srcAddr ) );
			FvNetBundle & bundle = sender.Bundle();

			bundle.StartReply( header.m_iReplyID );
			bundle << baseAppAddr;
			bundle << "No BaseApp could be found to add to.";

			return;
		}

		bool baseAppsOverloaded = (pBest->Load() > baseAppMgr.m_fBaseAppOverloadLevel);
		if (this->CalculateOverloaded( baseAppsOverloaded ))
		{
			FV_INFO_MSG( "BaseAppMgr::createEntity: All baseapps overloaded "
					"(best load=%.02f > overload level=%.02f.\n",
				pBest->Load(), baseAppMgr.m_fBaseAppOverloadLevel );
			baseAppAddr.m_uiPort =
				BaseAppMgrInterface::CREATE_ENTITY_ERROR_BASEAPPS_OVERLOADED;

			FvNetChannelSender sender( FvBaseAppMgr::GetChannel( srcAddr ) );
			FvNetBundle & bundle = sender.Bundle();

			bundle.StartReply( header.m_iReplyID );
			bundle << baseAppAddr;
			bundle << "All BaseApps overloaded.";

			return;
		}

		baseAppAddr = pBest->ExternalAddr();

		CreateBaseReplyHandler * pHandler =
			new CreateBaseReplyHandler( srcAddr, header.m_iReplyID,
				baseAppAddr );

		FvNetBundle	& bundle = pBest->Bundle();
		bundle.StartRequest( BaseAppIntInterface::CreateBaseWithCellData,
				pHandler );

		bundle.Transfer( data, data.RemainingLength() );

		pBest->Send();

		pBest->AddEntity();
	}

	bool CalculateOverloaded( bool baseAppsOverloaded )
	{
		FvBaseAppMgr & baseAppMgr = FvBaseAppMgr::Instance();
		if (baseAppsOverloaded)
		{
			FvUInt64 overloadTime;

			if (baseAppMgr.m_uiBaseAppOverloadStartTime == 0)
				baseAppMgr.m_uiBaseAppOverloadStartTime = Timestamp();

			overloadTime = Timestamp() - baseAppMgr.m_uiBaseAppOverloadStartTime;
			FV_INFO_MSG( "CellAppMgr::Overloaded for "PRIu64"ms\n",
				overloadTime/(StampsPerSecond()/1000) );

			if ((overloadTime > baseAppMgr.m_uiAllowOverloadPeriod) ||
				(baseAppMgr.m_iLoginsSinceOverload >=
				 	baseAppMgr.m_iAllowOverloadLogins))
			{
				return true;
			}
			else
			{
				baseAppMgr.m_iLoginsSinceOverload++;
				FV_INFO_MSG( "BaseAppMgr::Logins since overloaded " \
					"(allowing max of %d): %d\n",
					baseAppMgr.m_iAllowOverloadLogins,
					baseAppMgr.m_iLoginsSinceOverload );
			}
		}
		else
		{
			baseAppMgr.m_uiBaseAppOverloadStartTime = 0;
			baseAppMgr.m_iLoginsSinceOverload = 0;
		}

		return false;
	}
};


class SyncControlledShutDownHandler : public FvNetReplyMessageHandler
{
public:
	SyncControlledShutDownHandler( FvShutDownStage stage, int count ) :
		m_eStage( stage ),
		m_iCount( count )
	{
		if (m_iCount == 0)
			this->Finalise();
	}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data, void * )
	{
		this->DecCount();
	}

	virtual void HandleException( const FvNetNubException & ne, void * )
	{
		this->DecCount();
	}

	void Finalise()
	{
		FvBaseAppMgr * pApp = FvBaseAppMgr::pInstance();

		if (pApp)
		{
			FV_DEBUG_MSG( "All BaseApps have shut down, informing CellAppMgr\n" );
			FvNetBundle & bundle = pApp->CellAppMgr().Bundle();
			bundle.StartMessage( CellAppMgrInterface::AckBaseAppsShutDown );
			bundle << m_eStage;
			pApp->CellAppMgr().Send();
		}

		delete this;
	}

	void DecCount()
	{
		--m_iCount;

		if (m_iCount == 0)
			this->Finalise();
	}

	FvShutDownStage m_eStage;
	FvInt32			m_iCount;
};


class AsyncControlledShutDownHandler : public FvNetReplyMessageHandler
{
public:
	AsyncControlledShutDownHandler( FvShutDownStage stage,
			std::vector< FvNetAddress > & addrs ) :
		m_eStage( stage ),
		m_iNumSent( 0 )
	{
		m_kAddrs.swap( addrs );
		this->SendNext();
	}
	virtual ~AsyncControlledShutDownHandler() {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data, void * )
	{
		FV_DEBUG_MSG( "AsyncControlledShutDownHandler::handleMessage: "
			"BaseApp %s has finished stage %d\n",
			srcAddr.c_str(), m_eStage );

		if (m_eStage == SHUTDOWN_PERFORM)
		{
			FvBaseAppMgr * pApp = FvBaseAppMgr::pInstance();
			pApp->RemoveControlledShutdownBaseApp( srcAddr );
		}

		this->SendNext();
	}

	virtual void HandleException( const FvNetNubException & ne, void * )
	{
		FV_ERROR_MSG( "AsyncControlledShutDownHandler::handleException: "
			"Reason = %s\n", NetReasonToString( ne.Reason() ) );
		this->SendNext();
	}

	void SendNext()
	{
		bool shouldDeleteThis = true;
		FvBaseAppMgr * pApp = FvBaseAppMgr::pInstance();

		if (pApp)
		{
			if (m_iNumSent < int(m_kAddrs.size()))
			{
				FvNetChannelOwner * pChannelOwner =
					pApp->FindChannelOwner( m_kAddrs[ m_iNumSent ] );

				if (pChannelOwner != NULL)
				{
					FvNetBundle & bundle = pChannelOwner->Bundle();
					bundle.StartRequest(
							BaseAppIntInterface::ControlledShutDown, this );
					shouldDeleteThis = false;
					bundle << m_eStage;
					bundle << 0;
					pChannelOwner->Send();
				}
				else
				{
					FV_WARNING_MSG( "AsyncControlledShutDownHandler::sendNext: "
									"Could not find channel for %s\n",
							(char *)m_kAddrs[ m_iNumSent ] );
				}

				++m_iNumSent;
			}
			else if (m_eStage == SHUTDOWN_DISCONNECT_PROXIES)
			{
				new AsyncControlledShutDownHandler( SHUTDOWN_PERFORM, m_kAddrs );
			}
			else
			{
				FvNetBundle & bundle = pApp->CellAppMgr().Bundle();
				bundle.StartMessage( CellAppMgrInterface::AckBaseAppsShutDown );
				bundle << m_eStage;
				pApp->CellAppMgr().Send();
				pApp->ShutDown( false );
			}

		}

		if (shouldDeleteThis)
			delete this;
	}

	FvShutDownStage				m_eStage;
	std::vector< FvNetAddress > m_kAddrs;
	FvInt32						m_iNumSent;
};


void FvBaseAppMgr::Add( const FvNetAddress & srcAddr,
	FvNetReplyID replyID, const BaseAppMgrInterface::AddArgs & args )
{
	FV_ASSERT( srcAddr == args.addrForCells );

	if (!m_kCellAppMgr.Channel().IsEstablished() || !m_bHasInitData)
	{
		FV_INFO_MSG( "BaseAppMgr::add: Not allowing BaseApp at %s to register "
				"yet\n", srcAddr.c_str() );

		FvNetChannelSender sender( FvBaseAppMgr::GetChannel( srcAddr ) );
		sender.Bundle().StartReply( replyID );

		return;
	}

	if (!m_bAllowNewBaseApps || (m_eShutDownStage != SHUTDOWN_NONE))
		return;

	if (m_kBaseApps.empty())
	{
		FvNetBundle	& bundle = m_kCellAppMgr.Bundle();
		CellAppMgrInterface::SetBaseAppArgs setBaseAppArgs;
		setBaseAppArgs.addr = args.addrForCells;
		bundle << setBaseAppArgs;
		m_kCellAppMgr.Send();

		m_kBestBaseAppAddr = args.addrForCells;
	}

	FvBaseAppID id = this->GetNextID();
	FvBaseApp * pBaseApp =
		new FvBaseApp( args.addrForCells, args.addrForClients, id );
	m_kBaseApps[ srcAddr ].Set( pBaseApp );

	std::string cellNubStr = (char*)pBaseApp->Addr();
	FV_DEBUG_MSG( "BaseAppMgr::add:\n"
			"\tAllocated id    = %u\n"
			"\tBaseApps in use = %lu\n"
			"\tInternal nub    = %s\n"
			"\tExternal nub    = %s\n",
		id,
		m_kBaseApps.size(),
		cellNubStr.c_str(),
		pBaseApp->ExternalAddr().c_str() );

	FvNetBundle & bundle = pBaseApp->Bundle();
	bundle.StartReply( replyID );

	FvBaseAppInitData initData;
	initData.id = id;
	initData.time = m_uiTime;
	initData.isReady = m_bHasStarted;

	bundle << initData;

	if (!m_kGlobalBases.empty())
	{
		FvGlobalBases::iterator iter = m_kGlobalBases.begin();

		while (iter != m_kGlobalBases.end())
		{
			bundle.StartMessage( BaseAppIntInterface::AddGlobalBase );
			bundle << iter->first << iter->second;

			++iter;
		}
	}

	if (!m_kSharedBaseAppData.empty())
	{
		FvSharedData::iterator iter = m_kSharedBaseAppData.begin();

		while (iter != m_kSharedBaseAppData.end())
		{
			bundle.StartMessage( BaseAppIntInterface::SetSharedData );
			bundle << SharedDataType( SHARED_DATA_TYPE_BASE_APP ) <<
				iter->first << iter->second;
			++iter;
		}
	}

	if (!m_kSharedGlobalData.empty())
	{
		FvSharedData::iterator iter = m_kSharedGlobalData.begin();

		while (iter != m_kSharedGlobalData.end())
		{
			bundle.StartMessage( BaseAppIntInterface::SetSharedData );
			bundle << SharedDataType( SHARED_DATA_TYPE_GLOBAL ) <<
				iter->first << iter->second;
			++iter;
		}
	}

	if (m_bUseNewStyleBackup)
	{
		this->AdjustBackupLocations( pBaseApp->Addr(), true );
	}
	else
	{
		FvBackupBaseApp * pBestBackup = this->FindBestBackup( *pBaseApp );

		if (pBestBackup != NULL)
		{
			pBestBackup->Backup( *pBaseApp );
		}
	}

	pBaseApp->Send();
}


void FvBaseAppMgr::AddBackup( const FvNetAddress & srcAddr,
	FvNetReplyID replyID,
	const BaseAppMgrInterface::AddBackupArgs & args )
{
	if (!m_bAllowNewBaseApps || (m_eShutDownStage != SHUTDOWN_NONE))
		return;

	if (m_bUseNewStyleBackup)
	{
		FV_ERROR_MSG( "BaseAppMgr::addBackup: "
				"Backup BaseApps not used in new-style BaseApp backup (%s).\n",
			(char *)srcAddr );
		return;
	}

	FvBaseAppID id = this->GetNextID();
	FV_TRACE_MSG( "BaseAppMgr::addBackup: %s id = %u\n",
		(char *)args.addr, id );

	FV_ASSERT( srcAddr == args.addr );

	FvBackupBaseApp * pBaseApp = new FvBackupBaseApp( args.addr, id );
	m_kBackupBaseApps[ srcAddr ].Set( pBaseApp );

	FvNetBundle & bundle = pBaseApp->Bundle();
	bundle.StartReply( replyID );
	bundle << id << m_uiTime << m_bHasStarted;

	pBaseApp->Send();

	this->CheckBackups();
}

namespace
{
	template <class T>
	struct LoadCmp
	{
		inline bool operator()( T * p1, T * p2 )
		{
			return p1->Load() < p2->Load();
		}
	};
}

void FvBaseAppMgr::UpdateCreateBaseInfo()
{
	typedef std::vector< FvBaseApp * > BaseAppsVec;
	BaseAppsVec apps;
	apps.reserve( m_kBaseApps.size() );

	FvBaseApps::iterator iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		apps.push_back( iter->second.Get() );
		++iter;
	}

	std::sort( apps.begin(), apps.end(), LoadCmp<FvBaseApp>() );

	int totalSize = apps.size();
	int destSize = int(totalSize/m_fCreateBaseRatio + 0.99f);
	destSize = std::min( totalSize, std::max( 1, destSize ) );

	std::random_shuffle( apps.begin() + destSize, apps.end() );

	for (size_t i = 0; i < apps.size(); ++i)
	{
		FvNetBundle & bundle = apps[ i ]->Bundle();
		int destIndex = i % destSize;

		bundle.StartMessage( BaseAppIntInterface::SetCreateBaseInfo );
		bundle << apps[ destIndex ]->Addr();

		apps[ i ]->Send();
	}
}


void FvBaseAppMgr::RecoverBaseApp( const FvNetAddress & srcAddr,
			const FvNetUnpackedMessageHeader & /*header*/,
			FvBinaryIStream & data )
{
	if (!m_bIsRecovery)
	{
		FV_WARNING_MSG( "BaseAppMgr::recoverBaseApp: "
				"Recovering when we were not started with -recover\n" );
		m_bIsRecovery = true;
	}

	FvNetAddress		addrForCells;
	FvNetAddress		addrForClients;
	FvNetAddress		backupAddress;
	FvBaseAppID				id;

	data >> addrForCells >> addrForClients >> backupAddress >> id >> m_uiTime;

	this->StartTimer();

	FV_DEBUG_MSG( "BaseAppMgr::recoverBaseApp: %s, id = %u\n",
		(char *)addrForCells, id );

	m_iLastBaseAppID = std::max( id, m_iLastBaseAppID );

	FvBaseApp * pBaseApp = new FvBaseApp( addrForCells, addrForClients, id );
	m_kBaseApps[ addrForCells ].Set( pBaseApp );

	data >> pBaseApp->BackupHash() >> pBaseApp->NewBackupHash();

	if (backupAddress.m_uiIP != 0)
	{
		FvBackupBaseApps::iterator backupIter =
			m_kBackupBaseApps.find( backupAddress );

		if (backupIter != m_kBackupBaseApps.end())
		{
			backupIter->second->m_kBackedUp.insert( pBaseApp );
			pBaseApp->SetBackup( backupIter->second.Get() );
		}
		else
		{
			std::string backupAddr = (char *)backupAddress;
			FV_DEBUG_MSG( "BaseAppMgr::recoverBaseApp: "
					"Not yet setting backup of %s to %s\n",
				(char *)pBaseApp->Addr(), backupAddr.c_str() );
		}
	}

	{
		FvUInt32 numEntries;
		data >> numEntries;

		std::string key;
		std::string value;

		for (FvUInt32 i = 0; i < numEntries; ++i)
		{
			data >> key >> value;
			m_kSharedBaseAppData[ key ] = value;
		}
	}

	{
		FvUInt32 numEntries;
		data >> numEntries;

		std::string key;
		std::string value;

		for (FvUInt32 i = 0; i < numEntries; ++i)
		{
			data >> key >> value;
			m_kSharedGlobalData[ key ] = value;
		}
	}

	while (data.RemainingLength() > 0)
	{
		std::pair< std::string, FvEntityMailBoxRef > value;
		data >> value.first >> value.second;

		FV_ASSERT( value.second.m_kAddr == srcAddr );

		if (!m_kGlobalBases.insert( value ).second)
		{
			FV_WARNING_MSG( "BaseAppMgr::recoverBaseApp: "
					"Try to recover global base %s twice\n",
				value.first.c_str() );
		}
	}
}


void FvBaseAppMgr::OldRecoverBackupBaseApp( const FvNetAddress & srcAddr,
			const FvNetUnpackedMessageHeader & /*header*/,
			FvBinaryIStream & data )
{
	FvNetAddress addr;
	FvBaseAppID id;
	data >> addr >> id;
	FV_DEBUG_MSG( "BaseAppMgr::old_recoverBackupBaseApp: %s\n", (char *)addr );
	m_iLastBaseAppID = std::max( id, m_iLastBaseAppID );

	FV_ASSERT( addr == srcAddr );

	FvBackupBaseApp * pBaseApp = new FvBackupBaseApp( addr, id );
	m_kBackupBaseApps[ addr ].Set( pBaseApp );

	while (data.RemainingLength() >= int(sizeof( FvNetAddress )))
	{
		FvNetAddress backupAddr;
		data >> backupAddr;

		FvBaseApps::iterator iter = m_kBaseApps.find( backupAddr );

		if (iter != m_kBaseApps.end())
		{
			pBaseApp->m_kBackedUp.insert( iter->second.Get() );
			iter->second->SetBackup( pBaseApp );
		}
		else
		{
			std::string backupAddrStr = (char *)backupAddr;
			FV_DEBUG_MSG( "BaseAppMgr::old_recoverBackupBaseApp: "
					"Not yet setting backup of %s to %s\n",
				(char *)iter->second->Addr(), backupAddrStr.c_str() );
		}
	}
}


void FvBaseAppMgr::CheckBackups()
{
	FvBaseApps::iterator iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		if (iter->second->GetBackup() == NULL)
		{
			FvBackupBaseApp * pBackup = this->FindBestBackup( *iter->second );

			if (pBackup != NULL)
			{
				pBackup->Backup( *iter->second );
			}
		}

		++iter;
	}
}


void FvBaseAppMgr::Del( const BaseAppMgrInterface::DelArgs & args,
					 const FvNetAddress & addr )
{
	FV_TRACE_MSG( "BaseAppMgr::del: %u\n", args.id );

	if (this->OnBaseAppDeath( addr, false ))
	{
		FV_DEBUG_MSG( "BaseAppMgr::del: now have %lu base apps\n",
				m_kBaseApps.size() );
	}
	else if (this->OnBackupBaseAppDeath( addr ))
	{
		FV_DEBUG_MSG( "BaseAppMgr::del: Now have %lu backup base apps\n",
			m_kBackupBaseApps.size() );
	}
	else
	{
		FV_ERROR_MSG( "BaseAppMgr: Error deleting %s id = %u\n",
			(char *)addr, args.id );
	}
}


void FvBaseAppMgr::AdjustBackupLocations( const FvNetAddress & addr,
		bool isAdd )
{
	FvBaseApp * pNewBaseApp = NULL;

	if (isAdd)
	{
		pNewBaseApp = m_kBaseApps[ addr ].Get();
		FV_ASSERT( pNewBaseApp );
	}

	FvBaseApps::iterator iter = m_kBaseApps.begin();

	bool hadMultipleBaseAppMachines = m_bHasMultipleBaseAppMachines;

	m_bHasMultipleBaseAppMachines = false;

	while (iter != m_kBaseApps.end())
	{
		if (m_kBaseApps.begin()->first.m_uiIP != iter->first.m_uiIP)
		{
			m_bHasMultipleBaseAppMachines = true;
			break;
		}
		iter++;
	}

	if (m_bHasMultipleBaseAppMachines && !hadMultipleBaseAppMachines)
	{
		FV_INFO_MSG( "Baseapps detected on multiple machines, switching to "
				  "multi-machine backup strategy.\n" );
	}

	if (!m_bHasMultipleBaseAppMachines && hadMultipleBaseAppMachines)
	{
		FV_INFO_MSG( "Baseapps detected on only one machine, falling back to "
				  "single-machine backup strategy.\n" );
	}

	iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		if (addr != iter->first)
		{
			FvBaseApp & baseApp = *iter->second;

			if (baseApp.NewBackupHash().empty())
			{
				baseApp.NewBackupHash() = baseApp.BackupHash();
			}
			else
			{
				FV_WARNING_MSG( "BaseAppMgr::adjustBackupLocations: "
							"%s was still transitioning to a new hash.\n",
						(char *)iter->first );
			}

			if (m_bHasMultipleBaseAppMachines && !hadMultipleBaseAppMachines)
			{
				FV_ASSERT( isAdd );
				baseApp.NewBackupHash().clear();
			}

			else if (!m_bHasMultipleBaseAppMachines && hadMultipleBaseAppMachines)
			{
				FvBaseApps::iterator inner = m_kBaseApps.begin();

				while (inner != m_kBaseApps.end())
				{
					if ((inner != iter) && (inner->first != addr))
					{
						baseApp.NewBackupHash().push_back( inner->first );
					}
					++inner;
				}
			}

			if (isAdd)
			{
				if ((addr.m_uiIP != iter->first.m_uiIP) || !m_bHasMultipleBaseAppMachines)
				{
					baseApp.NewBackupHash().push_back( addr );
					pNewBaseApp->NewBackupHash().push_back( iter->first );
				}
			}
			else
			{
				baseApp.NewBackupHash().erase( addr );

				if (baseApp.BackupHash().erase( addr ))
				{
					baseApp.BackupHash().clear();
				}
			}

			FvNetBundle & bundle = baseApp.Bundle();
			bundle.StartMessage( BaseAppIntInterface::SetBackupBaseApps );
			bundle << baseApp.NewBackupHash();
			baseApp.Send();
		}

		++iter;
	}

	if (isAdd)
	{
		FvNetBundle & bundle = pNewBaseApp->Bundle();
		bundle.StartMessage( BaseAppIntInterface::SetBackupBaseApps );
		bundle << pNewBaseApp->NewBackupHash();
		pNewBaseApp->Send();
	}
}


bool FvBaseAppMgr::OnBaseAppDeath( const FvNetAddress & addr,
							   bool shouldRestore )
{
	shouldRestore = shouldRestore && !m_bUseNewStyleBackup;

	FvBaseApps::iterator iter = m_kBaseApps.find( addr );

	if (iter != m_kBaseApps.end())
	{
		FV_INFO_MSG( "BaseAppMgr::onBaseAppDeath: baseapp%02d @ %s\n",
			iter->second->Id(), (char *)addr );

		FvBaseApp & baseApp = *iter->second;
		FvBackupBaseApp * pBackup = baseApp.GetBackup();
		bool controlledShutDown = false;

		if (pBackup != NULL)
		{
			pBackup->StopBackup( baseApp, !shouldRestore );
		}
		else if (shouldRestore)
		{
			FV_ERROR_MSG( "BaseAppMgr::onBaseAppDeath: "
				"Unable to restore %s. No backup available.\n", (char *)addr );
			shouldRestore = false;

			if (m_bShutDownServerOnBadState)
			{
				controlledShutDown = true;
			}
		}

		if (m_bShouldHardKillDeadBaseApps)
		{
			FV_INFO_MSG( "BaseAppMgr::onBaseAppDeath: Sending SIGQUIT to %s\n",
					(char *) baseApp.Addr() );
			if (!SendSignalViaMachined( baseApp.Addr(), SIGQUIT, m_kNub ))
			{
				FV_ERROR_MSG( "BaseAppMgr::onBaseAppDeath: Failed to send "
						"SIGQUIT to %s\n", (char *) baseApp.Addr() );
			}
		}


		if (shouldRestore)
		{
			baseApp.Id( pBackup->Id() );

			FvNetBundle & bundle = pBackup->Bundle();
			bundle.StartMessage( BaseAppIntInterface::OldRestoreBaseApp );
			bundle << baseApp.Addr() << baseApp.ExternalAddr();

			pBackup->Send();

			this->OnBackupBaseAppDeath( pBackup->Addr() );

			this->CheckBackups();
		}
		else
		{
			if (m_bShutDownServerOnBaseAppDeath)
			{
				controlledShutDown = true;
				FV_NOTICE_MSG( "BaseAppMgr::onBaseAppDeath: "
						"shutDownServerOnBaseAppDeath is enabled. "
						"Shutting down server\n" );
			}
			else if (baseApp.BackupHash().empty())
			{
				if (baseApp.NewBackupHash().empty())
				{
					FV_ERROR_MSG( "BaseAppMgr::onBackupBaseAppDeath: "
							"No backup for %s\n", (char *)iter->first );
				}
				else
				{
					FV_ERROR_MSG( "BaseAppMgr::onBackupBaseAppDeath: "
							"Backup not ready for %s\n", (char *)iter->first );
				}

				if (m_bShutDownServerOnBadState)
				{
					controlledShutDown = true;
				}
			}

			{
				FvNetBundle & bundle = m_kCellAppMgr.Bundle();
				bundle.StartMessage(
						CellAppMgrInterface::HandleBaseAppDeath );
				bundle << iter->first;
				bundle << baseApp.BackupHash();
				m_kCellAppMgr.Send();
			}

			if (!m_bUseNewStyleBackup)
			{
				this->CheckGlobalBases( addr );
			}

			unsigned int numBaseAppsAlive = m_kBaseApps.size() - 1;
			if (numBaseAppsAlive > 0 && !controlledShutDown)
			{
				FvMemoryOStream args;
				args << iter->first << baseApp.BackupHash();

				this->SendToBaseApps(
					BaseAppIntInterface::HandleBaseAppDeath, args, &baseApp );

				m_kDeadBaseAppAddr = iter->first;
				m_uiArchiveCompleteMsgCounter = numBaseAppsAlive;
			}

			{
				FvGlobalBases::iterator gbIter = m_kGlobalBases.begin();

				while (gbIter != m_kGlobalBases.end())
				{
					FvEntityMailBoxRef & mailbox = gbIter->second;

					if (mailbox.m_kAddr == addr)
					{
						FvNetAddress newAddr =
							baseApp.BackupHash().AddressFor( mailbox.m_iID );
						mailbox.m_kAddr.m_uiIP = newAddr.m_uiIP;
						mailbox.m_kAddr.m_uiPort = newAddr.m_uiPort;
					}

					++gbIter;
				}
			}

			m_kBaseApps.erase( iter );

			if (m_bUseNewStyleBackup)
			{
				this->AdjustBackupLocations( addr, false );
			}
		}

		if (controlledShutDown)
		{
			this->ControlledShutDownServer();
		}
		else
		{
			this->UpdateCreateBaseInfo();
		}

		return true;
	}

	return false;
}

bool FvBaseAppMgr::OnBackupBaseAppDeath( const FvNetAddress & addr )
{
	FvBackupBaseApps::iterator iter = m_kBackupBaseApps.find( addr );

	if (iter != m_kBackupBaseApps.end())
	{
		FvBackupBaseApp::FvBackedUpSet backedUpSet = iter->second->m_kBackedUp;
		m_kBackupBaseApps.erase( iter );

		FvBackupBaseApp::FvBackedUpSet::iterator iter = backedUpSet.begin();

		while (iter != backedUpSet.end())
		{
			FvBaseApp & baseApp = **iter;

			baseApp.SetBackup( NULL );
			FvNetBundle & bundle = baseApp.Bundle();
			bundle.StartMessage( BaseAppIntInterface::OldSetBackupBaseApp );
			bundle << FvNetAddress( 0, 0 );
			baseApp.Send();

			FvBackupBaseApp * pBestBackup = this->FindBestBackup( baseApp );

			if (pBestBackup != NULL)
			{
				pBestBackup->Backup( baseApp );
			}
			else
			{
				FV_WARNING_MSG( "BaseAppMgr::onBackupBaseAppDeath: "
					"No backup available for %s\n",  (char *)baseApp.Addr() );
			}

			++iter;
		}

		return true;
	}

	return false;
}


void FvBaseAppMgr::RemoveControlledShutdownBaseApp(
		const FvNetAddress & addr )
{
	FV_TRACE_MSG( "BaseAppMgr::removeControlledShutdownBaseApp: %s\n",
			addr.c_str() );

	m_kBaseApps.erase( addr );
}


void FvBaseAppMgr::ShutDown( bool shutDownOthers )
{
	FV_INFO_MSG( "BaseAppMgr::shutDown: shutDownOthers = %d\n",
			shutDownOthers );
	m_bShouldShutDownOthers = shutDownOthers;
	m_kNub.BreakProcessing();
}


void FvBaseAppMgr::ShutDown( const BaseAppMgrInterface::ShutDownArgs & args )
{
	this->ShutDown( args.shouldShutDownOthers );
}


void FvBaseAppMgr::ControlledShutDown(
		const BaseAppMgrInterface::ControlledShutDownArgs & args )
{
	FV_INFO_MSG( "BaseAppMgr::controlledShutDown: stage = %d\n", args.stage );

	switch (args.stage)
	{
		case SHUTDOWN_REQUEST:
		{
			FvNetBundle & bundle = m_kCellAppMgr.Bundle();
			CellAppMgrInterface::ControlledShutDownArgs args;
			args.stage = SHUTDOWN_REQUEST;
			bundle << args;
			m_kCellAppMgr.Send();
			break;
		}

		case SHUTDOWN_INFORM:
		{
			m_eShutDownStage = args.stage;
			m_uiShutDownTime = args.shutDownTime;

			{
				SyncControlledShutDownHandler * pHandler =
					new SyncControlledShutDownHandler(
						args.stage, m_kBaseApps.size() + m_kBackupBaseApps.size() );

				for (FvBackupBaseApps::iterator it = m_kBackupBaseApps.begin();
						it != m_kBackupBaseApps.end(); it++)
				{
					FvBackupBaseApp & baseApp = *it->second;
					FvNetBundle & bundle = baseApp.Bundle();
					bundle.StartRequest(
							BaseAppIntInterface::ControlledShutDown, pHandler );
					bundle << args.stage;
					bundle << args.shutDownTime;

					baseApp.Send();
				}

				FvMemoryOStream payload;
				payload << args.stage << args.shutDownTime;
				this->SendToBaseApps( BaseAppIntInterface::ControlledShutDown,
					payload, NULL, pHandler );
			}

			break;
		}

		case SHUTDOWN_PERFORM:
		{
			this->StartAsyncShutDownStage( SHUTDOWN_DISCONNECT_PROXIES );
			break;
		}

		case SHUTDOWN_TRIGGER:
		{
			this->ControlledShutDownServer();
			break;
		}

		case SHUTDOWN_NONE:
		case SHUTDOWN_DISCONNECT_PROXIES:
			break;
	}
}


void FvBaseAppMgr::StartAsyncShutDownStage( FvShutDownStage stage )
{
	std::vector< FvNetAddress > addrs;
	addrs.reserve( m_kBaseApps.size() + m_kBackupBaseApps.size() );

	for (FvBackupBaseApps::iterator it = m_kBackupBaseApps.begin();
			it != m_kBackupBaseApps.end(); it++)
	{
		addrs.push_back( it->first );
	}

	FvBaseApps::iterator iter = m_kBaseApps.begin();

	while (iter != m_kBaseApps.end())
	{
		addrs.push_back( iter->first );

		++iter;
	}

	new AsyncControlledShutDownHandler( stage, addrs );
}


void FvBaseAppMgr::ControlledShutDownServer()
{
	FvNetAddress loginAppAddr;
	FvNetReason reason = m_kNub.FindInterface( "LoginIntInterface", -1, loginAppAddr );

	if (reason == FV_NET_REASON_SUCCESS)
	{
		FvNetChannelSender sender( FvBaseAppMgr::GetChannel( loginAppAddr ) );
		FvNetBundle & bundle = sender.Bundle();

		bundle.StartMessage( LoginIntInterface::ControlledShutDown );

		FV_INFO_MSG( "BaseAppMgr::controlledShutDownServer: "
			"Triggering server shutdown via LoginApp @ %s\n",
			loginAppAddr.c_str() );

		return;
	}
	else
	{
		FV_ERROR_MSG( "BaseAppMgr::controlledShutDownServer: "
			"Couldn't find a LoginApp to trigger server shutdown\n" );
	}

	if (this->DBMgr().Channel().IsEstablished())
	{
		FvNetBundle & bundle = this->DBMgr().Bundle();
		DBInterface::ControlledShutDownArgs::start( bundle ).stage =
			SHUTDOWN_REQUEST;
		this->DBMgr().Send();

		FV_INFO_MSG( "BaseAppMgr::controlledShutDownServer: "
				"Triggering server shutdown via DBMgr\n" );
		return;
	}
	else
	{
		FV_ERROR_MSG( "BaseAppMgr::controlledShutDownServer: "
			"Couldn't find the DBMgr to trigger server shutdown\n" );
	}

	BaseAppMgrInterface::ControlledShutDownArgs args;
	args.stage = SHUTDOWN_REQUEST;
	FV_INFO_MSG( "BaseAppMgr::controlledShutDownServer: "
		"Starting controlled shutdown here (no LoginApp or DBMgr found)\n" );
	this->ControlledShutDown( args );
}

void FvBaseAppMgr::RequestHasStarted( const FvNetAddress & srcAddr,
		const FvNetUnpackedMessageHeader& header,
		FvBinaryIStream & data )
{
	FvNetChannelSender sender( FvBaseAppMgr::GetChannel( srcAddr ) );
	FvNetBundle & bundle = sender.Bundle();

	bundle.StartReply( header.m_iReplyID );
	bundle << m_bHasStarted;

	return;
}


void FvBaseAppMgr::InitData( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
{
	if (m_bHasInitData)
	{
		FV_ERROR_MSG( "BaseAppMgr::initData: Ignored subsequent initialisation "
				"data from %s\n", addr.c_str() );
		return;
	}

	FvTimeStamp gameTime;
	data >> gameTime;
	if (m_uiTime == 0)
	{
		m_uiTime = gameTime;
		FV_INFO_MSG( "BaseAppMgr::initData: game time=%.1f\n",
				this->GameTimeInSeconds() );
	}

	FvInt32	maxAppID;
	data >> maxAppID;
	if (maxAppID > m_iLastBaseAppID)
	{
		m_iLastBaseAppID = maxAppID;
		FV_INFO_MSG( "BaseAppMgr::initData: lastBaseAppIDAllocated=%d\n",
				m_iLastBaseAppID );
	}

	m_bHasInitData = true;
}


void FvBaseAppMgr::SpaceDataRestore( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
{
	FV_ASSERT( !m_bHasStarted && m_bHasInitData );

	{
		FvNetBundle & bundle = m_kCellAppMgr.Bundle();
		bundle.StartMessage( CellAppMgrInterface::PrepareForRestoreFromDB );
		bundle << m_uiTime;
		bundle.Transfer( data, data.RemainingLength() );
		m_kCellAppMgr.Send();
	}
}


void FvBaseAppMgr::SetSharedData( FvBinaryIStream & data )
{
	bool sendToBaseApps = true;
	SharedDataType dataType;
	std::string key;
	std::string value;
	data >> dataType >> key >> value;

	if (dataType == SHARED_DATA_TYPE_BASE_APP)
	{
		m_kSharedBaseAppData[ key ] = value;
	}
	else if (dataType == SHARED_DATA_TYPE_GLOBAL)
	{
		m_kSharedGlobalData[ key ] = value;
	}
	else if ((dataType == SHARED_DATA_TYPE_GLOBAL_FROM_BASE_APP) ||
		(dataType == SHARED_DATA_TYPE_CELL_APP))
	{
		if (dataType == SHARED_DATA_TYPE_GLOBAL_FROM_BASE_APP)
		{
			dataType = SHARED_DATA_TYPE_GLOBAL;
		}

		FvNetBundle & bundle = m_kCellAppMgr.Bundle();
		bundle.StartMessage( CellAppMgrInterface::SetSharedData );
		bundle << dataType << key << value;
		m_kCellAppMgr.Send();

		sendToBaseApps = false;
	}
	else
	{
		FV_ERROR_MSG( "BaseAppMgr::setSharedData: Invalid dataType %d\n",
				dataType );
		return;
	}

	if (sendToBaseApps)
	{
		FvMemoryOStream payload;
		payload << dataType << key << value;

		this->SendToBaseApps( BaseAppIntInterface::SetSharedData, payload );
		this->SendToBackupBaseApps( BaseAppIntInterface::SetSharedData, payload );
	}
}


void FvBaseAppMgr::DelSharedData( FvBinaryIStream & data )
{
	bool sendToBaseApps = true;
	SharedDataType dataType;
	std::string key;
	data >> dataType >> key;

	if (dataType == SHARED_DATA_TYPE_BASE_APP)
	{
		m_kSharedBaseAppData.erase( key );
	}
	else if (dataType == SHARED_DATA_TYPE_GLOBAL)
	{
		m_kSharedGlobalData.erase( key );
	}
	else if ((dataType == SHARED_DATA_TYPE_GLOBAL_FROM_BASE_APP) ||
		(dataType == SHARED_DATA_TYPE_CELL_APP))
	{
		if (dataType == SHARED_DATA_TYPE_GLOBAL_FROM_BASE_APP)
		{
			dataType = SHARED_DATA_TYPE_GLOBAL;
		}

		FvNetBundle & bundle = m_kCellAppMgr.Bundle();
		bundle.StartMessage( CellAppMgrInterface::DelSharedData );
		bundle << dataType << key;
		m_kCellAppMgr.Send();

		sendToBaseApps = false;
	}
	else
	{
		FV_ERROR_MSG( "BaseAppMgr::delSharedData: Invalid dataType %d\n",
				dataType );
		return;
	}

	FvMemoryOStream payload;
	payload << dataType << key;

	if (sendToBaseApps)
	{
		this->SendToBaseApps( BaseAppIntInterface::DelSharedData, payload );
		this->SendToBackupBaseApps( BaseAppIntInterface::DelSharedData,
			payload );
	}
}


class FinishSetBackupDiffVisitor : public FvBackupHash::DiffVisitor
{
public:
	FinishSetBackupDiffVisitor( const FvNetAddress & realBaseAppAddr ) :
   		m_kRealBaseAppAddr( realBaseAppAddr )
	{}

	virtual void OnAdd( const FvNetAddress & addr,
			FvUInt32 index, FvUInt32 virtualSize, FvUInt32 prime )
	{
		FvBaseApp * pBaseApp = FvBaseAppMgr::Instance().FindBaseApp( addr );

		if (pBaseApp)
		{
			FvNetBundle & bundle = pBaseApp->Bundle();
			BaseAppIntInterface::StartBaseEntitiesBackupArgs & args =
				args.start( bundle );

			args.realBaseAppAddr = m_kRealBaseAppAddr;
			args.index = index;
			args.hashSize = virtualSize;
			args.prime = prime;
			args.isInitial = false;

			pBaseApp->Send();
		}
		else
		{
			FV_ERROR_MSG( "FinishSetBackupDiffVisitor::onAdd: No BaseApp for %s\n",
					addr.c_str() );
		}
	}

	virtual void OnChange( const FvNetAddress & addr,
			FvUInt32 index, FvUInt32 virtualSize, FvUInt32 prime )
	{
		this->OnAdd( addr, index, virtualSize, prime );
	}

	virtual void OnRemove( const FvNetAddress & addr,
			FvUInt32 index, FvUInt32 virtualSize, FvUInt32 prime )
	{
		FvBaseApp * pBaseApp = FvBaseAppMgr::Instance().FindBaseApp( addr );

		if (pBaseApp)
		{
			FvNetBundle & bundle = pBaseApp->Bundle();
			BaseAppIntInterface::StopBaseEntitiesBackupArgs & args =
				args.start( bundle );

			args.realBaseAppAddr = m_kRealBaseAppAddr;
			args.index = index;
			args.hashSize = virtualSize;
			args.prime = prime;
			args.isPending = false;

			pBaseApp->Send();
		}
	}

private:
	FvNetAddress m_kRealBaseAppAddr;
};


void FvBaseAppMgr::UseNewBackupHash( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
{
	FvBackupHash backupHash;
	FvBackupHash newBackupHash;

	data >> backupHash >> newBackupHash;

	FvBaseApp * pBaseApp = this->FindBaseApp( addr );

	if (pBaseApp)
	{
		FinishSetBackupDiffVisitor visitor( addr );
		backupHash.diff( newBackupHash, visitor );
		pBaseApp->BackupHash().swap( newBackupHash );
		pBaseApp->NewBackupHash().clear();
	}
	else
	{
		FV_WARNING_MSG( "BaseAppMgr::useNewBackupHash: "
				"No BaseApp %s. It may have just died.?\n", (char *)addr );
	}
}


void FvBaseAppMgr::InformOfArchiveComplete( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
{
	FvBaseApp * pBaseApp = this->FindBaseApp( addr );

	if (!pBaseApp)
	{
		FV_ERROR_MSG( "BaseAppMgr::informOfArchiveComplete: No BaseApp with "
				"address %s\n",	(char *) addr );
		return;
	}

	FvNetAddress deadBaseAppAddr;
	data >> deadBaseAppAddr;

	if (deadBaseAppAddr != m_kDeadBaseAppAddr)
	{
		return;
	}

	--m_uiArchiveCompleteMsgCounter;

	if (m_uiArchiveCompleteMsgCounter == 0)
	{
		FvNetBundle & bundle = this->DBMgr().Bundle();
		bundle.StartMessage( DBInterface::UpdateSecondaryDBs );

		bundle << FvUInt32(m_kBaseApps.size());

		for (FvBaseApps::iterator iter = m_kBaseApps.begin();
			   iter != m_kBaseApps.end(); ++iter)
		{
			bundle << iter->second->Id();
		}

		this->DBMgr().Send();
	}
}


void FvBaseAppMgr::Startup( const FvNetAddress & addr,
		const FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
{
	if (m_bHasStarted)
	{
		FV_WARNING_MSG( "BaseAppMgr::ready: Already ready.\n" );
		return;
	}

	FV_INFO_MSG( "BaseAppMgr is starting\n" );

	this->StartTimer();

	{
		FvNetBundle & bundle = m_kCellAppMgr.Bundle();
		bundle.StartMessage( CellAppMgrInterface::Startup );
		m_kCellAppMgr.Send();
	}

	{
		if (m_kBaseApps.empty())
		{
			FV_CRITICAL_MSG( "BaseAppMgr::ready: "
				"No Base apps running when started.\n" );
		}

		bool bootstrap = true;
		for (FvBaseApps::iterator it = m_kBaseApps.begin();	it != m_kBaseApps.end(); ++it)
		{
			FvBaseApp & baseApp = *it->second;
			FvNetBundle & bundle = baseApp.Bundle();
			bundle.StartMessage( BaseAppIntInterface::Startup );
			bundle << bootstrap;

			baseApp.Send();
			bootstrap = false;
		}

		bootstrap = false;
		for (FvBackupBaseApps::iterator it = m_kBackupBaseApps.begin();
			it != m_kBackupBaseApps.end();
			it++)
		{
			FvBackupBaseApp & backup = *it->second;
			FvNetBundle & bundle = backup.Bundle();
			bundle.StartMessage( BaseAppIntInterface::Startup );
			bundle << bootstrap;

			backup.Send();
		}
	}
}

void FvBaseAppMgr::StartTimer()
{
	if (!m_bHasStarted)
	{
		m_bHasStarted = true;
		FvNetTimerID gtid = m_kNub.RegisterTimer( 1000000/m_iUpdateHertz,
				this,
				reinterpret_cast< void * >( TIMEOUT_GAME_TICK ) );
		m_pkTimeKeeper = new FvTimeKeeper( m_kNub, gtid, m_uiTime, m_iUpdateHertz,
			&m_kCellAppMgr.Addr(), &CellAppMgrInterface::GameTimeReading );
	}
}


class CheckStatusReplyHandler : public ForwardingReplyHandler
{
public:
	CheckStatusReplyHandler( const FvNetAddress & srcAddr,
			FvNetReplyID replyID ) :
		ForwardingReplyHandler( srcAddr, replyID ) {};

	virtual void PrependData( FvNetBundle & bundle,
		   FvBinaryIStream & data )
	{
		FvUInt8 isOkay;
		data >> isOkay;

		bundle << isOkay << FvBaseAppMgr::Instance().NumBaseApps();
	}
};


void FvBaseAppMgr::CheckStatus( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
{
	if (m_kCellAppMgr.Channel().IsEstablished())
	{
		FvNetBundle & bundle = m_kCellAppMgr.Bundle();
		bundle.StartRequest( CellAppMgrInterface::CheckStatus,
			   new CheckStatusReplyHandler( addr, header.m_iReplyID ) );
		bundle.Transfer( data, data.RemainingLength() );
		m_kCellAppMgr.Send();
	}
	else
	{
		FV_IF_NOT_ASSERT_DEV( this->DBMgr().Addr() == addr )
		{
			return;
		}

		FvNetBundle & bundle = this->DBMgr().Bundle();
		bundle.StartReply( header.m_iReplyID );
		bundle << FvUInt8(false) << this->NumBaseApps() << 0;
		bundle << "No CellAppMgr";
		this->DBMgr().Send();
	}
}


void FvBaseAppMgr::HandleCellAppMgrBirth(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);

	FV_INFO_MSG( "BaseAppMgr::handleCellAppMgrBirth: %s\n", kAddr.c_str() );

	if (!m_kCellAppMgr.Channel().IsEstablished() && !kAddr.IsNone())
	{
		FV_INFO_MSG( "BaseAppMgr::handleCellAppMgrBirth: "
					"CellAppMgr is now ready.\n" );
	}

	m_kCellAppMgr.Addr( kAddr );

	m_kBestBaseAppAddr.m_uiIP = 0;
	m_kBestBaseAppAddr.m_uiPort = 0;
}


void FvBaseAppMgr::HandleBaseAppMgrBirth(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);

	if (kAddr != m_kNub.Address())
	{
		FV_WARNING_MSG( "BaseAppMgr::handleBaseAppMgrBirth: %s\n",
				kAddr.c_str() );
		this->ShutDown( false );
	}
}


void FvBaseAppMgr::HandleCellAppDeath( const FvNetAddress & /*addr*/,
		const FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data )
{
	FV_TRACE_MSG( "BaseAppMgr::handleCellAppDeath:\n" );

	FvMemoryOStream payload;
	payload.Transfer( data, data.RemainingLength() );

	this->SendToBaseApps( BaseAppIntInterface::HandleCellAppDeath, payload );
}


void FvBaseAppMgr::HandleBaseAppDeath(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);

	this->HandleBaseAppDeath( kAddr );
}


void FvBaseAppMgr::HandleBaseAppDeath( const FvNetAddress & addr )
{
	if (m_eShutDownStage != SHUTDOWN_NONE)
		return;

	FV_INFO_MSG( "BaseAppMgr::handleBaseAppDeath: dead app on %s\n", (char*)addr );

	if (!this->OnBaseAppDeath( addr, true ))
	{
		this->OnBackupBaseAppDeath( addr );
	}
}


void FvBaseAppMgr::CreateBaseEntity( const FvNetAddress & srcAddr,
		const FvNetUnpackedMessageHeader& header,
		FvBinaryIStream & data )
{
	//! add by Uman, 20100602, 好像不会被调用的
	FV_ASSERT(0);

	FvBaseApp * pBest = this->FindBestBaseApp();

	if (pBest)
	{
		FvNetBundle & bundle = pBest->Bundle();
		bundle.StartRequest( BaseAppIntInterface::CreateBaseWithCellData,
			new ForwardingReplyHandler( srcAddr, header.m_iReplyID ) );
		bundle.Transfer( data, data.RemainingLength() );
		pBest->Send();
	}
	else
	{
		FvNetChannelSender sender( FvBaseAppMgr::GetChannel( srcAddr ) );
		FvNetBundle & bundle = sender.Bundle();

		bundle.StartReply( header.m_iReplyID );
		bundle << (FvUInt8)0;
		bundle << "No proxy could be found to add to.";

		return;
	}
}


void FvBaseAppMgr::RegisterBaseGlobally( const FvNetAddress & srcAddr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
{
	FvBaseApp * pSender = this->FindBaseApp( srcAddr );
	FV_IF_NOT_ASSERT_DEV( pSender )
	{
		FV_ERROR_MSG( "BaseAppMgr::registerBaseGlobally: "
			"Got message from unregistered app @ %s, registration aborted\n",
			srcAddr.c_str() );

		return;
	}

	std::pair< std::string, FvEntityMailBoxRef > value;

	data >> value.first >> value.second;

	FV_INFO_MSG( "BaseAppMgr::registerBaseGlobally: %s\n", value.first.c_str() );

	FvInt8 successCode = 0;

	if (m_kGlobalBases.insert( value ).second)
	{
		successCode = 1;

		FvMemoryOStream args;
		args << value.first << value.second;

		this->SendToBaseApps( BaseAppIntInterface::AddGlobalBase,
			args, pSender );
		SendToCellAppMgr(CellAppMgrInterface::AddGlobalBase, args);
	}

	FvNetBundle & bundle = pSender->Bundle();
	bundle.StartReply( header.m_iReplyID );
	bundle << successCode;
	pSender->Send();
}


void FvBaseAppMgr::CheckGlobalBases( const FvNetAddress & deadBaseAppAddr )
{
	FvBaseApp & deadBaseApp = *this->FindBaseApp( deadBaseAppAddr );
	std::vector< std::string > deadGlobalBases;

	for (FvGlobalBases::iterator iter = m_kGlobalBases.begin();
		   iter != m_kGlobalBases.end(); ++iter)
	{
		if (iter->second.m_kAddr == deadBaseAppAddr)
		{
			deadGlobalBases.push_back( iter->first );
		}
	}

	while (!deadGlobalBases.empty())
	{
		std::string deadGB = deadGlobalBases.back();

		if (m_kGlobalBases.erase( deadGB ))
		{
			FvMemoryOStream args;
			args << deadGB;
			this->SendToBaseApps( BaseAppIntInterface::DelGlobalBase,
				args, &deadBaseApp );
			SendToCellAppMgr(CellAppMgrInterface::DelGlobalBase, args);
		}
		else
		{
			FV_ERROR_MSG( "BaseAppMgr::checkGlobalBases: Unable to erase %s\n",
					deadGB.c_str() );
		}
		deadGlobalBases.pop_back();
	}
}


void FvBaseAppMgr::DeregisterBaseGlobally( const FvNetAddress & srcAddr,
			const FvNetUnpackedMessageHeader & /*header*/,
			FvBinaryIStream & data )
{
	std::string label;
	data >> label;

	FV_INFO_MSG( "BaseAppMgr::delGlobalBase: %s\n", label.c_str() );

	if (m_kGlobalBases.erase( label ))
	{
		FvBaseApp * pSrc = this->FindBaseApp( srcAddr );
		FvMemoryOStream payload;
		payload << label;

		this->SendToBaseApps( BaseAppIntInterface::DelGlobalBase,
				payload, pSrc );
		SendToCellAppMgr(CellAppMgrInterface::DelGlobalBase, payload);
	}
	else
	{
		FV_ERROR_MSG( "BaseAppMgr::delGlobalBase: Failed to erase %s\n",
			label.c_str() );
	}
}


FvNetChannelOwner *
		FvBaseAppMgr::FindChannelOwner( const FvNetAddress & addr )
{
	{
		FvBaseApps::iterator iter = m_kBaseApps.find( addr );

		if (iter != m_kBaseApps.end())
		{
			return iter->second.Get();
		}
	}

	{
		FvBackupBaseApps::iterator iter = m_kBackupBaseApps.find( addr );

		if (iter != m_kBackupBaseApps.end())
		{
			return iter->second.Get();
		}
	}

	return NULL;
}


void FvBaseAppMgr::RunScript( const FvNetAddress & srcAddr,
		const FvNetUnpackedMessageHeader& header,
		FvBinaryIStream & data )
{
	FvInt8 broadcast;
	std::string script;
	data >> broadcast;
	data >> script;

	this->RunScript( script, broadcast );
}


void FvBaseAppMgr::RunScriptAll( std::string script )
{
	this->RunScript( script, 1 );
}


void FvBaseAppMgr::RunScriptSingle( std::string script )
{
	this->RunScript( script, 0 );
}


void FvBaseAppMgr::RunScript( const std::string & script, FvInt8 broadcast )
{
	if (broadcast != 0)
	{
		FvMemoryOStream payload;
		payload << script;

		if ((broadcast & 1) != 0)
		{
			this->SendToBaseApps( BaseAppIntInterface::RunScript, payload );
		}

		if ((broadcast & 2) != 0)
		{
			this->SendToBackupBaseApps( BaseAppIntInterface::RunScript,
					payload );
		}
	}
	else
	{
		FvBaseApp * pBest = this->FindBestBaseApp();

		if (pBest == NULL)
		{
			FV_ERROR_MSG( "No proxy could be found to run script\n" );
			return;
		}

		FvNetBundle & bundle = pBest->Bundle();
		bundle.StartMessage( BaseAppIntInterface::RunScript );
		bundle << script;
		pBest->Send();
	}
}


template <class ARGS_TYPE>
class BaseAppMgrMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvBaseAppMgr::*Handler)( const ARGS_TYPE & args );

	BaseAppMgrMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & /*srcAddr*/,
			FvNetUnpackedMessageHeader & /*header*/,
			FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;
		(FvBaseAppMgr::Instance().*handler_)( args );
	}

	Handler handler_;
};

template <class ARGS_TYPE>
class BaseAppMgrMessageHandlerWithAddr : public FvNetInputMessageHandler
{
public:
	typedef void (FvBaseAppMgr::*Handler)( const ARGS_TYPE & args,
		const FvNetAddress & addr );

	BaseAppMgrMessageHandlerWithAddr( Handler handler ) :
		handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & /*header*/,
			FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;
		(FvBaseAppMgr::Instance().*handler_)( args, srcAddr );
	}

	Handler handler_;
};


template <class ARGS_TYPE>
class BaseAppMgrReturnMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvBaseAppMgr::*Handler)(
			const FvNetAddress & srcAddr,
			FvNetReplyID replyID,
			const ARGS_TYPE & args );

	BaseAppMgrReturnMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;

		(FvBaseAppMgr::Instance().*handler_)( srcAddr, header.m_iReplyID, args );
	}

	Handler handler_;
};


class BaseAppMgrVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvBaseAppMgr::*Handler)( FvBinaryIStream & stream );

	BaseAppMgrVarLenMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
	{
		(FvBaseAppMgr::Instance().*handler_)( data );
	}

	Handler handler_;
};

class BaseAppMgrListenerMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvBaseAppMgr::*Handler)(const InterfaceListenerMsg& kMsg);

	BaseAppMgrListenerMessageHandler(Handler kHandler):m_kHandler(kHandler) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		InterfaceListenerMsg kMsg;
		data >> kMsg;
		FV_ASSERT(data.RemainingLength()==0 && !data.Error());
		(FvBaseAppMgr::Instance().*m_kHandler)(kMsg);
	}

	Handler m_kHandler;
};

class BaseAppMgrRawMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvBaseAppMgr::*Handler)(
			const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & stream );

	BaseAppMgrRawMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
	{
		(FvBaseAppMgr::Instance().*handler_)( srcAddr, header, data );
	}

	Handler handler_;
};


#define DEFINE_SERVER_HERE
#include "FvBaseAppManagerInterface.h"

#define DEFINE_INTERFACE_HERE
#include <../FvDBManager/FvDBInterface.h>
#define DEFINE_INTERFACE_HERE
#include <../FvLoginApp/FvLoginIntInterface.h>
#define DEFINE_INTERFACE_HERE
#include <../FvCell/FvCellAppInterface.h>
#define DEFINE_INTERFACE_HERE
#include <../FvCellAppManager/FvCellAppManagerInterface.h>
#define DEFINE_INTERFACE_HERE
#include <../FvBase/FvBaseAppIntInterface.h>

