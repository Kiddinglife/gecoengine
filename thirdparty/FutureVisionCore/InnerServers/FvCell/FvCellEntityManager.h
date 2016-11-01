#ifndef __FvCellEntityManager_H__
#define __FvCellEntityManager_H__

#include "FvCellDefines.h"
#include <FvMemoryStream.h>
#include <FvSingleton.h>
#include <FvTimeQueue.h>

#include <FvNetNub.h>
#include <FvNetChannel.h>
#include <FvNetInterfaces.h>
#include <FvIdClient.h>
#include <FvCellRect.h>

#include "FvCellAppInterface.h"
#include "FvCellEntity.h"


class FvCell;
class FvSpace;
struct FvCellAppInitData;
class FvTimeKeeper;
class SharedData;
typedef FvNetChannelOwner DBMgr;
class FvEntity;
typedef FvSmartPointer<FvEntity> FvEntityPtr;
struct FvAllCellEntityExports;
struct FvAllData;

class AoICfg
{
public:
	enum
	{
		GRIDSIZE = (0x01<<0),
		REFPTLVL = (0x01<<1),
		MINMOVESTEP = (0x01<<2),
		OBJINITSIZE = (0x01<<3),
		OBJINCRSIZE = (0x01<<4),
		POSINITSIZE = (0x01<<5),
		POSINCRSIZE = (0x01<<6),
		EVTINITSIZE = (0x01<<7),
		EVTINCRSIZE = (0x01<<8),
		AOIEXTINITSIZE = (0x01<<9),
		AOIEXTINCRSIZE = (0x01<<10),
		TRAPEXTINITSIZE = (0x01<<11),
		TRAPEXTINCRSIZE = (0x01<<12),
	};
	float		m_fGridSize;
	FvUInt32	m_uiRefPtLvl;
	float		m_fMinMoveStep;
	FvUInt32	m_uiObjInitSize;
	FvUInt32	m_uiObjIncrSize;
	FvUInt32	m_uiPosInitSize;
	FvUInt32	m_uiPosIncrSize;
	FvUInt32	m_uiEvtInitSize;
	FvUInt32	m_uiEvtIncrSize;
	FvUInt32	m_uiAoIExtInitSize;
	FvUInt32	m_uiAoIExtIncrSize;
	FvUInt32	m_uiTrapExtInitSize;
	FvUInt32	m_uiTrapExtIncrSize;

	AoICfg();
};

struct CellData
{
	bool FindGhostGroup(float fX, float fY, FvUInt32& uiSize, FvUInt16*& pkIDs) const;

	FvCellRect	m_kRect;
	AoICfg		m_kAoICfg;
	bool		m_bLoadEntity;
	bool		m_bLoadZone;

	struct GhostKDTree
	{
		struct Node
		{
			float	m_fKey;
			struct
			{
				FvUInt8	m_bLeftLeaf : 1;
				FvUInt8 m_bLeftValueHigh : 7;
				FvUInt8 m_bLeftValueLow;
				FvUInt8	m_bRightLeaf : 1;
				FvUInt8 m_bRightValueHigh : 7;
				FvUInt8 m_bRightValueLow;
			};

			FvUInt16 GetLeftValue() const { return (m_bLeftValueHigh<<8)|m_bLeftValueLow; }
			FvUInt16 GetRightValue() const { return (m_bRightValueHigh<<8)|m_bRightValueLow; }
		};
		FvVector4	m_kGhostRect;
		Node*		m_pkNodes[4];	//! 依次为左,右,下,上
		struct IDGroupAddr
		{
			FvUInt16*m_pkAddr;
			FvUInt32 m_uiSize;
		};
		IDGroupAddr*m_pkIDGroups;
	};
	GhostKDTree	m_kKDTree;
};

struct SpaceInfo
{
	FvUInt16 FindCellIdx(float fX, float fY) const;

	FvUInt16		m_uiSpaceType;
	FvUInt16		m_uiCellCnt;
	FvCellRect		m_kSpaceRect;
	FvString		m_kSpacePath;
	bool			m_bGlobal;

	struct SpaceKDTree
	{
		struct Node
		{
			float	m_fKey;
			struct
			{
				FvUInt8	m_bX : 1;
				FvUInt8	m_bLeftLeaf : 1;
				FvUInt8 m_bLeftValueHigh : 6;
				FvUInt8 m_bLeftValueLow;
				FvUInt8	m_uiR : 1;
				FvUInt8	m_bRightLeaf : 1;
				FvUInt8 m_bRightValueHigh : 6;
				FvUInt8 m_bRightValueLow;
			};
			FvUInt16 GetLeftValue() const { return (m_bLeftValueHigh<<8)|m_bLeftValueLow; }
			FvUInt16 GetRightValue() const { return (m_bRightValueHigh<<8)|m_bRightValueLow; }
		};
		Node*		m_pkNodes;
	};
	SpaceKDTree		m_kKDTree;
	CellData*	m_pkCellDatas;

	SpaceInfo();
	~SpaceInfo();
};

class EntityChannelFinder : public FvNetChannelFinder
{
public:
	EntityChannelFinder(FvNetNub& nub_);
	virtual ~EntityChannelFinder();

	virtual FvNetChannel* Find( FvNetChannelID id, const FvNetPacket * pPacket,
		bool & rHasBeenHandled );

	FvNetChannel*	Find(FvNetChannelID id);
	FvNetChannel*	MakeChannel(FvNetChannelID idx, const FvNetAddress& kBaseAddr);
	FvNetChannel*	TransportIn(FvNetChannelID idx, const FvNetAddress& kBaseAddr, FvBinaryIStream& stream);
	bool			TransportOut(FvNetChannelID id, const FvNetAddress& des, FvBinaryOStream& stream);
	void			DeleteChannel(FvNetChannelID id);
	FvNetAddress*	FindAddr(FvEntityID id);
	void			Dump();

protected:
	typedef std::map<FvNetChannelID, FvNetChannel*> IdxChannelMap;
	IdxChannelMap		m_kChannelMap;
	FvNetNub&			m_kNub;

	typedef std::map<FvNetChannelID, FvNetAddress> TransportChannelMap;
	TransportChannelMap	m_kTransportChannelMap;
};



class FV_CELL_API FvEntityManager : public FvNetTimerExpiryHandler, public FvSingleton< FvEntityManager >
{
public:
	FvEntityManager(FvNetNub& nub);
	~FvEntityManager();

	friend class FvSpace;

	bool				Init(int argc, char *argv[]);
	bool				FinishInit(const FvCellAppInitData & initData);

	FvEntity*			FindEntity(FvEntityID id) const;
	FvCell*				FindCell(FvSpaceID id) const;
	FvSpace*			FindSpace(FvSpaceID id) const;
	double				GameTimeInSeconds() const { return double(m_uiTime)/m_iUpdateHertz; }
	double				GameTimeInSeconds(FvTimeStamp uiTime) const { return double(uiTime)/m_iUpdateHertz; }
	FvTimeStamp			GameTime() const { return m_uiTime; }

	//! 脚本调用接口
	FvEntity*			CreateEntity(const char* pcEntityTypeName, const FvAllData& kAllData, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	FvEntity*			CreateEntity(FvEntityTypeID uiEntityType, const FvAllData& kAllData, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void				CreateEntityOnBaseApp(const char* pcEntityTypeName, const FvAllData* pkAllData, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void				CreateEntityOnBaseApp(FvEntityTypeID uiEntityType, const FvAllData* pkAllData, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	//void				SetBaseAppData(const char* pcKey, value);
	void				DelBaseAppData(const char* pcKey);
	bool				HasStarted() const { return m_bStarted; }
	bool				IsShuttingDown() const { return false; }
	const FvNetAddress&	Address() const { return m_kNub.Address(); }
	float				GetLoad() const { return m_fLoad; }
	bool				IsGlobalMode();
	FvMailBox*			FindGlobalBase(const char* pcLabel) const;
	FvMailBox*			FindGlobalBase(const FvString& kLabel) const;
	FvMailBox*			FindGlobalMailBox(const char* pcLabel) const;
	FvMailBox*			FindGlobalMailBox(const FvString& kLabel) const;
	FvEntityTypeID		EntityNameToIndex(const char* pcEntityTypeName) const;
	FvUInt64			NewGlobalID(FvUInt8 uiIdx);//! 全局id分配,uiIdx是全局id的分组
	//! 

	//! internal interface
	void				Startup(const CellAppInterface::StartupArgs & args);
	void				SetGameTime(const CellAppInterface::SetGameTimeArgs & args);
	void				SetBaseApp(const CellAppInterface::SetBaseAppArgs & args);
	void				ShutDown(const CellAppInterface::ShutDownArgs & args);
	void				ControlledShutDown(const CellAppInterface::ControlledShutDownArgs & args);
	void				AddCell(FvBinaryIStream & data);
	void				SetSharedData(FvBinaryIStream & data);
	void				DelSharedData(FvBinaryIStream & data);
	void				AddGlobalBase(FvBinaryIStream & data);
	void				DelGlobalBase(FvBinaryIStream & data);
	void				AddGlobalMailBox(FvBinaryIStream& kData);
	void				DelGlobalMailBox(FvBinaryIStream& kData);
	void				OnloadTeleportedEntity(const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data);
	void				CreateSpaceIfNecessary(FvBinaryIStream & data);
	void				HandleCellAppMgrBirth(const InterfaceListenerMsg& kMsg);
	void				HandleGlobalAppBirth(const InterfaceListenerMsg& kMsg);
	void				HandleCellAppDeath(const CellAppInterface::HandleCellAppDeathArgs & args);
	void				HandleBaseAppDeath(FvBinaryIStream & data);
	void				RunScript(FvBinaryIStream & data);
	void				CallWatcher(const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data);
	//! 

	static FvEntityManager*FindMessageHandler(const FvNetAddress& srcAddr, FvNetUnpackedMessageHeader& header, FvBinaryIStream& data) { return FvEntityManager::pInstance(); }

	FvNetNub&			Nub() { return m_kNub; }
	FvNetChannel*		FindOrCreateChannel(const FvNetAddress& kAddr) { return &m_kNub.FindOrCreateChannel(kAddr); }
	FvNetChannel*		CellAppMgr() { return m_pkCellAppMgr; }
	const SpaceInfo*	FindSpaceInfo(FvUInt16 uiSpaceType) const;
	void				EraseSpace(FvSpace* pkSpace);
	FvAllCellEntityExports*GetEntityExports() const { return m_pkEntityExports; }
	EntityChannelFinder& GetIdxChannelFinder() { return m_kIdxChannelFinder; }
	void				AddEntity(FvEntity* pkEntity);
	void				DelEntity(FvEntity* pkEntity);
	FvCellAppID			GetAppID() const { return m_iCellAppID; }
	typedef std::map<FvSpaceID, FvSpace *> SpaceMap;
	SpaceMap&			GetSpaceMap() { return m_kSpaceMap; }
	const SpaceMap&		GetSpaceMap() const { return m_kSpaceMap; }
	typedef std::map<FvEntityID, FvEntityPtr> EntityMap;
	EntityMap&			GetEntities() { return m_kAllEntityMap; }
	const EntityMap&	GetEntities() const { return m_kAllEntityMap; }
	bool				FindCellMidPt(FvUInt16 uiSpaceType, FvUInt16 uiCellIdx, FvVector3& kPos) const;
	void				AddDestroyEntity(FvEntity* pkEntity);
	void				RemoveDestroyEntity(FvEntity* pkEntity);

	struct ZoneEntityInfo
	{
		FvUInt32		m_uiGID[4];
		FvString		m_kName;
		FvVector3		m_kPos;
		FvDirection3	m_kDir;
		FvXMLSectionPtr m_spPropSec;
	};
	typedef std::vector<ZoneEntityInfo> ZoneEntityInfoList;
	const ZoneEntityInfoList*FindZoneEntityInfoList(FvUInt16 uiSpaceType) const;
	bool				LocalMailBoxAsRemote() { return m_bLocalMailBoxAsRemote; }

protected:
	virtual int			HandleTimeout( FvNetTimerID id, void * arg );
	DBMgr&				GetDBMgr() { return *m_kDBMgr.GetChannelOwner(); }
	bool				LoadSpaceGeometry(int argc, char *argv[]);
	bool				LoadSpaceInfo();
	bool				LoadZoneEntities(int iSpaceType, int iCellIdx);
	void				LoadZoneEntity(FvCellRect& kZoneRect, const FvString& kDir, const FvString& kFileName, int iSpaceType, int iGridX, int iGridY, bool bInnerZone = false);
	bool				LoadSpaceScheme(int argc, char *argv[]);
	bool				CheckRect(FvCellRect& kRect, int* piGridPos);
	void				AddToCellAppMgr();
	void				InformOfLoadToCellAppMgr();
	void				InitEntityIDClient();
	void				InitGlobalEntityIDClientAndRegister(FvNetAddress& kAddr, bool bBlock);
	void				OnStart();
	FvEntityID			NewEntityID();
	void				InitGlobalID();
	void				SaveGlobalID();

private:
	FvCellAppID				m_iCellAppID;
	FvNetNub&				m_kNub;

	FvNetChannel*			m_pkCellAppMgr;
	FvAnonymousChannelClient m_kDBMgr;
	FvNetAddress			m_kBaseAppAddr;

	SpaceMap				m_kSpaceMap;

	EntityMap				m_kAllEntityMap;
	FvAllCellEntityExports*	m_pkEntityExports;

	typedef std::vector<SpaceInfo*> SpaceInfoList;
	SpaceInfoList			m_kSpaceInfoList;
	FvUInt8					m_uiLoadSpaceGeometryIdx;	//! Space数据载入方案idx
	char*					m_pkSpaceCfgBuf;

	EntityChannelFinder		m_kIdxChannelFinder;

	bool					m_bStarted;
	float					m_fLoad;
	FvUInt64				m_uiDllTickLastTime;

	enum TimeOutType
	{
		TIMEOUT_GAME_TICK,
	};
	FvTimeStamp				m_uiTime;
	FvTimeKeeper*			m_pkTimeKeeper;
	FvInt32					m_iUpdateHertz;
	FvInt32					m_iSystemManagePeriod;
	FvInt32					m_iDllUpdatePeriod;
	FvInt32					m_iSyncTimePeriod;

	SharedData*				m_pkCellAppData;
	SharedData*				m_pkGlobalData;
	typedef std::map<FvString, FvMailBox> GlobalMailBoxMap;
	GlobalMailBoxMap		m_kGlobalMailBoxMap;

	FvIDClient				m_kEntityIDClient;
	FvIDClient				m_kGlobalEntityIDClient;
	bool					m_bGlobalInit;

	typedef std::map<FvUInt16, ZoneEntityInfoList> ZoneEntityInfoSpaceMap;
	ZoneEntityInfoSpaceMap	m_kZoneEntityInfoSpaceMap;

	bool					m_bLocalMailBoxAsRemote;	//! 调用本地MB是否当作异地MB处理

	typedef std::vector<FvEntityPtr> DestroyEntities;
	DestroyEntities	m_kDestroyEntities;

	typedef std::map<FvString, FvMailBox> FvGlobalBases;
	FvGlobalBases			m_kGlobalBases;

	FvXMLSectionPtr			m_spGlobalIDs;
	struct GlobalID
	{
		FvUInt64	m_uiNowID;
		FvUInt64	m_uiMaxID;

		GlobalID(FvUInt64 uiNowID, FvUInt64 uiMaxID):m_uiNowID(uiNowID),m_uiMaxID(uiMaxID){}
	};
	typedef std::vector<std::pair<FvUInt8, GlobalID>> GlobalIDs;
	GlobalIDs				m_kGlobalIDs;
};




#endif // __FvCellEntityManager_H__
