//{future header message}
#ifndef __FvBaseAppManager_H__
#define __FvBaseAppManager_H__

#include "FvBaseAppManagerInterface.h"

#include <FvSingleton.h>
#include <FvNetChannel.h>
#include <FvServerCommon.h>

#include <map>
#include <set>
#include <string>

class FvBaseApp;
class FvBackupBaseApp;
class FvTimeKeeper;

typedef FvNetChannelOwner FvCellAppMgr;
typedef FvNetChannelOwner FvDBMgr;


class FvBaseAppMgr : public FvNetTimerExpiryHandler,
	public FvSingleton< FvBaseAppMgr >
{
public:
	FvBaseAppMgr( FvNetNub & nub );
	virtual ~FvBaseAppMgr();
	bool Init(int argc, char* argv[]);

	void Add( const FvNetAddress & srcAddr, FvNetReplyID replyID,
		const BaseAppMgrInterface::AddArgs & args );
	void AddBackup( const FvNetAddress & srcAddr, FvNetReplyID replyID,
		const BaseAppMgrInterface::AddBackupArgs & args );

	void RecoverBaseApp( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );
	void OldRecoverBackupBaseApp( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void Del( const BaseAppMgrInterface::DelArgs & args,
		const FvNetAddress & addr );
	void InformOfLoad( const BaseAppMgrInterface::InformOfLoadArgs  & args,
		const FvNetAddress & addr );

	void ShutDown( bool shutDownOthers );
	void ShutDown( const BaseAppMgrInterface::ShutDownArgs & args );
	void Startup( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void CheckStatus( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void ControlledShutDown(
			const BaseAppMgrInterface::ControlledShutDownArgs & args );

	void RequestHasStarted( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void InitData( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void SpaceDataRestore( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void SetSharedData( FvBinaryIStream & data );
	void DelSharedData( FvBinaryIStream & data );

	void UseNewBackupHash( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void InformOfArchiveComplete( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	FvBaseApp * FindBaseApp( const FvNetAddress & addr );
	FvNetChannelOwner * FindChannelOwner( const FvNetAddress & addr );

	FvNetNub & Nub()						{ return m_kNub; }

	static FvNetChannel & GetChannel( const FvNetAddress & addr )
	{
		return FvBaseAppMgr::Instance().Nub().FindOrCreateChannel( addr );
	}

	int NumBaseApps() const					{ return m_kBaseApps.size(); }
	int NumBackupBaseApps() const			{ return m_kBackupBaseApps.size(); }

	int NumBases() const;
	int NumProxies() const;

	float MinBaseAppLoad() const;
	float AvgBaseAppLoad() const;
	float MaxBaseAppLoad() const;

	double GameTimeInSeconds() const { return double( m_uiTime )/m_iUpdateHertz; }

	FvCellAppMgr & CellAppMgr()	{ return m_kCellAppMgr; }
	FvDBMgr & DBMgr()				{ return *m_kDBMgr.GetChannelOwner(); }

	void HandleCellAppMgrBirth(const InterfaceListenerMsg& kMsg);
	void HandleBaseAppMgrBirth(const InterfaceListenerMsg& kMsg);
	void HandleCellAppDeath( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );
	void HandleBaseAppDeath(const InterfaceListenerMsg& kMsg);
	void HandleBaseAppDeath( const FvNetAddress & addr );

	void CreateBaseEntity( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void RegisterBaseGlobally( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void DeregisterBaseGlobally( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void RunScript( const FvNetAddress & addr,
			const FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data );

	void StartAsyncShutDownStage( FvShutDownStage stage );

	void ControlledShutDownServer();

	bool ShutDownServerOnBadState() const	{ return m_bShutDownServerOnBadState; }

	bool OnBaseAppDeath( const FvNetAddress & addr, bool shouldRestore );
	bool OnBackupBaseAppDeath( const FvNetAddress & addr );

	void RemoveControlledShutdownBaseApp( const FvNetAddress & addr );

	FvBaseApp * FindBestBaseApp() const;

private:
	enum TimeOutType
	{
		TIMEOUT_GAME_TICK
	};

	virtual int HandleTimeout( FvNetTimerID id, void * arg );

	void StartTimer();
	void CheckBackups();
	void AdjustBackupLocations( const FvNetAddress & addr, bool isAdd );

	void CheckForDeadBaseApps();

	void CheckGlobalBases( const FvNetAddress & addr );

	void UpdateCreateBaseInfo();

	void RunScriptAll( std::string script );
	void RunScriptSingle( std::string script );

	void RunScript( const std::string & script, FvInt8 broadcast );

	FvNetNub & m_kNub;

	template <class TYPE>
	class FvAutoPointer
	{
	public:
		FvAutoPointer() : m_pValue( NULL ) {}
		~FvAutoPointer()	{ delete m_pValue; }

		void Set( TYPE * pValue )	{ m_pValue = pValue; }

		TYPE & operator *()						{ return *m_pValue; }
		const TYPE & operator *() const			{ return *m_pValue; }

		TYPE * operator->()						{ return m_pValue; }
		const TYPE * operator->() const			{ return m_pValue; }

		TYPE * Get()				{ return m_pValue; }
		const TYPE * Get() const	{ return m_pValue; }

	private:
		TYPE * m_pValue;
	};

	typedef FvAutoPointer< FvBaseApp > FvBaseAppPtr;
	typedef FvAutoPointer< FvBackupBaseApp > FvBackupBaseAppPtr;

public:
	typedef std::map< FvNetAddress, FvBaseAppPtr > FvBaseApps;
	FvBaseApps & BaseApps()	{ return m_kBaseApps; }

private:
	FvCellAppMgr				m_kCellAppMgr;
	FvAnonymousChannelClient	m_kDBMgr;

	FvBaseApps					m_kBaseApps;

	typedef std::map< FvNetAddress, FvBackupBaseAppPtr > FvBackupBaseApps;
	FvBackupBaseApps			m_kBackupBaseApps;

	typedef std::map< std::string, std::string > FvSharedData;
	FvSharedData				m_kSharedBaseAppData;
	FvSharedData				m_kSharedGlobalData;

	FvBaseAppID 				m_iLastBaseAppID;

	FvBackupBaseApp * FindBestBackup( const FvBaseApp & baseApp ) const;
	FvBaseAppID GetNextID();

	void SendToBaseApps( const FvNetInterfaceElement & ifElt,
		FvMemoryOStream & args, const FvBaseApp * pExclude = NULL,
		FvNetReplyMessageHandler * pHandler = NULL );

	void SendToBackupBaseApps( const FvNetInterfaceElement & ifElt,
		FvMemoryOStream & args, const FvBackupBaseApp * pExclude = NULL,
		FvNetReplyMessageHandler * pHandler = NULL );

	void SendToCellAppMgr(const FvNetInterfaceElement& ifElt, FvMemoryOStream& args);

	void AddWatchers();

	bool			m_bAllowNewBaseApps;

	typedef std::map< std::string, FvEntityMailBoxRef > FvGlobalBases;
	FvGlobalBases	m_kGlobalBases;

	FvTimeStamp		m_uiTime;
	FvTimeKeeper *	m_pkTimeKeeper;
	FvInt32			m_iSyncTimePeriod;
	FvInt32			m_iUpdateHertz;

	float			m_fBaseAppOverloadLevel;
	float			m_fCreateBaseRatio;
	FvInt32			m_iUpdateCreateBaseInfoPeriod;

	FvNetAddress	m_kBestBaseAppAddr;

	bool			m_bIsRecovery;
	bool			m_bHasInitData;
	bool			m_bHasStarted;
	bool			m_bShouldShutDownOthers;
	bool			m_bShouldHardKillDeadBaseApps;
	bool			m_bOnlyUseBackupOnSameMachine;
	bool			m_bUseNewStyleBackup;
	bool			m_bShutDownServerOnBadState;
	bool			m_bShutDownServerOnBaseAppDeath;
	bool			m_bIsProduction;

	FvNetAddress	m_kDeadBaseAppAddr;
	FvUInt32		m_uiArchiveCompleteMsgCounter;

	FvTimeStamp		m_uiShutDownTime;
	FvShutDownStage	m_eShutDownStage;

	FvUInt64		m_uiBaseAppTimeoutPeriod;

	FvUInt64		m_uiBaseAppOverloadStartTime;
	FvInt32			m_iLoginsSinceOverload;
	FvUInt64		m_uiAllowOverloadPeriod;
	FvInt32			m_iAllowOverloadLogins;

	bool			m_bHasMultipleBaseAppMachines;

	friend class CreateEntityIncomingHandler;
	friend class CreateBaseReplyHandler;
};


#endif // __FvBaseAppManager_H__
