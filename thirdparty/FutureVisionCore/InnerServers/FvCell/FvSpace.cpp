#include "FvSpace.h"
#include "FvCell.h"
#include "FvCellEntity.h"
#include "FvCellEntityManager.h"
#include <FvZoneLoader.h>
#include <FvZoneManager.h>
#include <../FvCellAppManager/FvCellAppManagerInterface.h>
#include <../FvBase/FvBaseAppIntInterface.h>
#include <FvServerConfig.h>
#include <FvEntityExport.h>
#include <FvLogicDllManager.h>
#include "FvSpaceDataTypes.h"



FvSpace::FvSpace( FvSpaceID id, FvUInt16 type, FvUInt16 cellIdx, const FvString& kSpacePath )
:m_iSpaceID(id)
,m_uiSpaceType(type)
,m_kSpacePath(kSpacePath)
,m_iTransitionAndGhostTimerID(NULL)
,m_iAoIAndTrapTimerID(NULL)
,m_iEventSyncTimerID(NULL)
,m_iEntityTickTimerID(NULL)
,m_iSystemManageTimerID(NULL)
,m_iZoneTickTimerID(NULL)
{
	m_pkTimeOfDay = new FvTimeOfDay();

	m_pkSpaceInfo = FvEntityManager::Instance().FindSpaceInfo(type);
	FV_ASSERT(m_pkSpaceInfo);
	m_kCellInfos.resize(m_pkSpaceInfo->m_uiCellCnt);

	for(FvUInt16 i=0; i<m_pkSpaceInfo->m_uiCellCnt; ++i)
	{
		m_kCellInfos[i].m_uiIdx = i;
		m_kCellInfos[i].m_fLoad = 0.0f;
		m_kCellInfos[i].m_pkCellData = &(m_pkSpaceInfo->m_pkCellDatas[i]);
	}
	m_kCellInfos[cellIdx].m_kAddr = FvEntityManager::Instance().Address();

	m_pkCell = new FvCell(*this, &m_kCellInfos[cellIdx]);

	bool bLoadZone = FvServerConfig::Get( "LoadZone", bool(false) );
	if(bLoadZone && m_kCellInfos[cellIdx].m_pkCellData->m_bLoadZone)
	{
		m_spZoneSpace = FvZoneManager::Instance().Space(m_iSpaceID,false);
		if(m_spZoneSpace)
		{
			m_spZoneSpace->Clear();
			FvZoneManager::Instance().DelSpace(m_spZoneSpace.Get());
			m_spZoneSpace = NULL;
			FV_INFO_MSG("FvSpace::FvSpace space id:%d exist,delete this space.",m_iSpaceID);
		}

		m_spZoneSpace = new FvZoneSpace(m_iSpaceID);
		FvZoneDirMapping *pkMapping = m_spZoneSpace->AddMapping(
			FvZoneSpaceEntryID(m_kCellInfos[cellIdx].m_kAddr.m_uiIP,
			m_kCellInfos[cellIdx].m_kAddr.m_uiPort),
			(float*)&FvMatrix::IDENTITY,"General",m_kSpacePath);

		//int iGridMinX = FvZoneSpace::PointToGrid( m_kCellInfos[cellIdx].m_pkCellData->m_kRect.LeftBorder() );
		//int iGridMaxX = FvZoneSpace::PointToGrid( m_kCellInfos[cellIdx].m_pkCellData->m_kRect.RightBorder() );
		//int iGridMinY = FvZoneSpace::PointToGrid( m_kCellInfos[cellIdx].m_pkCellData->m_kRect.BottomBorder() );
		//int iGridMaxY = FvZoneSpace::PointToGrid( m_kCellInfos[cellIdx].m_pkCellData->m_kRect.TopBorder() );
		//! 多载入一圈
		int iGridMinX = FvZoneSpace::PointToGrid( m_kCellInfos[cellIdx].m_pkCellData->m_kRect.LeftBorder() ) -1;
		int iGridMaxX = FvZoneSpace::PointToGrid( m_kCellInfos[cellIdx].m_pkCellData->m_kRect.RightBorder() ) +1;
		int iGridMinY = FvZoneSpace::PointToGrid( m_kCellInfos[cellIdx].m_pkCellData->m_kRect.BottomBorder() ) -1;
		int iGridMaxY = FvZoneSpace::PointToGrid( m_kCellInfos[cellIdx].m_pkCellData->m_kRect.TopBorder() ) +1;
		int iSpaceMinX = FvZoneSpace::PointToGrid( m_pkSpaceInfo->m_kSpaceRect.LeftBorder() );
		int iSpaceMaxX = FvZoneSpace::PointToGrid( m_pkSpaceInfo->m_kSpaceRect.RightBorder() );
		int iSpaceMinY = FvZoneSpace::PointToGrid( m_pkSpaceInfo->m_kSpaceRect.BottomBorder() );
		int iSpaceMaxY = FvZoneSpace::PointToGrid( m_pkSpaceInfo->m_kSpaceRect.TopBorder() );
		if(iGridMinX < iSpaceMinX) iGridMinX = iSpaceMinX;
		if(iGridMaxX > iSpaceMaxX) iGridMaxX = iSpaceMaxX;
		if(iGridMinY < iSpaceMinY) iGridMinY = iSpaceMinY;
		if(iGridMaxY > iSpaceMaxY) iGridMaxY = iSpaceMaxY;
		for(int iX = iGridMinX; iX < iGridMaxX; iX++)
		{
			for(int iY = iGridMinY; iY < iGridMaxY; iY++)
			{
				if ( iX < pkMapping->MinLGridX() || iX > pkMapping->MaxLGridX() ||
					iY < pkMapping->MinLGridY() || iY > pkMapping->MaxLGridY() )
					continue;

				FvString kZoneName = pkMapping->OutsideZoneIdentifier( iX, iY, false );
				FvZone *pkZone = new FvZone( kZoneName, pkMapping );
				pkMapping->pSpace()->AddZone( pkZone );

				if( pkZone && !pkZone->Loading() && !pkZone->Loaded() )
				{
					m_kLoadingZones.push_back( pkZone );
					pkZone->Loading( true );
					pkZone->Mapping()->IncRef();
					FvZoneLoader::Load( pkZone, FvBGTaskManager::DEFAULT );
				}
			}
		}
	}

	//! TODO: 可以根据不同space类型做调整,让小场景更及时
	m_iTransitionAndGhostTimerID = FvEntityManager::Instance().Nub().RegisterTimer(1000000, this);
	m_iAoIAndTrapTimerID = FvEntityManager::Instance().Nub().RegisterTimer(1000000, this);
	m_iEventSyncTimerID = FvEntityManager::Instance().Nub().RegisterTimer(150000, this);
	m_iEntityTickTimerID = FvEntityManager::Instance().Nub().RegisterTimer(333000, this);
	m_iSystemManageTimerID = FvEntityManager::Instance().Nub().RegisterTimer(5000000, this);
	m_iZoneTickTimerID = FvEntityManager::Instance().Nub().RegisterTimer(150000,this);
	//m_iTransitionAndAoiUpdateTimerID = FvEntityManager::Instance().Nub().RegisterTimer(200000, this);
	//m_iEventSyncTimerID = FvEntityManager::Instance().Nub().RegisterTimer(100000, this);
	//m_iEntityTickTimerID = FvEntityManager::Instance().Nub().RegisterTimer(500000, this);
	//m_iSystemManageTimerID = FvEntityManager::Instance().Nub().RegisterTimer(5000000, this);

	if(1)
	{
		CellInfo* pkCellInfo = GetCellInfo();
		//! TODO: 把ghost边界抽出来
		float fGhostSize(100.0f);
		float x = pkCellInfo->m_pkCellData->m_kRect.LeftBorder() - fGhostSize;
		float y = pkCellInfo->m_pkCellData->m_kRect.BottomBorder() - fGhostSize;
		float w = pkCellInfo->m_pkCellData->m_kRect.RightBorder() - pkCellInfo->m_pkCellData->m_kRect.LeftBorder() + fGhostSize*2.0f;
		float h = pkCellInfo->m_pkCellData->m_kRect.TopBorder() - pkCellInfo->m_pkCellData->m_kRect.BottomBorder() + fGhostSize*2.0f;
		//! TODO: 考虑把所有Space的内存分配器统一起来
		m_kAoIMgr.Init(x, y, w, h,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_fGridSize,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiRefPtLvl,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_fMinMoveStep,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiObjInitSize,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiObjIncrSize,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiPosInitSize,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiPosIncrSize,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiEvtInitSize,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiEvtIncrSize,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiAoIExtInitSize,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiAoIExtIncrSize,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiTrapExtInitSize,
						pkCellInfo->m_pkCellData->m_kAoICfg.m_uiTrapExtIncrSize);
	}
}

FvSpace::~FvSpace()
{
	if(m_iTransitionAndGhostTimerID)
	{
		FvEntityManager::Instance().Nub().CancelTimer(m_iTransitionAndGhostTimerID);
		m_iTransitionAndGhostTimerID = NULL;
	}
	if(m_iAoIAndTrapTimerID)
	{
		FvEntityManager::Instance().Nub().CancelTimer(m_iAoIAndTrapTimerID);
		m_iAoIAndTrapTimerID = NULL;
	}
	if(m_iEventSyncTimerID)
	{
		FvEntityManager::Instance().Nub().CancelTimer(m_iEventSyncTimerID);
		m_iEventSyncTimerID = NULL;
	}
	if(m_iEntityTickTimerID)
	{
		FvEntityManager::Instance().Nub().CancelTimer(m_iEntityTickTimerID);
		m_iEntityTickTimerID = NULL;
	}
	if(m_iSystemManageTimerID)
	{
		FvEntityManager::Instance().Nub().CancelTimer(m_iSystemManageTimerID);
		m_iSystemManageTimerID = NULL;
	}
	if(m_iZoneTickTimerID)
	{
		FvEntityManager::Instance().Nub().CancelTimer(m_iZoneTickTimerID);
		m_iZoneTickTimerID = NULL;
	}

	while (!m_kLoadingZones.empty())
	{
		m_kLoadingZones.back()->Mapping()->DecRef();
		m_kLoadingZones.pop_back();
	}

	FV_ASSERT(m_spZoneSpace);
	FvZoneManager::Instance().DelSpace(m_spZoneSpace.Get());

#ifndef __DEV_MODE__
	FV_ASSERT(m_kSpaceEntities.empty());
#endif

	delete m_pkCell;
	delete m_pkTimeOfDay;
}

FvSpace::CellInfo* FvSpace::GetCellAt( float x, float y )
{
	FvVector3 kPos(x,y,0);
	FV_ASSERT(m_pkSpaceInfo);
	if(!m_pkSpaceInfo->m_kSpaceRect.IsPointIn(kPos))
		return NULL;

	FvUInt16 uiCellIdx = m_pkSpaceInfo->FindCellIdx(x, y);
	FV_ASSERT(uiCellIdx < (FvUInt16)m_kCellInfos.size());
	return &(m_kCellInfos[uiCellIdx]);
}

FvSpace::CellInfo* FvSpace::GetCellInfo()
{
	FV_ASSERT(m_pkCell);
	return m_pkCell->GetCellInfo();
}

FvSpace* FvSpace::FindMessageHandler(const FvNetAddress& srcAddr, FvNetUnpackedMessageHeader& header, FvBinaryIStream& data)
{
	FvSpaceID iSpaceID;
	data >> iSpaceID;
	FvSpace* pkSpace = FvEntityManager::Instance().FindSpace(iSpaceID);

	//! TODO: 处理失败情况
	if(!pkSpace)
	{
		if(header.m_uiIdentifier == CellAppInterface::CreateEntity.GetID())
		{
			//! FvVector3 kPos;
			//! FvVector3 kDir;
			//! FvTimeStamp uiGameTime;
			//! FvNetAddress kBaseAddr;
			//! FvEntityTypeID uiEntityTypeID;
			//! FvEntityID iEntityID;
			//! FvDatabaseID iDBID;
			//! FvUInt8 uiInitFlg;
			//! Cell部分数据
			FvVector3 kPos;
			FvDirection3 kDir;
			FvTimeStamp uiGameTime;
			FvNetAddress kBaseAddr;
			FvEntityTypeID uiEntityTypeID;
			FvEntityID iEntityID;
			data >> kPos >> kDir >> uiGameTime >> kBaseAddr >> uiEntityTypeID >> iEntityID;
			FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kBaseAddr);
			pkChannel->Bundle().StartMessage(BaseAppIntInterface::CurrentCell);
			pkChannel->Bundle() << iEntityID << FvSpaceID(0);//! 返回0表示失败
			pkChannel->DelayedSend();
		}

		data.Finish();
	}

	return pkSpace;
}

bool FvSpace::IsToDRunning() const
{
	return m_pkTimeOfDay->IsRunning();
}

void FvSpace::StartToD()
{
	if(!m_pkTimeOfDay->Start(FvEntityManager::Instance().GameTimeInSeconds()))
		return;
	UpdateTOD();
}

void FvSpace::StopToD()
{
	if(!m_pkTimeOfDay->Stop(FvEntityManager::Instance().GameTimeInSeconds()))
		return;
	UpdateTOD();
}

void FvSpace::SetToD(double fDayTime, float fTimeScaling)
{
	if(!m_pkTimeOfDay->Set(FvEntityManager::Instance().GameTimeInSeconds(), fDayTime, fTimeScaling))
		return;
	UpdateTOD();
}

double FvSpace::GetToD()
{
	return m_pkTimeOfDay->GetDayTime(FvEntityManager::Instance().GameTimeInSeconds());
}

float FvSpace::GetToDScaling() const
{
	return m_pkTimeOfDay->GetTimeScaling();
}

void FvSpace::SpaceData( FvBinaryIStream & data )
{
	FvSpaceEntryID kEntryID;
	FvUInt16 uiKey;
	data >> kEntryID >> uiKey;
	if(data.Error())
	{
		FV_ERROR_MSG("%s, stream Err\n", __FUNCTION__);
		return;
	}

	FV_INFO_MSG("%s, SpaceID:%d, Key:%d, EntryID:%s\n", __FUNCTION__, SpaceID(), uiKey, kEntryID.c_str());

	FvString kDataString;
	kDataString.assign((char*)data.Retrieve(0), data.RemainingLength());

	if(uiKey == SPACE_DATA_TOD_KEY)
	{
		FvTimeOfDay kFvTimeOfDay;
		data >> kFvTimeOfDay;
		if(data.Error())
		{
			FV_ERROR_MSG("%s, stream Err\n", __FUNCTION__);
			return;
		}

		*m_pkTimeOfDay = kFvTimeOfDay;
		m_pkCell->SendSpaceDataToAllClient(kEntryID, uiKey, kDataString);
		FvLogicDllManager::Instance().OnSpaceData(SpaceID(), kEntryID, uiKey, kDataString);
	}

	data.Finish();
}

void FvSpace::UpdateGeometry( FvBinaryIStream & data )
{
	//! 按cell顺序的addr
	for(int i=0; i<(int)m_kCellInfos.size(); ++i)
	{
		FvNetAddress kAddr;
		data >> kAddr;
		m_kCellInfos[i].m_kAddr = kAddr;
	}
}

void FvSpace::SerializeTimeOfDay(FvBinaryOStream& kOS)
{
	kOS << *m_pkTimeOfDay;
}

void FvSpace::SpaceGeometryLoaded( FvBinaryIStream & data )
{

}

void FvSpace::ShutDownSpace( const CellAppInterface::ShutDownSpaceArgs & args )
{
	ShutDown();
}

void FvSpace::CreateEntity( FvBinaryIStream & data )
{
	//! FvVector3 kPos;
	//! FvVector3 kDir;
	//! FvNetAddress kBaseAddr;
	//! FvEntityTypeID uiEntityTypeID;
	//! FvEntityID iEntityID;
	//! FvDatabaseID iDBID;
	//! FvUInt8 uiInitFlg;
	//! Cell部分数据
	FvVector3 kPos;
	data >> kPos;
	CreateEntity(kPos, data);
}

void FvSpace::CreateGhost( FvBinaryIStream & data )
{
	//! FvVector3 kPos;
	//! FvVector3 kDir;
	//! FvTimeStamp uiGameTime;
	//! FvNetAddress kBaseAddr;
	//! FvNetAddress kRealAddr;
	//! FvEntityTypeID uiEntityTypeID;
	//! FvEntityID iEntityID;
	//! FvDatabaseID iDBID;
	//! FvUInt16 uiAoIObjMask;
	//! float fAoIVisibility;
	//! FvEventNumber iLastEventNumber;
	//! FvVolatileNumber uiLastVolatileUpdateNumber;
	//! FvUInt32 uiGhostUpdateNumber;
	//! ghost部分数据
	FvVector3 kPos;
	FvDirection3 kDir;
	data >> kPos >> kDir;
	CreateGhost(kPos, kDir, data);
}

void FvSpace::AllSpaceData( FvBinaryIStream & data )
{

}

void FvSpace::ShutDown()
{
	SpaceEntities::size_type i(0);
	for(; i<m_kSpaceEntities.size(); ++i)
	{
		if(m_kSpaceEntities[i]->IsReal())
			m_kSpaceEntities[i]->OnSpaceGone();
		m_kSpaceEntities[i]->Destroy();
	}

	//! 销毁
	FvEntityManager::Instance().EraseSpace(this);
}

void FvSpace::CreateEntity(FvVector3& kPos, FvBinaryIStream & data)
{
	//! FvVector3 kDir;
	//! FvNetAddress kBaseAddr;
	//! FvEntityTypeID uiEntityTypeID;
	//! FvEntityID iEntityID;
	//! FvDatabaseID iDBID;
	//! FvUInt8 uiInitFlg;
	//! Cell部分数据
	FvDirection3 kDir;
	FvNetAddress kBaseAddr;
	FvEntityTypeID uiEntityTypeID;
	FvEntityID iEntityID;
	FvDatabaseID iDBID;
	FvUInt8 uiInitFlg;
	data >> kDir >> kBaseAddr >> uiEntityTypeID >> iEntityID >> iDBID >> uiInitFlg;

	//! 如果iEntityID非0表示创建Entity的Cell部分
	//! 如果iEntityID为0表示创建CellEntity,需要分配
	if(iEntityID == FV_NULL_ENTITY)
	{
		if(iDBID != 0)
		{
			FV_ERROR_MSG("%s, iEntityID==0, iDBID(%I64d)\n", __FUNCTION__, iDBID);
			data.Finish();
			return;
		}

		iEntityID = FvEntityManager::Instance().NewEntityID();
		if(iEntityID == FV_NULL_ENTITY)
		{
			data.Finish();
			return;
		}

		kBaseAddr = FvNetAddress::NONE;
	}

	FV_INFO_MSG("%s, id:%d, dbID:%I64d, type:%d, spaceID:%d, cellIdx:%d\n, kPos(%f,%f,%f), InitFlg:%d\n", __FUNCTION__,
		iEntityID, iDBID, uiEntityTypeID,
		m_iSpaceID, m_pkCell->CellIdx(),
		kPos.x, kPos.y, kPos.z,
		uiInitFlg);

	FV_ASSERT(uiEntityTypeID < FvEntityManager::Instance().GetEntityExports()->uiEntityCnt);
	FvCellEntityExport* pkExport = FvEntityManager::Instance().GetEntityExports()->ppkEntityExport[uiEntityTypeID];
	FV_ASSERT(pkExport);
	FvEntity* pRole = pkExport->pFunCreateEntity();
	pRole->Init(iEntityID, iDBID, pkExport, kPos, kDir, this, kBaseAddr);
	pRole->InitReal(data, uiInitFlg);
}

void FvSpace::AddEntity(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity && pkEntity->GetSpaceID()==SpaceID());

	if(pkEntity->IsReal())
		m_pkCell->AddRealEntity(pkEntity);

	pkEntity->SetEntitiesIdx((int)m_kSpaceEntities.size());
	m_kSpaceEntities.push_back(pkEntity);

	FvEntityManager::Instance().AddEntity(pkEntity);
}

void FvSpace::DelEntity(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity && pkEntity->GetSpaceID()==SpaceID());

	if(pkEntity->IsReal())
		m_pkCell->DelRealEntity(pkEntity);

	int iIdx = pkEntity->GetEntitiesIdx();
	int iCnt = (int)m_kSpaceEntities.size();
	FV_ASSERT(-1 < iIdx && iIdx < iCnt);
	if(iIdx < iCnt -1)
	{
		m_kSpaceEntities[iIdx] = m_kSpaceEntities[iCnt -1];
		m_kSpaceEntities[iIdx]->SetEntitiesIdx(iIdx);
	}
	m_kSpaceEntities.pop_back();
	pkEntity->SetEntitiesIdx(-1);

	FvEntityManager::Instance().DelEntity(pkEntity);
}

void FvSpace::AddEntityForTeleport(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity && pkEntity->GetSpaceID()==SpaceID());

	if(pkEntity->IsReal())
		m_pkCell->AddRealEntity(pkEntity);

	pkEntity->SetEntitiesIdx((int)m_kSpaceEntities.size());
	m_kSpaceEntities.push_back(pkEntity);
}

void FvSpace::DelEntityForTeleport(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity && pkEntity->GetSpaceID()==SpaceID());

	if(pkEntity->IsReal())
		m_pkCell->DelRealEntity(pkEntity);

	int iIdx = pkEntity->GetEntitiesIdx();
	int iCnt = (int)m_kSpaceEntities.size();
	FV_ASSERT(-1 < iIdx && iIdx < iCnt);
	if(iIdx < iCnt -1)
	{
		m_kSpaceEntities[iIdx] = m_kSpaceEntities[iCnt -1];
		m_kSpaceEntities[iIdx]->SetEntitiesIdx(iIdx);
	}
	m_kSpaceEntities.pop_back();
	pkEntity->SetEntitiesIdx(-1);
}

void FvSpace::RealToGhost(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity && !pkEntity->IsReal());
	m_pkCell->DelRealEntity(pkEntity);
}

void FvSpace::GhostToReal(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity && pkEntity->IsReal());
	m_pkCell->AddRealEntity(pkEntity);
}

void FvSpace::CreateGhost(const FvVector3& kPos, const FvDirection3& kDir, FvBinaryIStream& data)
{
	//! FvTimeStamp uiGameTime;
	//! FvNetAddress kBaseAddr;
	//! FvNetAddress kRealAddr;
	//! FvEntityTypeID uiEntityTypeID;
	//! FvEntityID iEntityID;
	//! FvDatabaseID iDBID;
	//! FvUInt16 uiAoIObjMask;
	//! float fAoIVisibility;
	//! FvEventNumber iLastEventNumber;
	//! FvVolatileNumber uiLastVolatileUpdateNumber;
	//! FvUInt32 uiGhostUpdateNumber;
	//! ghost部分数据
	FvTimeStamp uiGameTime;
	FvNetAddress kBaseAddr;
	FvNetAddress kRealAddr;
	FvEntityTypeID uiEntityTypeID;
	FvEntityID iEntityID;
	FvDatabaseID iDBID;
	FvUInt16 uiAoIObjMask;
	float fAoIVisibility;
	data >> uiGameTime >> kBaseAddr >> kRealAddr >> uiEntityTypeID >> iEntityID >> iDBID >> uiAoIObjMask >> fAoIVisibility;

	FV_ASSERT(uiEntityTypeID < FvEntityManager::Instance().GetEntityExports()->uiEntityCnt);
	FvCellEntityExport* pkExport = FvEntityManager::Instance().GetEntityExports()->ppkEntityExport[uiEntityTypeID];
	FV_ASSERT(pkExport);
	FvEntity* pRole = pkExport->pFunCreateEntity();
	pRole->Init(iEntityID, iDBID, pkExport, kPos, kDir, this, kBaseAddr);
	pRole->InitAoIParam(uiAoIObjMask, fAoIVisibility);
	pRole->InitGhost(data, kRealAddr);
}

void FvSpace::ZoneTick()
{
	std::list<FvZone*>::iterator kIt = m_kLoadingZones.begin();
	while(kIt != m_kLoadingZones.end())
	{
		FvZone *pkZone = *kIt;
		if ( pkZone->Loaded() )
		{
			kIt = m_kLoadingZones.erase( kIt );
			pkZone->Loading( false );

			FvZoneDirMapping *pkMapping = pkZone->Mapping();
			if (pkMapping->Condemned())
			{
				pkZone->Space()->EjectLoadedZoneBeforeBinding( pkZone );
				delete pkZone;
				pkMapping->DecRef();
				continue;
			}
			pkMapping->DecRef();

			pkZone->Bind( true );

			if( pkZone->IsOutsideZone() && FvZone::OverlapperFinder() )
			{
				std::vector<FvZone*> kShells = FvZone::OverlapperFinder()( pkZone );
				if( !kShells.empty() )
				{
					for( std::vector<FvZone*>::iterator kIter = kShells.begin(); kIter != kShells.end();
						++kIter )
					{
						(*kIter)->BindPortals( true );
					}
				}
			}

			pkZone->Focus();
		}

		if(kIt != m_kLoadingZones.end())
			kIt++;
	}
}

int FvSpace::HandleTimeout( FvNetTimerID id, void * arg )
{
	if(id == m_iEventSyncTimerID)
	{
		m_pkCell->EventSyncUpdate();
	}
	else if(id == m_iTransitionAndGhostTimerID)
	{
		m_pkCell->TransitionUpdate();
	}
	else if(id == m_iAoIAndTrapTimerID)
	{
		m_pkCell->AoiAndTrapUpdate();
	}
	else if(id == m_iEntityTickTimerID)
	{
		m_pkCell->EntityUpdate();
	}
	else if(id == m_iSystemManageTimerID)
	{
		TrimHistoryEvent();
	}
	else if(id == m_iZoneTickTimerID)
	{
		ZoneTick();
	}

	return 0;
}

void FvSpace::TrimHistoryEvent()
{
	SpaceEntities::size_type i(0);
	for(; i<m_kSpaceEntities.size(); ++i)
	{
		m_kSpaceEntities[i]->TrimHistoryEvent();
	}
}

void FvSpace::UpdateTOD()
{
	FV_INFO_MSG("%s, SpaceID:%d, Key:%d\n", __FUNCTION__, SpaceID(), SPACE_DATA_TOD_KEY);

	FvNetChannel* pkChannel = FvEntityManager::Instance().CellAppMgr();
	FV_ASSERT(pkChannel);
	pkChannel->Bundle().StartMessage(CellAppMgrInterface::UpdateSpaceData);
	pkChannel->Bundle() << SpaceID() << SPACE_DATA_TOD_KEY << *m_pkTimeOfDay;
	pkChannel->DelayedSend();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void AoICache::Create(void* pkObs, void* pkObj)
{
	*(FvUInt32*)&m_spEntity = NULL;
	m_spEntity = (FvEntity*)pkObj;

	m_uiLastEventNumber = 0;
	m_uiLastVolatileUpdateNumber = 0;
	m_fPriority = 0;
	m_uiDetailLevel = 0;
	m_aiLodEventNumbers[0] = 0;
}

void AoICache::Destroy()
{
	m_spEntity = NULL;
}

void AoICache::InAoI(FvEntity* pkEntity)
{
	m_uiLastEventNumber = m_spEntity->LastEventNumber();
	m_uiLastVolatileUpdateNumber = m_spEntity->LastVolatileUpdateNumber();
	//m_uiDetailLevel = m_spEntity->GetEntityDes()->LodLevels().Size();
	FV_ASSERT( m_uiDetailLevel <= MAX_LOD_LEVELS );

	const FvVector3& p1 = pkEntity->GetPos();
	const FvVector3& p2 = m_spEntity->GetPos();
	FvPriority dx = p1.x-p2.x;
	FvPriority dy = p1.y-p2.y;
	FvPriority fAoiPriority = dx*dx + dy*dy;

	////! 更新Lod
	//const FvDataLoDLevels kLodLevels = m_spEntity->GetEntityDes()->LodLevels();
	//if(fAoiPriority < kLodLevels.GetLevel(m_uiDetailLevel).Low())
	//{
	//	do
	//	{
	//		--m_uiDetailLevel;
	//		FV_ASSERT(m_uiDetailLevel >= 0);
	//	}while(kLodLevels.NeedsMoreDetail(m_uiDetailLevel, fAoiPriority));
	//}

	m_fPriority = fAoiPriority;

	//! 发送进入Aoi包
	SendEnterAoi(pkEntity->GetRealChannel());
	pkEntity->OnEnterAoi(*(m_spEntity.Get()));
}

void AoICache::UpdateAoI(FvEntity* pkEntity)
{
	const FvVector3& p1 = pkEntity->GetPos();
	const FvVector3& p2 = m_spEntity->GetPos();
	FvPriority dx = p1.x-p2.x;
	FvPriority dy = p1.y-p2.y;
	FvPriority distSQ = dx*dx + dy*dy;

	////! 更新Lod
	//FvDetailLevel uiNewDetailLevel(m_uiDetailLevel);
	//const FvDataLoDLevels kLodLevels = m_spEntity->GetEntityDes()->LodLevels();
	//if(distSQ < kLodLevels.GetLevel(m_uiDetailLevel).Low())
	//{
	//	do
	//	{
	//		--uiNewDetailLevel;
	//		FV_ASSERT(uiNewDetailLevel >= 0);
	//	}while(kLodLevels.NeedsMoreDetail(uiNewDetailLevel, distSQ));
	//}
	//else if(distSQ > kLodLevels.GetLevel(m_uiDetailLevel).High())
	//{
	//	do
	//	{
	//		++uiNewDetailLevel;
	//		FV_ASSERT(uiNewDetailLevel < kLodLevels.Size());
	//	}while(kLodLevels.NeedsLessDetail(uiNewDetailLevel, distSQ));
	//}

	////! 发送更新Lod包
	//if(uiNewDetailLevel < m_uiDetailLevel)
	//	SendUpdateLod(m_uiDetailLevel-1, uiNewDetailLevel, pkEntity->GetRealChannel());

	//m_uiDetailLevel = uiNewDetailLevel;
	m_fPriority = distSQ;
}

void AoICache::OutAoI(FvEntity* pkEntity)
{
	SendLeaveAoi(pkEntity->GetRealChannel());
	pkEntity->OnLeaveAoi(*(m_spEntity.Get()));
}

void AoICache::EventUpdate(FvEntity* pkEntity)
{
	FvEventHistory& kEventHistory = m_spEntity->GetEventHistory();
	if(kEventHistory.empty())
		return;

	FvNetChannel* pkChannel = pkEntity->GetRealChannel();

	FvEventNumber m_uiNeedEventNumber = m_uiLastEventNumber +1;
	FvEventHistory::const_iterator itrB = kEventHistory.begin();
	FvEventHistory::const_iterator itrE = kEventHistory.end();

	//! 需要整体更新
	if(EventNumberLessThan(m_uiNeedEventNumber, (*itrB)->Number()))
	{
		//! TODO: 当作进出AoI处理,网络数据流量是一样的,可以防止客户端因为消息丢失(方法/属性更新)导致逻辑异常
		//!	增大消息队列的容量,降低需要整体更新的概率
		//SendUpdateLod(m_spEntity->GetEntityDes()->LodLevels().Size()-1, m_uiDetailLevel, pkChannel);
		SendUpdateLod(0, m_uiDetailLevel, pkChannel);
		return;
	}

	FvNetBundle& kBundle = pkChannel->Bundle();

	//! TODO: deque支持[]定位,不需要从头开始遍历

	//! 单个Event更新
	while(itrB != itrE)
	{
		FvEventNumber number = (*itrB)->Number();

		if(EventNumberLessThan(m_uiLastEventNumber, number))
		{
			if((*itrB)->ShouldSend(m_fPriority, m_uiDetailLevel))
			{
				(*itrB)->AddToBundle(kBundle);
				//if((*itrB)->IsStateChange())
				//{
				//	FvDetailLevel level = (*itrB)->GetLevel();
				//	if(EventNumberLessThan(m_aiLodEventNumbers[level], number))
				//	{
				//		(*itrB)->AddToBundle(kBundle);
				//		m_aiLodEventNumbers[level] = number;
				//	}
				//}
				//else
				//{
				//	(*itrB)->AddToBundle(kBundle);
				//}
			}

			m_uiLastEventNumber = number;
		}

		++itrB;
	}
}

bool AoICache::EventNumberLessThan(FvEventNumber a, FvEventNumber b)
{
	return (( a - b ) & 0xFFFFFFFF) > 0x80000000;
}

void AoICache::SendEnterAoi(FvNetChannel* pkChannel)
{
	FV_ASSERT(pkChannel);

	//! 直接发送创建实体
	FvNetBundle& kBundle = pkChannel->Bundle();

	int s1 = kBundle.Size();
	int p1 = kBundle.SizeInPackets();
	int n1 = kBundle.NumMessages();

	kBundle.StartMessage(BaseAppIntInterface::CreateEntity);
	kBundle << m_spEntity->GetEntityID() << m_spEntity->GetEntityTypeID() << m_spEntity->GetPos() << m_spEntity->GetDir() << m_spEntity->GetVehicleID();

	m_spEntity->GetAttrib().SerializeToStreamForOthersCliData(kBundle);

	////! 发送能看见的Lod的属性
	//FvUInt16* pPropCnt = (FvUInt16*)kBundle.Reserve(sizeof(FvUInt16));
	//FvUInt16 cnt(0);
	//const FvEntityDescription* pkEntityDes = m_spEntity->GetEntityDes();
	//FvUInt32 uiPropCnt = pkEntityDes->ClientServerPropertyCount();
	//for(FvUInt32 index=0; index<uiPropCnt; ++index)
	//{
	//	FvDataDescription* pkDataDes = pkEntityDes->ClientServerProperty(index);
	//	FV_ASSERT(pkDataDes);
	//	if(pkDataDes->IsOtherClientData() && pkDataDes->DetailLevel()>=m_uiDetailLevel)
	//	{
	//		kBundle << FvUInt16(index);
	//		FvObjPtr spObj = m_spEntity->FindProperty(pkDataDes->Index());
	//		FV_ASSERT(spObj);
	//		pkDataDes->AddToStream(spObj, kBundle, false);
	//		++cnt;
	//	}
	//}
	//*pPropCnt = cnt;

	int s2 = kBundle.Size();
	int p2 = kBundle.SizeInPackets();
	int n2 = kBundle.NumMessages();
	FV_INFO_MSG("EnterAoi, %d See %d, Size:%d, Packet:%d, Message:%d\n", pkChannel->id(), m_spEntity->GetEntityID(), s2, p2, n2);
}

void AoICache::SendLeaveAoi(FvNetChannel* pkChannel)
{
	FV_ASSERT(pkChannel);

	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::LeaveAoI);
	kBundle << m_spEntity->GetEntityID();
}

void AoICache::SendLeaveAoi(FvNetChannel* pkChannel, FvEntityID iEntityID)
{
	FV_ASSERT(pkChannel);

	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::LeaveAoI);
	kBundle << iEntityID;
}

void AoICache::SendUpdateLod(FvDetailLevel uiFrom, FvDetailLevel uiTo, FvNetChannel* pkChannel)
{
	//FV_ASSERT(uiFrom >= uiTo);
	//for(FvDetailLevel l=uiFrom; l>=uiTo; --l)
	//{
	//	m_aiLodEventNumbers[l] = m_spEntity->LastEventNumber();	//! 更新到当前最新
	//	if(l == 0)
	//		break;
	//}

	//! 发送新可见的Lod的属性
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::UpdateEntity);
	kBundle << m_spEntity->GetEntityID();

	m_spEntity->GetAttrib().SerializeToStreamForOthersCliData(kBundle);

	//FvUInt16* pPropCnt = (FvUInt16*)kBundle.Reserve(sizeof(FvUInt16));
	//FvUInt16 cnt(0);
	//const FvEntityDescription* pkEntityDes = m_spEntity->GetEntityDes();
	//FvUInt32 uiPropCnt = pkEntityDes->ClientServerPropertyCount();
	//for(FvUInt32 index=0; index<uiPropCnt; ++index)
	//{
	//	FvDataDescription* pkDataDes = pkEntityDes->ClientServerProperty(index);
	//	FV_ASSERT(pkDataDes);
	//	if(pkDataDes->IsOtherClientData() && uiTo<=pkDataDes->DetailLevel() && pkDataDes->DetailLevel()<=uiFrom)
	//	{
	//		kBundle << FvUInt16(index);
	//		FvObjPtr spObj = m_spEntity->FindProperty(pkDataDes->Index());
	//		FV_ASSERT(spObj);
	//		pkDataDes->AddToStream(spObj, kBundle, false);
	//		++cnt;
	//	}
	//}
	//*pPropCnt = cnt;
}

void AoICache::SteamOut(FvBinaryOStream& kStream)
{
	//FV_ASSERT(m_spEntity->GetEntityDes()->LodLevels().Size() > m_uiDetailLevel);
	//FvUInt8 uiSeenLods = m_spEntity->GetEntityDes()->LodLevels().Size() - m_uiDetailLevel;
	FvUInt8 uiSeenLods = 0;
	kStream << m_spEntity->GetEntityID()
		<< m_spEntity->GetVehicleID()
		<< m_uiLastEventNumber
		<< m_uiLastVolatileUpdateNumber
		<< m_fPriority
		<< m_uiDetailLevel
		<< uiSeenLods;
	for(FvUInt8 i=0; i<uiSeenLods; ++i)
	{
		kStream << m_aiLodEventNumbers[m_uiDetailLevel + i];
	}
}

void AoICache::SteamIn(FvBinaryIStream& kStream, FvEntityID& iEntityID, FvEntityID& iVehicleID)
{
	FvUInt8 uiSeenLods;
	kStream >> iEntityID
		>> iVehicleID
		>> m_uiLastEventNumber
		>> m_uiLastVolatileUpdateNumber
		>> m_fPriority
		>> m_uiDetailLevel
		>> uiSeenLods;
	for(FvUInt8 i=0; i<uiSeenLods; ++i)
	{
		kStream >> m_aiLodEventNumbers[m_uiDetailLevel + i];
	}
}

/**
void AoICache::Create(void* pkObs, void* pkObj)
{
	*(FvUInt32*)&m_spEntity = NULL;
	m_spEntity = (FvEntity*)pkObj;

	m_uiLastEventNumber = 0;
	m_uiLastVolatileUpdateNumber = 0;
	m_fPriority = 0;
	m_uiDetailLevel = 0;
	m_aiLodEventNumbers[0] = 0;
}

void AoICache::Destroy()
{
	m_spEntity = NULL;
}

void AoICache::InAoI(FvEntity* pkEntity)
{
	m_uiLastEventNumber = m_spEntity->LastEventNumber();
	m_uiLastVolatileUpdateNumber = m_spEntity->LastVolatileUpdateNumber();
	m_uiDetailLevel = m_spEntity->GetEntityDes()->LodLevels().Size();
	FV_ASSERT( m_uiDetailLevel <= MAX_LOD_LEVELS );

	const FvVector3& p1 = pkEntity->GetPos();
	const FvVector3& p2 = m_spEntity->GetPos();
	FvPriority dx = p1.x-p2.x;
	FvPriority dy = p1.y-p2.y;
	FvPriority fAoiPriority = dx*dx + dy*dy;

	//! 更新Lod
	const FvDataLoDLevels kLodLevels = m_spEntity->GetEntityDes()->LodLevels();
	if(fAoiPriority < kLodLevels.GetLevel(m_uiDetailLevel).Low())
	{
		do
		{
			--m_uiDetailLevel;
			FV_ASSERT(m_uiDetailLevel >= 0);
		}while(kLodLevels.NeedsMoreDetail(m_uiDetailLevel, fAoiPriority));
	}

	m_fPriority = fAoiPriority;

	//! 发送进入Aoi包
	SendEnterAoi(pkEntity->GetRealChannel());
	pkEntity->OnEnterAoi(m_spEntity.Get());
}

void AoICache::UpdateAoI(FvEntity* pkEntity)
{
	const FvVector3& p1 = pkEntity->GetPos();
	const FvVector3& p2 = m_spEntity->GetPos();
	FvPriority dx = p1.x-p2.x;
	FvPriority dy = p1.y-p2.y;
	FvPriority distSQ = dx*dx + dy*dy;

	//! 更新Lod
	FvDetailLevel uiNewDetailLevel(m_uiDetailLevel);
	const FvDataLoDLevels kLodLevels = m_spEntity->GetEntityDes()->LodLevels();
	if(distSQ < kLodLevels.GetLevel(m_uiDetailLevel).Low())
	{
		do
		{
			--uiNewDetailLevel;
			FV_ASSERT(uiNewDetailLevel >= 0);
		}while(kLodLevels.NeedsMoreDetail(uiNewDetailLevel, distSQ));
	}
	else if(distSQ > kLodLevels.GetLevel(m_uiDetailLevel).High())
	{
		do
		{
			++uiNewDetailLevel;
			FV_ASSERT(uiNewDetailLevel < kLodLevels.Size());
		}while(kLodLevels.NeedsLessDetail(uiNewDetailLevel, distSQ));
	}

	//! 发送更新Lod包
	if(uiNewDetailLevel < m_uiDetailLevel)
		SendUpdateLod(m_uiDetailLevel-1, uiNewDetailLevel, pkEntity->GetRealChannel());

	m_uiDetailLevel = uiNewDetailLevel;
	m_fPriority = distSQ;
}

void AoICache::OutAoI(FvEntity* pkEntity)
{
	SendLeaveAoi(pkEntity->GetRealChannel());
	pkEntity->OnLeaveAoi(m_spEntity.Get());
}

void AoICache::EventUpdate(FvEntity* pkEntity)
{
	FvEventHistory& kEventHistory = m_spEntity->GetEventHistory();
	if(kEventHistory.empty())
		return;

	FvNetChannel* pkChannel = pkEntity->GetRealChannel();

	FvEventNumber m_uiNeedEventNumber = m_uiLastEventNumber +1;
	FvEventHistory::const_iterator itrB = kEventHistory.begin();
	FvEventHistory::const_iterator itrE = kEventHistory.end();

	//! 需要整体更新
	if(EventNumberLessThan(m_uiNeedEventNumber, (*itrB)->Number()))
	{
		//! TODO: 当作进出AoI处理,网络数据流量是一样的,可以防止客户端因为消息丢失(方法/属性更新)导致逻辑异常
		//!	增大消息队列的容量,降低需要整体更新的概率
		SendUpdateLod(m_spEntity->GetEntityDes()->LodLevels().Size()-1, m_uiDetailLevel, pkChannel);
		return;
	}

	FvNetBundle& kBundle = pkChannel->Bundle();

	//! TODO: deque支持[]定位,不需要从头开始遍历

	//! 单个Event更新
	while(itrB != itrE)
	{
		FvEventNumber number = (*itrB)->Number();

		if(EventNumberLessThan(m_uiLastEventNumber, number))
		{
			if((*itrB)->ShouldSend(m_fPriority, m_uiDetailLevel))
			{
				if((*itrB)->IsStateChange())
				{
					FvDetailLevel level = (*itrB)->GetLevel();
					if(EventNumberLessThan(m_aiLodEventNumbers[level], number))
					{
						(*itrB)->AddToBundle(kBundle);
						m_aiLodEventNumbers[level] = number;
					}
				}
				else
				{
					(*itrB)->AddToBundle(kBundle);
				}
			}

			m_uiLastEventNumber = number;
		}

		++itrB;
	}
}

bool AoICache::EventNumberLessThan(FvEventNumber a, FvEventNumber b)
{
	return (( a - b ) & 0xFFFFFFFF) > 0x80000000;
}

void AoICache::SendEnterAoi(FvNetChannel* pkChannel)
{
	FV_ASSERT(pkChannel);

	//! 直接发送创建实体
	FvNetBundle& kBundle = pkChannel->Bundle();

	int s1 = kBundle.Size();
	int p1 = kBundle.SizeInPackets();
	int n1 = kBundle.NumMessages();

	kBundle.StartMessage(BaseAppIntInterface::CreateEntity);
	kBundle << m_spEntity->GetEntityID() << m_spEntity->GetEntityTypeID() << m_spEntity->GetPos() << m_spEntity->GetDir() << m_spEntity->GetVehicleID();

	//! 发送能看见的Lod的属性
	FvUInt16* pPropCnt = (FvUInt16*)kBundle.Reserve(sizeof(FvUInt16));
	FvUInt16 cnt(0);
	const FvEntityDescription* pkEntityDes = m_spEntity->GetEntityDes();
	FvUInt32 uiPropCnt = pkEntityDes->ClientServerPropertyCount();
	for(FvUInt32 index=0; index<uiPropCnt; ++index)
	{
		FvDataDescription* pkDataDes = pkEntityDes->ClientServerProperty(index);
		FV_ASSERT(pkDataDes);
		if(pkDataDes->IsOtherClientData() && pkDataDes->DetailLevel()>=m_uiDetailLevel)
		{
			kBundle << FvUInt16(index);
			FvObjPtr spObj = m_spEntity->FindProperty(pkDataDes->Index());
			FV_ASSERT(spObj);
			pkDataDes->AddToStream(spObj, kBundle, false);
			++cnt;
		}
	}
	*pPropCnt = cnt;

	int s2 = kBundle.Size();
	int p2 = kBundle.SizeInPackets();
	int n2 = kBundle.NumMessages();
	FV_INFO_MSG("EnterAoi, %d See %d, Size:%d, Packet:%d, Message:%d\n", pkChannel->id(), m_spEntity->GetEntityID(), s2, p2, n2);
}

void AoICache::SendLeaveAoi(FvNetChannel* pkChannel)
{
	FV_ASSERT(pkChannel);

	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::LeaveAoI);
	kBundle << m_spEntity->GetEntityID();
}

void AoICache::SendLeaveAoi(FvNetChannel* pkChannel, FvEntityID iEntityID)
{
	FV_ASSERT(pkChannel);

	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::LeaveAoI);
	kBundle << iEntityID;
}

void AoICache::SendUpdateLod(FvDetailLevel uiFrom, FvDetailLevel uiTo, FvNetChannel* pkChannel)
{
	FV_ASSERT(uiFrom >= uiTo);
	for(FvDetailLevel l=uiFrom; l>=uiTo; --l)
	{
		m_aiLodEventNumbers[l] = m_spEntity->LastEventNumber();	//! 更新到当前最新
		if(l == 0)
			break;
	}

	//! 发送新可见的Lod的属性
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::UpdateEntity);
	kBundle << m_spEntity->GetEntityID();
	FvUInt16* pPropCnt = (FvUInt16*)kBundle.Reserve(sizeof(FvUInt16));
	FvUInt16 cnt(0);
	const FvEntityDescription* pkEntityDes = m_spEntity->GetEntityDes();
	FvUInt32 uiPropCnt = pkEntityDes->ClientServerPropertyCount();
	for(FvUInt32 index=0; index<uiPropCnt; ++index)
	{
		FvDataDescription* pkDataDes = pkEntityDes->ClientServerProperty(index);
		FV_ASSERT(pkDataDes);
		if(pkDataDes->IsOtherClientData() && uiTo<=pkDataDes->DetailLevel() && pkDataDes->DetailLevel()<=uiFrom)
		{
			kBundle << FvUInt16(index);
			FvObjPtr spObj = m_spEntity->FindProperty(pkDataDes->Index());
			FV_ASSERT(spObj);
			pkDataDes->AddToStream(spObj, kBundle, false);
			++cnt;
		}
	}
	*pPropCnt = cnt;
}

void AoICache::SteamOut(FvBinaryOStream& kStream)
{
	FV_ASSERT(m_spEntity->GetEntityDes()->LodLevels().Size() > m_uiDetailLevel);
	FvUInt8 uiSeenLods = m_spEntity->GetEntityDes()->LodLevels().Size() - m_uiDetailLevel;
	kStream << m_spEntity->GetEntityID()
		<< m_spEntity->GetVehicleID()
		<< m_uiLastEventNumber
		<< m_uiLastVolatileUpdateNumber
		<< m_fPriority
		<< m_uiDetailLevel
		<< uiSeenLods;
	for(FvUInt8 i=0; i<uiSeenLods; ++i)
	{
		kStream << m_aiLodEventNumbers[m_uiDetailLevel + i];
	}
}

void AoICache::SteamIn(FvBinaryIStream& kStream, FvEntityID& iEntityID, FvEntityID& iVehicleID)
{
	FvUInt8 uiSeenLods;
	kStream >> iEntityID
		>> iVehicleID
		>> m_uiLastEventNumber
		>> m_uiLastVolatileUpdateNumber
		>> m_fPriority
		>> m_uiDetailLevel
		>> uiSeenLods;
	for(FvUInt8 i=0; i<uiSeenLods; ++i)
	{
		kStream >> m_aiLodEventNumbers[m_uiDetailLevel + i];
	}
}
**/
