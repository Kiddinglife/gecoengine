//{future header message}
#ifndef __FvDatabase_H__
#define __FvDatabase_H__

#include <FvMD5.h>
#include <FvSingleton.h>
#include <FvNetChannel.h>
#include <FvXMLSection.h>
#include <FvDocWatcher.h>
#include <FvBackupHash.h>

#include "FvDBInterface.h"
#include "FvDBStatus.h"
#include "FvIDatabase.h"
#include "FvWorkerThread.h"
//#include "FvSignalSet.h"

#ifdef FV_USE_ORACLE
#include "FvOracleDatabase.h"
#endif // FV_USE_ORACLE

#include <map>
#include <set>

class RelogonAttemptHandler;
class FvEntityDefs;

class FvDBConfigServer;

typedef FvNetChannelOwner FvBaseAppManager;

class FvDatabase : public FvNetTimerExpiryHandler,
	public FvIDatabase::IGetBaseAppMgrInitDataHandler,
	public FvIDatabase::IUpdateSecondaryDBshandler,
	public FvSingleton< FvDatabase >
{
public:
	enum InitResult
	{
		InitResultSuccess,
		InitResultFailure,
		InitResultAutoShutdown
	};

public:

	FvDatabase( FvNetNub & nub );
	virtual ~FvDatabase();
	InitResult Init( bool isUpgrade, bool isSyncTablesToDefs );
	void Run();
	void Finalise();

	void OnSignalled( int sigNum );
	//void onChildProcessExit( pid_t pid, int status );

	FvBaseAppManager &GetBaseAppManager() { return m_kBaseAppManager; }

	int HandleTimeout( FvNetTimerID id, void * arg );

	void HandleStatusCheck( FvBinaryIStream & data );

	void HandleBaseAppMgrBirth(const InterfaceListenerMsg& kMsg);
	void HandleDatabaseBirth(const InterfaceListenerMsg& kMsg);
	void ShutDown( DBInterface::ShutDownArgs & /*args*/ );
	void StartSystemControlledShutdown();
	void ShutDownNicely();
	void ShutDown();

	void ControlledShutDown( DBInterface::ControlledShutDownArgs & args );
	void CellAppOverloadStatus( DBInterface::CellAppOverloadStatusArgs & args );

	void LogOn( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );

	void LogOn( const FvNetAddress & srcAddr,
		FvNetReplyID replyID,
		LogOnParamsPtr pParams,
		const FvNetAddress & addrForProxy,
		bool offChannel );

	void OnLogOnLoggedOnUser( FvEntityTypeID typeID, FvDatabaseID dbID,
		LogOnParamsPtr pParams,
		const FvNetAddress & proxyAddr, const FvNetAddress& replyAddr,
		bool offChannel, FvNetReplyID replyID,
		const FvEntityMailBoxRef* pExistingBase );

	void OnEntityLogOff( FvEntityTypeID typeID, FvDatabaseID dbID );

	bool CalculateOverloaded( bool isOverloaded );

	void SendFailure( FvNetReplyID replyID,
		const FvNetAddress & dstAddr,
		bool offChannel,
		FvDatabaseLoginStatus reason, const char * pDescription = NULL );

	void LoadEntity( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );

	void WriteEntity( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );

	void DeleteEntity( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		DBInterface::DeleteEntityArgs & args );
	void DeleteEntityByName( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );

	void LookupEntity( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		DBInterface::LookupEntityArgs & args );
	void LookupEntityByName( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );
	void LookupDBIDByName( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );
	void LookupEntity( FvEntityDBKey & ekey, FvBinaryOStream & bos );

	void ExecuteRawCommand( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );

	void PutIDs( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );

	void GetIDs( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );

	void WriteSpaces( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );

	void WriteGameTime( DBInterface::WriteGameTimeArgs & args );

	void HandleBaseAppDeath( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void CheckStatus( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void SecondaryDBRegistration( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void UpdateSecondaryDBs( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );
	virtual void OnUpdateSecondaryDBsComplete(
			const FvIDatabase::SecondaryDBEntries& removedEntries );

	void GetSecondaryDBDetails( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );

	const FvEntityDefs &GetEntityDefs() const
		{ return *m_pkEntityDefs; }
	FvEntityDefs& SwapEntityDefs( FvEntityDefs& entityDefs )
	{
		FvEntityDefs& curDefs = *m_pkEntityDefs;
		m_pkEntityDefs = &entityDefs;
		return curDefs;
	}
	FvDBConfigServer &GetServerConfig()	{ return *m_spServerConfig; }

	FvNetNub & nub()			{ return m_kNub; }
	static FvNetNub &GetNub()	{ return FvDatabase::Instance().nub(); }

	static FvNetChannel &GetChannel( const FvNetAddress & addr )
	{
		return FvDatabase::Instance().nub().FindOrCreateChannel( addr );
	}

	FvWorkerThreadManager& GetWorkerThreadMgr()	{	return m_kWorkerThreadManager;	}
	FvIDatabase& GetIDatabase()	{ FV_ASSERT(m_pkDatabase); return *m_pkDatabase; }

	class GetEntityHandler : public FvIDatabase::IGetEntityHandler
	{
	public:
		virtual void OnGetEntityComplete( bool isOK );
		virtual void OnGetEntityCompleted( bool isOK ) = 0;
	};
	void GetEntity( GetEntityHandler& handler );
	void PutEntity( const FvEntityDBKey& ekey, FvEntityDBRecordIn& erec,
			FvIDatabase::IPutEntityHandler& handler );
	void DelEntity( const FvEntityDBKey & ekey,
			FvIDatabase::IDelEntityHandler& handler );
	void SetLoginMapping( const FvString & username,
			const FvString & password, const FvEntityDBKey& ekey,
			FvIDatabase::ISetLoginMappingHandler& handler );

	bool ShouldLoadUnknown() const		{ return m_bShouldLoadUnknown; }
	bool ShouldCreateUnknown() const	{ return m_bShouldCreateUnknown; }
	bool ShouldRememberUnknown() const	{ return m_bShouldRememberUnknown; }

	bool ClearRecoveryDataOnStartUp() const
										{ return m_bClearRecoveryDataOnStartUp; }

	void HasStartBegun( bool hasStartBegun );
	bool HasStartBegun() const
	{
		return m_kStatus.GetStatus() > FvDBStatus::WAITING_FOR_APPS;
	}
	//bool isConsolidating() const		{ return consolidatePid_ != 0; }

	bool IsReady() const
	{
		return m_kStatus.GetStatus() >= FvDBStatus::WAITING_FOR_APPS;
	}

	void StartServerBegin( bool isRecover = false );
	void StartServerEnd( bool isRecover = false );
	void StartServerError();

	static void SetBaseRefToLoggingOn( FvEntityMailBoxRef& baseRef,
		FvEntityTypeID entityTypeID )
	{
		baseRef.Init( 0, FvNetAddress( 0, 0 ),
			FvEntityMailBoxRef::BASE, entityTypeID );
	}
	static bool IsValidMailBox( const FvEntityMailBoxRef* pBaseRef )
	{	return (pBaseRef && pBaseRef->m_iID != 0);	}

	bool DefaultEntityToStrm( FvEntityTypeID typeID, const FvString& name,
		FvBinaryOStream& strm, const FvString* pPassword = 0 ) const;

	static FvDatabaseID* PrepareCreateEntityBundle( FvEntityTypeID typeID,
		FvDatabaseID dbID, const FvNetAddress& addrForProxy,
		FvNetReplyMessageHandler* pHandler, FvNetBundle& bundle,
		LogOnParamsPtr pParams = NULL );

	RelogonAttemptHandler* GetInProgRelogonAttempt( FvEntityTypeID typeID,
			FvDatabaseID dbID )
	{
		PendingAttempts::iterator iter =
				m_kPendingAttempts.find( FvEntityKey( typeID, dbID ) );
		return (iter != m_kPendingAttempts.end()) ? iter->second : NULL;
	}
	void OnStartRelogonAttempt( FvEntityTypeID typeID, FvDatabaseID dbID,
		 	RelogonAttemptHandler* pHandler )
	{
		FV_VERIFY( m_kPendingAttempts.insert(
				PendingAttempts::value_type( FvEntityKey( typeID, dbID ),
						pHandler ) ).second );
	}
	void OnCompleteRelogonAttempt( FvEntityTypeID typeID, FvDatabaseID dbID )
	{
		FV_VERIFY( m_kPendingAttempts.erase( FvEntityKey( typeID, dbID ) ) > 0 );
	}

	bool OnStartEntityCheckout( const FvEntityKey& entityID )
	{
		return m_kInProgCheckouts.insert( entityID ).second;
	}
	bool OnCompleteEntityCheckout( const FvEntityKey& entityID,
			const FvEntityMailBoxRef* pBaseRef );

	struct ICheckoutCompletionListener
	{
		virtual void OnCheckoutCompleted( const FvEntityMailBoxRef* pBaseRef ) = 0;
	};
	bool RegisterCheckoutCompletionListener( FvEntityTypeID typeID,
			FvDatabaseID dbID, ICheckoutCompletionListener& listener );

	bool HasMailboxRemapping() const	{ return !m_kRemappingDestAddrs.empty(); }
	void RemapMailbox( FvEntityMailBoxRef& mailbox ) const;

private:
	void EndMailboxRemapping();

	void SendInitData();
	virtual void OnGetBaseAppMgrInitDataComplete( FvTimeStamp gameTime,
			FvInt32 appID );

	bool ProcessSignals();

	void ConsolidateData();
	bool StartConsolidationProcess();
	void OnConsolidateProcessEnd( bool isOK );
	bool SendRemoveDBCmd( FvUInt32 destIP, const FvString& dbLocation );

#ifdef FV_DBMGR_SELFTEST
	void RunSelfTest();
#endif

	FvNetNub &m_kNub;
	FvWorkerThreadManager m_kWorkerThreadManager;
	FvEntityDefs *m_pkEntityDefs;
	FvIDatabase *m_pkDatabase;

	//Signal::Set signals_;

	FvDBStatus m_kStatus;

	FvBaseAppManager m_kBaseAppManager;

	bool m_bShouldLoadUnknown;
	bool m_bShouldCreateUnknown;
	bool m_bShouldRememberUnknown;
	std::auto_ptr<FvDBConfigServer> m_spServerConfig;

	bool m_bAllowEmptyDigest;

	bool m_bShouldSendInitData;

	bool m_bShouldConsolidate;

	FvUInt32 m_uiDesiredBaseApps;
	FvUInt32 m_uiDesiredCellApps;

	FvNetTimerID m_kStatusCheckTimerID;

	bool m_bClearRecoveryDataOnStartUp;

	FvNetNub::TransientMiniTimer m_kWriteEntityTimer;

	friend class RelogonAttemptHandler;
	typedef std::map< FvEntityKey, RelogonAttemptHandler * > PendingAttempts;
	PendingAttempts m_kPendingAttempts;
	typedef std::set< FvEntityKey > EntityKeySet;
	EntityKeySet m_kInProgCheckouts;

	typedef std::multimap< FvEntityKey, ICheckoutCompletionListener* >
			CheckoutCompletionListeners;
	CheckoutCompletionListeners m_kCheckoutCompletionListeners;

	float m_fCurLoad;
	float m_fMaxLoad;
	bool m_bAnyCellAppOverloaded;
	FvUInt64 m_uiAllowOverloadPeriod;
	FvUInt64 m_uiOverloadStartTime;

	FvNetAddress m_kRemappingSrcAddr;
	FvBackupHash m_kRemappingDestAddrs;
	int m_iMailboxRemapCheckCount;

	FvString m_kSecondaryDBPrefix;
	unsigned int m_uiSecondaryDBIndex;

	//pid_t				consolidatePid_;

	bool m_bIsProduction;
};

#endif // __FvDatabase_H__

