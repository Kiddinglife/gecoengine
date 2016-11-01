//{future header message}
#ifndef __FvCellAppManager_H__
#define __FvCellAppManager_H__

#include "FvCellApp.h"
#include "FvCellAppManagerInterface.h"

#include <FvSingleton.h>
#include <FvNetChannel.h>
#include <FvServerCommon.h>
#include <FvCellRect.h>

#include <map>
#include <set>
#include <deque>
#include <string>

class FvTimeKeeper;
class FvSpace;
class FvCellApp;
typedef FvNetChannelOwner FvBaseAppMgr;
typedef FvNetChannelOwner FvDBMgr;

struct CellInfo
{
	FvCellRect				m_kRect;
	bool					m_bLoadEntity;
	bool					m_bLoadZone;
	std::vector<FvCellApp*>	m_kApps;

	void	AddCellApp(FvCellApp* pkApp);
	void	RemoveCellApp(FvCellApp* pkApp);
};

struct SpaceKDTree
{
	FvUInt16 FindCellIdx(float fX, float fY) const;

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

struct SpaceInfo
{
	FvUInt16		m_uiSpaceType;
	FvUInt16		m_uiCellCnt;
	FvCellRect		m_kSpaceRect;
	FvString		m_kSpacePath;
	bool			m_bGlobal;
	CellInfo*		m_pkCellInfo;
	SpaceKDTree		m_kKDTree;

	~SpaceInfo();
	bool			CheckSpaceReady();
};

class FvCellAppMgr : public FvNetTimerExpiryHandler,
	public FvSingleton< FvCellAppMgr >
{
public:
	FvCellAppMgr( FvNetNub & nub );
	virtual ~FvCellAppMgr();
	bool Init(int argc, char* argv[]);

	void ShutDown( bool shutDownOthers );

	FvNetNub& Nub()				{ return m_kNub; }
	FvBaseAppMgr& BaseAppMgr()	{ return m_kBaseAppMgr; }
	FvDBMgr& DBMgr()			{ return *m_kDBMgr.GetChannelOwner(); }
	FvCellApp* FindCellApp(const FvNetAddress& kAddr);
	FvSpace* FindSpace(FvSpaceID iSpaceID);
	double GameTimeInSeconds() const{ return double(m_uiTime)/m_iUpdateHertz; }
	bool IsGlobalMode();

	virtual int HandleTimeout( FvNetTimerID id, void * arg );

	void CreateEntity( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void CreateEntityInNewSpace( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void PrepareForRestoreFromDB( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void Startup( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void ShutDown( const CellAppMgrInterface::ShutDownArgs & args );
	void ControlledShutDown( const CellAppMgrInterface::ControlledShutDownArgs & args );
	void ShouldOffload( const CellAppMgrInterface::ShouldOffloadArgs & args );
	void RunScript( FvBinaryIStream & data );
	void AddApp( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void RecoverCellApp( FvBinaryIStream & data );
	void DelApp( const CellAppMgrInterface::DelAppArgs & args );
	void SetBaseApp( const CellAppMgrInterface::SetBaseAppArgs & args );
	void HandleCellAppMgrBirth(const InterfaceListenerMsg& kMsg);
	void HandleBaseAppMgrBirth(const InterfaceListenerMsg& kMsg);
	void HandleCellAppDeath(const InterfaceListenerMsg& kMsg);
	void HandleBaseAppDeath( FvBinaryIStream & data );
	void AckCellAppDeath( const FvNetAddress & addr, const CellAppMgrInterface::AckCellAppDeathArgs & args );
	void GameTimeReading( const FvNetAddress & srcAddr, FvNetReplyID replyID, const CellAppMgrInterface::GameTimeReadingArgs & args );
	void UpdateSpaceData( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void ShutDownSpace( const CellAppMgrInterface::ShutDownSpaceArgs & args );
	void AckBaseAppsShutDown( const CellAppMgrInterface::AckBaseAppsShutDownArgs & args );
	void CheckStatus( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void SetSharedData( FvBinaryIStream & data );
	void DelSharedData( FvBinaryIStream & data );
	void AddGlobalBase( FvBinaryIStream & data );
	void DelGlobalBase( FvBinaryIStream & data );

protected:
	bool		LoadSpaceData();
	bool		LoadSpaceInfo();
	bool		LoadSpaceScheme();
	bool		CheckRect(FvCellRect& kRect, int* piGridPos);
	FvSpaceID	GetSpaceID();
	void		PutSpaceID(FvSpaceID iSpaceID);
	void		CheckSpaceReady();
	void		CheckOverLoad();
	void		CheckSpaceEmpty();
	void		CheckDeadCellApp();
	FvSpace*	CreateNewSpace(FvUInt16 uiSpaceType);
	void		OnStartup();
	void		SendToCellApps(const FvNetInterfaceElement& ifElt, FvMemoryOStream& args);
	bool		IsValidSpaceID(FvSpaceID iSpaceID);

private:
	FvNetNub &					m_kNub;

	FvBaseAppMgr				m_kBaseAppMgr;
	FvAnonymousChannelClient	m_kDBMgr;

	bool						m_bRun;
	bool						m_bSpaceReady;

	FvCellAppID					m_iCellAppID;
	typedef std::map<FvNetAddress, FvCellApp*> CellAppMap;
	CellAppMap					m_kCellAppMap;

	FvNetAddress				m_kBaseAppAddr;

	FvSpaceID					m_iSpaceID;
	FvSpaceID					m_iMaxSpaceID;
	typedef std::deque<FvSpaceID> FreeSpaceIDList;
	FreeSpaceIDList				m_kFreeSpaceIDList;
	typedef std::map<FvSpaceID, FvSpace*> SpaceMap;
	SpaceMap					m_kSpaceMap;

	enum TimeOutType
	{
		TIMEOUT_GAME_TICK
	};
	FvTimeStamp					m_uiTime;
	FvTimeKeeper*				m_pkTimeKeeper;
	FvInt32						m_iUpdateHertz;
	FvInt32						m_iCellAppTimeoutPeriod;
	FvInt32						m_iCellAppTimeoutTime;
	FvInt32						m_iSystemManagePeriod;


	struct LoadSpaceGeometry
	{
		FvUInt16	m_uiSpaceType;
		FvUInt16	m_uiCellIdx;
	};
	typedef std::vector<LoadSpaceGeometry> LoadSpaceGeometrys;
	typedef std::vector<LoadSpaceGeometrys> LoadSpaceGeometryList;
	LoadSpaceGeometryList		m_kLoadSpaceGeometryList;

	typedef std::vector<SpaceInfo*>	SpaceInfoList;
	SpaceInfoList				m_kSpaceInfoList;
	char*						m_pkSpaceCfgBuf;

	typedef std::vector<FvCellApp*> DeadCellAppList;
	DeadCellAppList				m_kDeadApps;

	typedef std::map<FvString, FvEntityMailBoxRef> FvGlobalBases;
	FvGlobalBases	m_kGlobalBases;
};


#endif//__FvCellAppManager_H__
