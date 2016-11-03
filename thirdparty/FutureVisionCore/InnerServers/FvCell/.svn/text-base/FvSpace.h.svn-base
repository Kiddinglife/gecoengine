//{future header message}
#ifndef __FvSpace_H__
#define __FvSpace_H__

#include "FvCellDefines.h"
#include "FvCellAppInterface.h"
#include <FvNetTypes.h>
#include <FvCellRect.h>
#include <FvZoneSpace.h>
#include "FvAoIMgr.h"

#include <vector>

class FvBinaryIStream;
class FvCell;
class FvEntity;
struct CellData;
struct SpaceInfo;
typedef FvSmartPointer< FvEntity > FvEntityPtr;
class FvTimeOfDay;

struct AoICache : public FvAoIExt
{
	static const int MAX_LOD_LEVELS = 4;
	typedef double FvPriority;

	FvEntityPtr			m_spEntity;
	FvEventNumber		m_uiLastEventNumber;
	FvVolatileNumber	m_uiLastVolatileUpdateNumber;
	FvPriority			m_fPriority;
	FvDetailLevel		m_uiDetailLevel;

	FvEventNumber		m_aiLodEventNumbers[ MAX_LOD_LEVELS ];	//! 记录各级Lod EventNumber的用途:
																//! 在遍历事件列表做属性同步/事件更新时,需判断对应Lod的EventNumber,防止重复更新
																//! 如:一共3级Lod,最新的消息id为100,上次遍历的消息id为80
																//! 上次能看到Lod2,消息id为75
																//! 这次新看到Lod1,先做全部更新,消息id为100
																//! 然后遍历消息队列,对Lod1和2做属性同步
																//! 如果不判断各级Lod的消息id,则此时Lod1会重复更新

	void Create(void* pkObs, void* pkObj);
	void Destroy();

	void InAoI(FvEntity* pkEntity);
	void UpdateAoI(FvEntity* pkEntity);
	void OutAoI(FvEntity* pkEntity);
	void EventUpdate(FvEntity* pkEntity);

	bool EventNumberLessThan(FvEventNumber a, FvEventNumber b);
	void SendEnterAoi(FvNetChannel* pkChannel);
	void SendLeaveAoi(FvNetChannel* pkChannel);
	void SendLeaveAoi(FvNetChannel* pkChannel, FvEntityID iEntityID);
	void SendUpdateLod(FvDetailLevel uiFrom, FvDetailLevel uiTo, FvNetChannel* pkChannel);

	void SteamOut(FvBinaryOStream& kStream);
	void SteamIn(FvBinaryIStream& kStream, FvEntityID& iEntityID, FvEntityID& iVehicleID);
};

struct TrapCache : public FvAoIExt
{
	FvEntityPtr	m_spEntity;

	void Create(void* pkObs, void* pkObj)
	{
		*(FvUInt32*)&m_spEntity = NULL;
		m_spEntity = (FvEntity*)pkObj;
	}

	void Destroy()
	{
		m_spEntity = NULL;
	}
};


class FV_CELL_API FvSpace : public FvNetTimerExpiryHandler
{
public:
	FvSpace( FvSpaceID id, FvUInt16 type, FvUInt16 cellIdx, const FvString& kSpacePath );
	virtual ~FvSpace();

	struct CellInfo
	{
		FvUInt16		m_uiIdx;
		FvNetAddress	m_kAddr;
		float			m_fLoad;
		CellData*		m_pkCellData;
	};
	typedef std::vector<CellInfo> CellInfos;

	FvSpaceID		SpaceID() const { return m_iSpaceID; }
	FvUInt16		SpaceType() const { return m_uiSpaceType; }
	const FvString&	SpacePath() const { return m_kSpacePath; }
	CellInfo*		GetCellAt( float x, float y );
	FvCell*			GetCell() { return m_pkCell; }
	CellInfo*		GetCellInfo();
	CellInfos&		GetCellInfos() { return m_kCellInfos; }
	static FvSpace* FindMessageHandler(const FvNetAddress& srcAddr, FvNetUnpackedMessageHeader& header, FvBinaryIStream& data);

	bool			IsToDRunning() const;
	void			StartToD();
	void			StopToD();
	void			SetToD(double fDayTime, float fTimeScaling);
	double			GetToD();
	float			GetToDScaling() const;

	//! internal interface
	void			SpaceData( FvBinaryIStream & data );
	void			UpdateGeometry( FvBinaryIStream & data );
	void			SpaceGeometryLoaded( FvBinaryIStream & data );
	void			ShutDownSpace( const CellAppInterface::ShutDownSpaceArgs & args );
	void			CreateEntity( FvBinaryIStream & data );
	void			CreateGhost( FvBinaryIStream & data );
	void			AllSpaceData( FvBinaryIStream & data );
	//! 

	void			ShutDown();
	void			CreateEntity(FvVector3& kPos, FvBinaryIStream & data);
	void			AddEntity(FvEntity* pkEntity);
	void			DelEntity(FvEntity* pkEntity);
	void			AddEntityForTeleport(FvEntity* pkEntity);
	void			DelEntityForTeleport(FvEntity* pkEntity);
	void			RealToGhost(FvEntity* pkEntity);
	void			GhostToReal(FvEntity* pkEntity);
	typedef std::vector<FvEntity*> SpaceEntities;
	SpaceEntities&	GetSpaceEntities() { return m_kSpaceEntities; }
	void			CreateGhost(const FvVector3& kPos, const FvDirection3& kDir, FvBinaryIStream& data);

	void			ZoneTick();
	bool			IsSpaceLoaded() const { return m_kLoadingZones.empty(); }

	FvAoIMgr<AoICache, TrapCache>& GetAoIMgr() { return m_kAoIMgr; }

	FvZoneSpace* GetZoneSpace()const{return m_spZoneSpace.Get();}

	void			SerializeTimeOfDay(FvBinaryOStream& kOS);

protected:
	virtual int		HandleTimeout( FvNetTimerID id, void * arg );
	void			TrimHistoryEvent();
	void			UpdateTOD();

	FvSpaceID			m_iSpaceID;
	FvUInt16			m_uiSpaceType;
	FvString			m_kSpacePath;
	CellInfos			m_kCellInfos;
	const SpaceInfo*	m_pkSpaceInfo;
	FvCell*				m_pkCell;
	FvZoneSpacePtr		m_spZoneSpace;

	SpaceEntities		m_kSpaceEntities;

	std::list< FvZone* > m_kLoadingZones;

	FvTimeOfDay*		m_pkTimeOfDay;

	FvNetTimerID		m_iTransitionAndGhostTimerID;
	FvNetTimerID		m_iAoIAndTrapTimerID;
	FvNetTimerID		m_iEventSyncTimerID;
	FvNetTimerID		m_iEntityTickTimerID;
	FvNetTimerID		m_iSystemManageTimerID;
	FvNetTimerID		m_iZoneTickTimerID;

	FvAoIMgr<AoICache, TrapCache> m_kAoIMgr;
};


#include "FvSpace.inl"

#endif // __FvSpace_H__
