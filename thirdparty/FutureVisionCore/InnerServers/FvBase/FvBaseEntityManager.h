//{future header message}
#ifndef __FvBaseEntityManager_H__
#define __FvBaseEntityManager_H__

#include "FvBaseDefines.h"

#include <FvMemoryStream.h>
#include <FvSingleton.h>
#include <FvIdClient.h>
#include <FvTimeQueue.h>

#include "FvBaseEntity.h"
#include "FvBaseAppIntInterface.h"
#include "FvBaseAppExtInterface.h"

typedef FvNetChannelOwner FvBaseAppMgr;
typedef FvNetChannelOwner FvDBMgr;

struct FvBaseAppInitData;
class FvProxy;
class FvSharedData;
class FvGlobalBases;
class FvTimeKeeper;
class CreateBaseCallBack;
class CreateBaseFromDBCallBack;
class LookUpBaseCallBack;
class LookUpDBIDCallBack;
class DeleteBaseCallBack;
class ExecuteRawDatabaseCommandCallBack;
struct FvAllBaseEntityExports;
struct FvAllData;


class EntityChannelFinder : public FvNetChannelFinder
{
public:
	EntityChannelFinder(FvNetNub& kNub);
	virtual ~EntityChannelFinder();

	virtual FvNetChannel* Find( FvNetChannelID id, const FvNetPacket * pPacket, bool & rHasBeenHandled );

	FvNetChannel*	MakeChannel(FvNetChannelID idx, const FvNetAddress& kCellAddr);
	FvNetChannel*	Find(FvNetChannelID id);
	void			DestroyChannel(FvNetChannelID idx);
	void			Dump();

protected:
	FvNetNub&		m_kNub;
	typedef std::map<FvNetChannelID, FvNetChannel*> IdxChannelMap;
	IdxChannelMap	m_kChannelMap;
};



class FV_BASE_API FvEntityManager : public FvNetTimerExpiryHandler, public FvSingleton< FvEntityManager >
{
public:
	FvEntityManager(FvNetNub& kIntNub, FvUInt16 uiExternalPort);
	~FvEntityManager();

	bool			Init(int iArgc, char* pcArgv[], FvUInt16 uiExternalPort);
	bool			FinishInit(const FvBaseAppInitData& kInitData);
	void			ShutDown();

	FvNetNub&		IntNub()	{ return m_kIntNub; }
	FvNetNub&		ExtNub()	{ return m_kExtNub; }
	FvNetChannel*	FindChannel(const FvNetAddress& kAddr) { return m_kIntNub.FindChannel(kAddr); }
	FvNetChannel*	FindOrCreateChannel(const FvNetAddress& kAddr) { return &m_kIntNub.FindOrCreateChannel(kAddr); }
	double			GameTimeInSeconds() const { return double(m_uiTime)/m_iUpdateHertz; }
	double			GameTimeInSeconds(FvTimeStamp uiTime) const { return double(uiTime)/m_iUpdateHertz; }
	FvTimeStamp		GameTime() const { return m_uiTime; }

	FvEntity*		FindEntity(FvEntityID iID) const;
	FvProxy*		FindProxy(const FvNetAddress& addr) const;
	FvDBMgr&		DBMgr() { return *m_kDBMgr.GetChannelOwner(); }
	FvNetChannel*	CellAppMgr();
	FvNetChannel*	GlobalCellAppMgr();
	typedef std::map<FvEntityID, FvEntityPtr> Entities;
	const Entities&	GetEntities() const { return m_kEntities; }
	FvSharedData*	GetBaseAppData() const { return m_pkBaseAppData; }
	FvSharedData*	GetGlobalData() const { return m_pkGlobalData; }
	FvGlobalBases*	GetGlobalBases() const { return m_pkGlobalBases; }
	FvMailBox*		FindGlobalBase(const char* pcLabel) const;
	FvMailBox*		FindGlobalBase(const FvString& kLabel) const;
	FvMailBox*		FindGlobalMailBox(const char* pcLabel) const;
	FvMailBox*		FindGlobalMailBox(const FvString& kLabel) const;
	FvEntityTypeID	EntityNameToIndex(const char* pcEntityTypeName) const;

	//! 脚本调用接口
					//! TODO: 创建Entity接口去除FvEntityTypeID, FvAllData里已经有类型信息了
	void			CreateBaseAnywhere(FvEntityTypeID uiEntityType, const FvAllData& kAllData, bool bGlobal=false, CreateBaseCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);//! TODO: 回调要做成退栈后回调
	void			CreateBaseAnywhere(const char* pcEntityTypeName, const FvAllData& kAllData, bool bGlobal=false, CreateBaseCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);//! TODO: 回调要做成退栈后回调
	void			CreateBaseAnywhereFromDB(FvEntityTypeID uiEntityType, const char* pcName, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseAnywhereFromDB(const char* pcEntityTypeName, const char* pcName, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseAnywhereFromDBID(FvEntityTypeID uiEntityType, FvDatabaseID iDBID, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseAnywhereFromDBID(const char* pcEntityTypeName, FvDatabaseID iDBID, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	FvEntity*		CreateBaseLocally(FvEntityTypeID uiEntityType, const FvAllData& kAllData, bool bGlobal=false, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	FvEntity*		CreateBaseLocally(const char* pcEntityTypeName, const FvAllData& kAllData, bool bGlobal=false, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseLocallyFromDB(FvEntityTypeID uiEntityType, const char* pcName, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseLocallyFromDB(const char* pcEntityTypeName, const char* pcName, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseLocallyFromDBID(FvEntityTypeID uiEntityType, FvDatabaseID iDBID, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseLocallyFromDBID(const char* pcEntityTypeName, FvDatabaseID iDBID, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseRemotely(FvEntityTypeID uiEntityType, const FvAllData& kAllData, FvMailBox& kBaseMB, bool bGlobal=false, CreateBaseCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseRemotely(const char* pcEntityTypeName, const FvAllData& kAllData, FvMailBox& kBaseMB, bool bGlobal=false, CreateBaseCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseRemotelyFromDB(FvEntityTypeID uiEntityType, const char* pcName, FvMailBox& kBaseMB, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseRemotelyFromDB(const char* pcEntityTypeName, const char* pcName, FvMailBox& kBaseMB, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseRemotelyFromDBID(FvEntityTypeID uiEntityType, FvDatabaseID iDBID, FvMailBox& kBaseMB, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateBaseRemotelyFromDBID(const char* pcEntityTypeName, FvDatabaseID iDBID, FvMailBox& kBaseMB, CreateBaseFromDBCallBack* pkCallBack=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			LookUpBaseByDBID(FvEntityTypeID uiEntityType, FvDatabaseID iDBID, LookUpBaseCallBack* pkCallBack);
	void			LookUpBaseByDBID(const char* pcEntityTypeName, FvDatabaseID iDBID, LookUpBaseCallBack* pkCallBack);
	void			LookUpBaseByName(FvEntityTypeID uiEntityType, const char* pcName, LookUpBaseCallBack* pkCallBack);
	void			LookUpBaseByName(const char* pcEntityTypeName, const char* pcName, LookUpBaseCallBack* pkCallBack);
	void			LookUpDBIDByName(FvEntityTypeID uiEntityType, const char* pcName, LookUpDBIDCallBack* pkCallBack);
	void			LookUpDBIDByName(const char* pcEntityTypeName, const char* pcName, LookUpDBIDCallBack* pkCallBack);
	void			DeleteBaseByDBID(FvEntityTypeID uiEntityType, FvDatabaseID iDBID, DeleteBaseCallBack* pkCallBack);
	void			DeleteBaseByDBID(const char* pcEntityTypeName, FvDatabaseID iDBID, DeleteBaseCallBack* pkCallBack);
	void			ExecuteRawDatabaseCommand(const FvString kCommand, ExecuteRawDatabaseCommandCallBack* pkCallBack);
	//void			SetCellAppData(const char* pcKey, value);
	//void			DelCellAppData(const char* pcKey);
	const FvNetAddress& Address() const;
	float			GetLoad() const { return m_fLoad; }
	bool			HasStarted() const { return false; }
	bool			IsShuttingDown() const { return false; }
	//! 

	//! external interface
	void			BaseAppLogin( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data );
	void			Authenticate( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, const BaseAppExtInterface::AuthenticateArgs & args );
	//! internal interface
	void			CreateBaseWithCellData( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data );
	void			CreateBaseFromDB( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data );
	void			LogOnAttempt( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data );
	void			AddGlobalBase( FvBinaryIStream & data );
	void			DelGlobalBase( FvBinaryIStream & data );
	void			AddGlobalMailBox(FvBinaryIStream& kData);
	void			DelGlobalMailBox(FvBinaryIStream& kData);
	void			HandleCellAppMgrBirth(const InterfaceListenerMsg& kMsg);
	void			HandleBaseAppMgrBirth(const InterfaceListenerMsg& kMsg);
	void			HandleGlobalAppBirth(const InterfaceListenerMsg& kMsg);
	void			HandleCellAppDeath( FvBinaryIStream & data );
	void			Startup( const BaseAppIntInterface::StartupArgs & args );
	void			ShutDown( const BaseAppIntInterface::ShutDownArgs & args );
	void			ControlledShutDown( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header,	FvBinaryIStream & data );
	void			SetCreateBaseInfo( FvBinaryIStream & data );
	void			SetSharedData( FvBinaryIStream & data );
	void			DelSharedData( FvBinaryIStream & data );
	void			SetClient( const BaseAppIntInterface::SetClientArgs & args );
	//! 暂时不用的internal interface
	void			RunScript( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data );
	void			OldSetBackupBaseApp( const BaseAppIntInterface::OldSetBackupBaseAppArgs & args );
	void			OldStartBaseAppBackup( const BaseAppIntInterface::OldStartBaseAppBackupArgs & args );
	void			OldStopBaseAppBackup( const BaseAppIntInterface::OldStopBaseAppBackupArgs & args );
	void			OldBackupBaseEntities( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data );
	void			OldBackupHeartbeat( const BaseAppIntInterface::OldBackupHeartbeatArgs & args );
	void			OldRestoreBaseApp( const BaseAppIntInterface::OldRestoreBaseAppArgs & args );
	void			StartBaseEntitiesBackup( const BaseAppIntInterface::StartBaseEntitiesBackupArgs & args );
	void			StopBaseEntitiesBackup( const BaseAppIntInterface::StopBaseEntitiesBackupArgs & args );
	void			BackupBaseEntity( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void			StopBaseEntityBackup( const FvNetAddress & srcAddr, const BaseAppIntInterface::StopBaseEntityBackupArgs & args );
	void			HandleBaseAppDeath( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void			SetBackupBaseApps( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void			EmergencySetCurrentCell( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void			CallWatcher( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data );
	//! 

	void			AddToBaseAppMgr();
	FvProxy*		GetAndCheckProxyForCall(FvNetUnpackedMessageHeader& kHeader, const FvNetAddress& kSrcAddr);
	FvEntity*		GetBaseForCall(FvNetUnpackedMessageHeader& kHeader);
	void			AddressDead(const FvNetAddress& kAddress, FvNetReason eReason);
	FvEntityID		GetNextEntityID();
	FvEntityID		GetNextGlobalEntityID();
	FvEntity*		CreateEntityCommon(FvEntityTypeID uiTypeID, FvEntityID iEntityID, FvDatabaseID iDBID, FvBinaryIStream& stream, bool bFromDB, FvUInt8 uiInitFlg);
	FvEntity*		CreateEntityCommon(FvEntityTypeID uiTypeID, FvEntityID iEntityID, FvDatabaseID iDBID, const FvAllData* pkAllData, FvUInt8 uiInitFlg);
	EntityChannelFinder& GetIdxChannelFinder() { return m_kIdxChannelFinder; }
	void			ChangeProxy(const FvNetAddress& kAddr, FvProxy* pkProxy);
	bool			IsCreateBaseAnywhereRemote() const { return m_kIntNub.Address()!=m_kCreateBaseAnywhereAddr; }
	const FvNetAddress&	CreateBaseAnywhereAddr() { return m_kCreateBaseAnywhereAddr; }
	void			AddEntity(FvEntity* pkEntity);
	void			DelEntity(FvEntity* pkEntity);
	void			EraseProxy(const FvNetAddress& kAddr);
	FvBaseAppMgr&	BaseAppMgr() { return m_kBaseAppMgr; }
	bool			LocalMailBoxAsRemote() { return m_bLocalMailBoxAsRemote; }
	void			AddDestroyEntity(FvEntity* pkEntity) { m_kDestroyEntities.push_back(pkEntity); }

protected:
	virtual int		HandleTimeout( FvNetTimerID id, void * arg );
	void			InitEntityIDClient();
	void			InitGlobalEntityIDClientAndRegister(FvNetAddress& kAddr, bool bBlock);
	void			StartUpdate() { ++m_iUpdateFlag; }
	void			StopUpdate() { --m_iUpdateFlag; }
	void			ClearAddDelEntityList();
	void			CheckDelEntities();
	void			CheckProxyTimeOut();
	void			OnStartup();

private:
	FvBaseAppID		m_iBaseAppID;
	FvNetNub&		m_kIntNub;
	FvNetNub		m_kExtNub;
	FvNetAddress	m_kExtMappingAddr;

	FvBaseAppMgr	m_kBaseAppMgr;
	FvNetAddress	m_kCellAppMgr;
	FvNetAddress	m_kGlobalCellAppMgr;
	FvAnonymousChannelClient m_kDBMgr;

	Entities		m_kEntities;
	typedef std::map<FvNetAddress, FvProxy *> Proxies;
	Proxies			m_kProxies;
	FvAllBaseEntityExports* m_pkEntityExports;
	FvIDClient		m_kEntityIDClient;
	FvIDClient		m_kGlobalEntityIDClient;

	FvEntity*		m_pkEntityForCall;

	FvSharedData*	m_pkBaseAppData;
	FvSharedData*	m_pkGlobalData;
	FvGlobalBases*	m_pkGlobalBases;
	typedef std::map<FvString, FvMailBox> GlobalMailBoxMap;
	GlobalMailBoxMap	m_kGlobalMailBoxMap;

	enum TimeOutType
	{
		TIMEOUT_GAME_TICK
	};
	FvTimeStamp		m_uiTime;
	FvTimeKeeper*	m_pkTimeKeeper;
	FvInt32			m_iUpdateHertz;
	FvInt32			m_iSystemManagePeriod;
	FvInt32			m_iProxyTimeoutPeriod;	//! TODO: 改为FvNetChannel::StartInactivityDetection方式
	FvInt32			m_iSyncTimePeriod;

	bool			m_bIsBootstrap;
	float			m_fLoad;
	FvNetAddress	m_kCreateBaseAnywhereAddr;

	typedef std::map<FvUInt32, std::pair<FvProxy*,FvUInt64>> PendingLogins;
	PendingLogins	m_kPendingLogins;

	EntityChannelFinder	m_kIdxChannelFinder;

	int				m_iUpdateFlag;//! 防止在容器迭代过程中对容器进行操作
	struct AddDelEntity
	{
		AddDelEntity(FvEntity* pkEntity, int iflg)
		:m_pkEntity(pkEntity),m_iFlag(iflg)
		{}

		FvEntity*		m_pkEntity;
		int				m_iFlag;	//! 0:del, 1:add
	};
	typedef std::vector<AddDelEntity> AddDelEntityList;
	AddDelEntityList m_kAddDelEntityList;

	typedef std::vector<FvEntity*> DelEntityList;
	DelEntityList	m_kDelEntityList;

	bool			m_bLocalMailBoxAsRemote;	//! 调用本地MB是否当作异地MB处理

	FvUInt64		m_uiDllTickLastTime;

	typedef std::vector<FvEntityPtr> DestroyEntities;
	DestroyEntities	m_kDestroyEntities;
};

class FV_BASE_API CreateBaseCallBack
{
public:
	virtual void HandleCallBack(FvMailBox* pkBaseMB) = 0;
};

class FV_BASE_API CreateBaseFromDBCallBack
{
public:
	virtual void HandleCallBack(FvMailBox* pkBaseMB, FvDatabaseID iDBID, bool bWasActive) = 0;
};

class FV_BASE_API LookUpBaseCallBack
{
public:
	virtual void HandleCallBack(FvMailBox* pkBaseMB, bool bExist) = 0;
};

class FV_BASE_API LookUpDBIDCallBack
{
public:
	virtual void HandleCallBack(FvDatabaseID iDBID) = 0;
};

class FV_BASE_API DeleteBaseCallBack
{
public:
	virtual void HandleCallBack(bool bSuccess, FvMailBox* pkBaseMB) = 0;
};

//! 数据库查询结果中一行一列的值
struct DBResultCell
{
	const char*	m_pcData;
	FvUInt32	m_uiLength;

	DBResultCell():m_pcData(NULL),m_uiLength(0) {}
	DBResultCell(const char* pcData, FvUInt32 uiLen):m_pcData(pcData),m_uiLength(uiLen) {}

	bool IsNull() const	{ return (m_pcData == NULL); }
};

typedef std::vector<DBResultCell> DBResultLine;
typedef std::vector<DBResultLine> DBResult;

class FV_BASE_API ExecuteRawDatabaseCommandCallBack
{
public:
	virtual void HandleCallBack(DBResult& kResult, FvUInt64 uiAffectedRows, FvString& kErr) = 0;
};


#endif // __FvBaseEntityManager_H__
