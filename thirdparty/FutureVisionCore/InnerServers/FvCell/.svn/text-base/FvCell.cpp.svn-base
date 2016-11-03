#include "FvCell.h"
#include "FvCellEntity.h"
#include "FvCellEntityManager.h"


FvCell::FvCell(FvSpace& space, FvSpace::CellInfo* pkCellInfo)
:m_kSpace(space)
,m_pkCellInfo(pkCellInfo)
,m_fCellHysteresisSize(100.0f)
{
	m_uiEntityTickLastTime = Timestamp();
}

FvCell::~FvCell()
{
}

FvCell * FvCell::FindMessageHandler(const FvNetAddress& srcAddr, FvNetUnpackedMessageHeader& header, FvBinaryIStream& data)
{
	FvSpaceID iSpaceID;
	data >> iSpaceID;
	FvCell* pkCell = FvEntityManager::Instance().FindCell(iSpaceID);

	//! TODO: 处理失败情况
	if(!pkCell)
	{

		data.Finish();
	}

	return pkCell;
}

FvUInt16 FvCell::CellIdx()
{
	return m_pkCellInfo->m_uiIdx;
}

FvSpaceID FvCell::SpaceID()
{
	return m_kSpace.SpaceID();
}

FvUInt16 FvCell::SpaceType()
{
	return m_kSpace.SpaceType();
}

bool FvCell::IsInCell(const FvVector3& kPos) const
{
	return m_pkCellInfo->m_pkCellData->m_kRect.IsPointIn(kPos);
}

void FvCell::ShouldOffload( const CellAppInterface::ShouldOffloadArgs & args )
{

}


void FvCell::SetRetiringCell( const CellAppInterface::SetRetiringCellArgs & args )
{

}

void FvCell::AddRealEntity(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity);

	pkEntity->SetRealEntitiesIdx((int)m_kRealEntities.size());
	m_kRealEntities.push_back(pkEntity);
}

void FvCell::DelRealEntity(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity);

	int iIdx = pkEntity->GetRealEntitiesIdx();
	int iCnt = (int)m_kRealEntities.size();
	FV_ASSERT(-1 < iIdx && iIdx < iCnt);
	if(iIdx < iCnt -1)
	{
		m_kRealEntities[iIdx] = m_kRealEntities[iCnt -1];
		m_kRealEntities[iIdx]->SetRealEntitiesIdx(iIdx);
	}
	m_kRealEntities.pop_back();
	pkEntity->SetRealEntitiesIdx(-1);
}

void FvCell::EventSyncUpdate()
{
	RealEntities::size_type i(0);
	for(; i<m_kRealEntities.size(); ++i)
	{
		if(m_kRealEntities[i]->HasWitness())
			m_kRealEntities[i]->CheckEventSync();
	}
}

void FvCell::TransitionUpdate()
{
	//! 迁移检测
	RealEntities::size_type i(0);
	for(; i<m_kRealEntities.size(); ++i)
	{
		if(!m_kRealEntities[i]->CheckTransition())
			m_kRealEntities[i]->CheckGhosts();
	}
}

void FvCell::AoiAndTrapUpdate()
{
	RealEntities::size_type i(0);
	for(; i<m_kRealEntities.size(); ++i)
	{
		if(m_kRealEntities[i]->HasWitness())
			m_kRealEntities[i]->CheckAoI();
		m_kRealEntities[i]->CheckTraps();
	}
/**
	EntityMap::iterator itrB = m_kRealEntityMap.begin();
	EntityMap::iterator itrE = m_kRealEntityMap.end();
	while(itrB != itrE)
	{
		if(itrB->second->HasWitness())
			itrB->second->CheckOutOfAoi();
		++itrB;
	}

	int iInAoiCnt(0);

	//! 再检测进入AOI的情况
	FvSpace::SpaceEntityMap& kSpaceEntityMap = m_kSpace.GetSpaceEntityMap();
	FvSpace::SpaceEntityMap::iterator itrB1 = kSpaceEntityMap.begin();
	FvSpace::SpaceEntityMap::iterator itrE1 = kSpaceEntityMap.end();
	while(itrB1 != itrE1)
	{
		FvEntity* pkEntityA = itrB1->second;
		bool bAHasWitness = pkEntityA->HasWitness();
		bool bACanBeSeen = pkEntityA->CanBeSeen();

		FvSpace::SpaceEntityMap::iterator itrB2 = itrB1;
		++itrB2;

		while(itrB2 != itrE1)
		{
			FvEntity* pkEntityB = itrB2->second;
			if(bAHasWitness && pkEntityB->CanBeSeen() && pkEntityA->CheckIntoAoi(pkEntityB))
				++iInAoiCnt;
			if(pkEntityB->HasWitness() && bACanBeSeen && pkEntityB->CheckIntoAoi(pkEntityA))
				++iInAoiCnt;

			if(iInAoiCnt > 100)
				goto EXIT;

			++itrB2;
		}

		++itrB1;
	}

EXIT:
	//! 发送
	{
		EntityMap::iterator itrB = m_kRealEntityMap.begin();
		EntityMap::iterator itrE = m_kRealEntityMap.end();
		while(itrB != itrE)
		{
			if(itrB->second->HasWitness())
				itrB->second->SendToClient();
			++itrB;
		}
	}
**/
}

void FvCell::EntityUpdate()
{
	FvUInt64 nowTime = Timestamp();
	float dTime = float(nowTime-m_uiEntityTickLastTime) / StampsPerSecond();
	m_uiEntityTickLastTime = nowTime;

	RealEntities::size_type i(0);
	for(; i<m_kRealEntities.size(); ++i)
	{
		m_kRealEntities[i]->Tick(dTime);
	}
}

void FvCell::SendSpaceDataToAllClient(const FvSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kData)
{
	RealEntities::size_type i(0);
	for(; i<m_kRealEntities.size(); ++i)
	{
		m_kRealEntities[i]->SendSpaceDataToClient(kEntryID, uiKey, kData);
	}
}



