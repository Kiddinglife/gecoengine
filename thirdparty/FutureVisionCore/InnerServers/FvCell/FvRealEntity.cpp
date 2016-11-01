#include "FvRealEntity.h"
#include "FvCellEntityManager.h"
#include "FvSpace.h"
#include "FvWitness.h"
#include "FvCell.h"
#include <../FvBase/FvBaseAppIntInterface.h>



FvRealEntity::FvRealEntity( FvEntity & owner )
:m_kEntity(owner)
,m_pkWitness(NULL)
,m_pkChannel(NULL)
,m_bWitnessed(false)
,m_iNextTrapID(0)
,m_pkBoardVehicleCallBack(NULL)
{
}

FvRealEntity::~FvRealEntity()
{
#ifndef __DEV_MODE__
	FV_ASSERT(!m_pkChannel);
	FV_ASSERT(m_kTrapList.empty());
#endif

	if(m_pkWitness)
	{
		delete m_pkWitness;
		m_pkWitness = NULL;
	}

	FV_ASSERT(!m_spVehicle);
	FV_ASSERT(m_kPassengers.empty());
}

bool FvRealEntity::Init(FvBinaryIStream* pkData, CreateRealInfo createRealInfo)
{
	FV_INFO_MSG("%s, create real from %s\n", __FUNCTION__,
		createRealInfo==CREATE_REAL_FROM_INIT ? "init" : "offload");

	if(createRealInfo == CREATE_REAL_FROM_INIT)
	{
		FV_ASSERT(!pkData);
		//! 创建连接Base的Idx channel
		if(!m_kEntity.BaseAddr().IsNone())
			m_pkChannel = FvEntityManager::Instance().GetIdxChannelFinder().MakeChannel(m_kEntity.GetEntityID(), m_kEntity.BaseAddr());
	}
	else if(createRealInfo == CREATE_REAL_FROM_OFFLOAD)
	{
		FV_ASSERT(pkData && !m_pkWitness && !m_pkChannel && m_kHaunts.empty());
		FvBinaryIStream& kStream = *pkData;
		FvSpace::CellInfos& kCellInfos = m_kEntity.GetSpace()->GetCellInfos();

		FV_ASSERT(!kStream.Error());

		//! 恢复idx channel数据
		if(!m_kEntity.BaseAddr().IsNone())
			m_pkChannel = FvEntityManager::Instance().GetIdxChannelFinder().TransportIn(m_kEntity.GetEntityID(), m_kEntity.BaseAddr(), kStream);

		FV_ASSERT(!kStream.Error());

		//! 恢复Real数据
		kStream >> m_bWitnessed;
		FvUInt16 uiAoIObsMask;
		float fAoIVision;
		float fAoIDisVisibility;
		kStream >> uiAoIObsMask >> fAoIVision >> fAoIDisVisibility;
		m_kEntity.InitAoIParam(m_kEntity.GetAoIObjMask(), m_kEntity.GetAoIVisibility(), uiAoIObsMask, fAoIVision, fAoIDisVisibility);

		FV_ASSERT(!kStream.Error());

		//! 恢复载具
		FV_ASSERT(!m_spVehicle);
		FvEntityID iVehicleID;
		kStream >> iVehicleID;
		if(iVehicleID != FV_NULL_ENTITY)
		{
			m_spVehicle = FvEntityManager::Instance().FindEntity(iVehicleID);
			FV_ASSERT(m_spVehicle);
		}

		FV_ASSERT(!kStream.Error());

		//! 恢复乘客s
		{
			FV_ASSERT(m_kPassengers.empty());
			FvUInt16 uiPassengers(0);
			kStream >> uiPassengers;
			for(FvUInt16 i=0; i<uiPassengers; ++i)
			{
				FvEntityID iPassengerID;
				kStream >> iPassengerID;
				FvEntity* pkPassenger = FvEntityManager::Instance().FindEntity(iPassengerID);
				FV_ASSERT(pkPassenger);
				m_kPassengers.push_back(pkPassenger);
			}
		}

		FV_ASSERT(!kStream.Error());

		//! 恢复Trap数据
		{
			//! 写入Trap ids
			FvUInt16 uiIDs;
			kStream >> m_iNextTrapID >> uiIDs;
			FV_ASSERT(m_kFreeTrapIDList.empty());
			for(FvUInt16 i=0; i<uiIDs; ++i)
			{
				FvInt16 iID;
				kStream >> iID;
				m_kFreeTrapIDList.push_back(iID);
			}

			FV_ASSERT(!kStream.Error());

			FvUInt16 uiTraps;
			kStream >> uiTraps;
			FV_ASSERT(m_kTrapList.empty());
			for(FvUInt16 i=0; i<uiTraps; ++i)
			{
				Trap* pkTrap = new Trap(m_kEntity);
				kStream >> *pkTrap;
				FV_ASSERT(pkTrap->GetTrapID() != -1);
				m_kTrapList.push_back(pkTrap);
			}

			FV_ASSERT(!kStream.Error());
		}

		//! 恢复ghost数据
		FvUInt16 uiGhostCnt;
		kStream >> uiGhostCnt;
		for(FvUInt16 i=0; i<uiGhostCnt; ++i)
		{
			FvUInt16 uiCellIdx;
			FvNetChannel* pkChannel;
			bool bWitnessed;
			kStream >> uiCellIdx >> bWitnessed;
			//FV_ASSERT(uiCellIdx <= kCellInfos.size());
			if(uiCellIdx > kCellInfos.size())
			{
				FV_ERROR_MSG("CellIdx:%d,CellSize:%d,Err:%d\n", uiCellIdx, kCellInfos.size(), kStream.Error());
				FV_ASSERT(uiCellIdx <= kCellInfos.size());
			}
			pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kCellInfos[uiCellIdx].m_kAddr);
			m_kHaunts.push_back(Haunt(pkChannel, uiCellIdx, bWitnessed));
		}

		FV_ASSERT(!kStream.Error());

		//! 恢复witness数据
		FvUInt8 uiHasWitness;
		kStream >> uiHasWitness;
		FV_ASSERT(!kStream.Error());
		if(uiHasWitness)
		{
			m_pkWitness = new FvWitness(*this, kStream, createRealInfo);
			FV_ASSERT(m_pkChannel);
			//m_pkChannel->IsIrregular(false);
			FV_ASSERT(!kStream.Error());
		}
	}
	else
	{
		FV_ASSERT(0);
	}

	//! 通知Base端,新的Cell地址
	if(!m_kEntity.BaseAddr().IsNone())
	{
		FV_ASSERT(m_pkChannel);
		FV_ASSERT(m_kEntity.GetSpace()->SpaceID() != 0);
		FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kEntity.BaseAddr());
		pkChannel->Bundle().StartMessage(BaseAppIntInterface::CurrentCell);
		pkChannel->Bundle() << m_kEntity.GetEntityID() << m_kEntity.GetSpace()->SpaceID() << FvEntityManager::Instance().Address();
		pkChannel->Send();
	}

	return true;
}

void FvRealEntity::Destroy()
{
	CloseVehicle();
	AlightVehicle();

	if(m_pkChannel)
	{
		FvNetBundle& kBundle = m_pkChannel->Bundle();
		//! FvVector3 kPos;
		//! FvVector3 kDir;
		//! FvSpaceID iSpaceID;
		//! FvUInt16 uiSpaceType;
		//! FvTimeStamp uiGameTime;
		//! Cell部分数据
		kBundle.StartMessage(BaseAppIntInterface::CellEntityLost);
		kBundle << m_kEntity.GetPos() << m_kEntity.GetDir() << m_kEntity.GetSpace()->SpaceID() << m_kEntity.GetSpace()->SpaceType() << FvTimeStamp(0);
		m_kEntity.GetAttrib().SerializeToStreamForCellData(kBundle);
		m_pkChannel->Send();

		FvEntityManager::Instance().GetIdxChannelFinder().DeleteChannel(m_pkChannel->id());
		m_pkChannel = NULL;
	}

	{
		Haunts::iterator itrB = m_kHaunts.begin();
		Haunts::iterator itrE = m_kHaunts.end();
		while(itrB != itrE)
		{
			DeleteGhost(&(itrB->Channel()));
			FV_INFO_MSG("DestroyGhost, id:%d, cell:%d\n", m_kEntity.GetEntityID(), itrB->CellIdx());
			++itrB;
		}
		m_kHaunts.clear();
	}

	{
		int iCnt = (int)m_kTrapList.size();
		for(int i=0; i<iCnt; ++i)
		{
			delete m_kTrapList[i];
		}
		m_kTrapList.clear();
	}

	if(m_pkWitness)
		m_kEntity.CloseAoI();
}

void FvRealEntity::Offload(FvBinaryOStream& kStream, const FvNetAddress& kDstAddr, bool bTeleport)
{
	//! 写入idx channel数据
	if(m_pkChannel)
	{
		FvEntityManager::Instance().GetIdxChannelFinder().TransportOut(m_pkChannel->id(), kDstAddr, kStream);
		m_pkChannel = NULL;
	}

	//! 写入Real数据
	kStream << m_bWitnessed;
	kStream << m_kEntity.GetAoIObsMask() << m_kEntity.GetAoIVision() << m_kEntity.GetAoIDisVisibility();

	//! 写入载具
	if(m_spVehicle)
		kStream << m_spVehicle->GetEntityID();
	else
		kStream << FV_NULL_ENTITY;
	m_spVehicle = NULL;

	//! 写入乘客s
	{
		FvUInt16 uiPassengers = (FvUInt16)m_kPassengers.size();
		kStream << uiPassengers;
		for(FvUInt16 i=0; i<uiPassengers; ++i)
		{
			kStream << m_kPassengers[i]->GetEntityID();
		}
		m_kPassengers.clear();
	}

	//! 写入Trap数据
	{
		//! 写入Trap ids
		FvUInt16 uiIDs = (FvUInt16)m_kFreeTrapIDList.size();
		kStream << m_iNextTrapID << uiIDs;
		for(FvUInt16 i=0; i<uiIDs; ++i)
		{
			kStream << m_kFreeTrapIDList[i];
		}
		m_kFreeTrapIDList.clear();

		FvUInt16 uiTraps = (FvUInt16)m_kTrapList.size();
		kStream << uiTraps;
		for(FvUInt16 i=0; i<uiTraps; ++i)
		{
			kStream << *(m_kTrapList[i]);
			delete m_kTrapList[i];
		}
		m_kTrapList.clear();
	}

	//! 写入ghost数据
	if(bTeleport)//! 传送模式,没有Ghost数据
	{
		FvUInt16 uiGhostCnt = 0;
		kStream << uiGhostCnt;
	}
	else//! 迁移模式,需要保存Ghost数据
	{
		//! 目标上肯定有ghost
		bool bHasDesGhost(false);
		Haunts::iterator itrB = m_kHaunts.begin();
		Haunts::iterator itrE = m_kHaunts.end();
		while(itrB != itrE)
		{
			if(itrB->Channel().addr() == kDstAddr)
			{
				bHasDesGhost = true;
				m_kHaunts.erase(itrB);
				break;
			}
			++itrB;
		}
		FV_ASSERT(bHasDesGhost);

		FvUInt16 uiGhostCnt = m_kHaunts.size() +1;//! 把自己也当成一个ghost
		kStream << uiGhostCnt;
		FvUInt16 uiMyCellIdx = m_kEntity.GetSpace()->GetCell()->CellIdx();
		bool bIns(false);
		//kStream << m_kEntity.GetSpace()->GetCell()->CellIdx() << !m_kEntity.IsObserverListEmpty();
		itrB = m_kHaunts.begin();
		itrE = m_kHaunts.end();
		while(itrB != itrE)
		{
			if(!bIns && uiMyCellIdx < itrB->CellIdx())
			{
				kStream << uiMyCellIdx << false;
				bIns = true;
			}
			kStream << itrB->CellIdx() << itrB->IsWitnessed();
			++itrB;
		}
		if(!bIns)
		{
			kStream << uiMyCellIdx << false;
		}
	}

	//! 写入witness数据
	if(m_pkWitness)
	{
		kStream << FvUInt8(1);
		m_pkWitness->Offload(kStream);
	}
	else
	{
		kStream << FvUInt8(0);
	}
}

void FvRealEntity::EnableWitness(FvBinaryIStream& kData)
{
	FV_ASSERT(!m_pkWitness);
	m_pkWitness = new FvWitness(*this, kData, CREATE_REAL_FROM_INIT);
	FV_ASSERT(m_pkChannel);
	//m_pkChannel->IsIrregular(false);

	m_kEntity.OpenAoI();
}

void FvRealEntity::DisableWitness()
{
	FV_ASSERT(m_pkWitness);
	delete m_pkWitness;
	m_pkWitness = NULL;
	FV_ASSERT(m_pkChannel);
	//m_pkChannel->IsIrregular(true);

	m_kEntity.CloseAoI();
}

void FvRealEntity::AvatarUpdateForOtherClient(FvEntityID iEntityID, FvUInt32 uiTime, FvVector3& kPos, FvDirection3& kDir, FvUInt8 uiFlag)
{
	FV_ASSERT(m_pkChannel);

	FvNetBundle& kBundle = m_pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::DetailedPosition);
	kBundle << iEntityID << kPos << kDir;
	m_pkChannel->Send();
}

void FvRealEntity::CheckGhosts()
{
	FvSpace::CellInfo* pkCellInfo = m_kEntity.GetSpace()->GetCellInfo();
	FvSpace::CellInfos& kCellInfos = m_kEntity.GetSpace()->GetCellInfos();
	const FvVector3& kPos = m_kEntity.GetPos();
	FvUInt32 uiSize(0);
	FvUInt16* pkIDs(NULL);
	if(pkCellInfo->m_pkCellData->FindGhostGroup(kPos.x, kPos.y, uiSize, pkIDs))
	{
		if(m_kHaunts.empty())
		{
			for(FvUInt32 i(0); i<uiSize; ++i)
			{
				FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kCellInfos[pkIDs[i]].m_kAddr);
				m_kHaunts.push_back(Haunt(pkChannel, pkIDs[i], false));
				CreateGhost(pkChannel);
				FV_INFO_MSG("CreateGhost, id:%d, cell:%d\n", m_kEntity.GetEntityID(), pkIDs[i]);
			}
		}
		else
		{
			static Haunts kOldHaunts;
			kOldHaunts.clear();
			kOldHaunts.swap(m_kHaunts);
			FV_ASSERT(m_kHaunts.empty());

			FvUInt32 uiNewIdx(0);
			Haunts::iterator itrB = kOldHaunts.begin();
			Haunts::iterator itrE = kOldHaunts.end();
			while(uiNewIdx<uiSize && itrB!=itrE)
			{
				if(pkIDs[uiNewIdx] < itrB->CellIdx())
				{
					//! new
					FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kCellInfos[pkIDs[uiNewIdx]].m_kAddr);
					m_kHaunts.push_back(Haunt(pkChannel, pkIDs[uiNewIdx], false));
					CreateGhost(pkChannel);
					FV_INFO_MSG("CreateGhost, id:%d, cell:%d\n", m_kEntity.GetEntityID(), pkIDs[uiNewIdx]);
					++uiNewIdx;
				}
				else if(itrB->CellIdx() < pkIDs[uiNewIdx])
				{
					//! del
					DeleteGhost(&(itrB->Channel()));
					++itrB;
				}
				else
				{
					//! stand
					m_kHaunts.push_back(*itrB);
					++uiNewIdx;
					++itrB;
				}
			}

			while(uiNewIdx < uiSize)
			{
				//! new
				FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kCellInfos[pkIDs[uiNewIdx]].m_kAddr);
				m_kHaunts.push_back(Haunt(pkChannel, pkIDs[uiNewIdx], false));
				CreateGhost(pkChannel);
				FV_INFO_MSG("CreateGhost, id:%d, cell:%d\n", m_kEntity.GetEntityID(), pkIDs[uiNewIdx]);
				++uiNewIdx;
			}

			while(itrB != itrE)
			{
				//! del
				DeleteGhost(&(itrB->Channel()));
				++itrB;
			}
		}
	}
	else if(!m_kHaunts.empty())
	{
		Haunts::iterator itrB = m_kHaunts.begin();
		Haunts::iterator itrE = m_kHaunts.end();
		while(itrB != itrE)
		{
			DeleteGhost(&(itrB->Channel()));
			++itrB;
		}
		m_kHaunts.clear();
	}
}

void FvRealEntity::CreateGhostIfNo(FvUInt16 uiCellIdx, FvNetAddress& kAddr)
{
	Haunts::iterator itrB = m_kHaunts.begin();
	Haunts::iterator itrE = m_kHaunts.end();
	while(itrB != itrE)
	{
		if(uiCellIdx < itrB->CellIdx())
		{
			FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kAddr);
			m_kHaunts.insert(itrB, Haunt(pkChannel, uiCellIdx, false));
			CreateGhost(pkChannel);
			FV_INFO_MSG("CreateGhost, id:%d, cell:%d\n", m_kEntity.GetEntityID(), uiCellIdx);
			return;
		}
		else if(uiCellIdx == itrB->CellIdx())
		{
			return;
		}
		++itrB;
	}

	FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kAddr);
	m_kHaunts.insert(itrB, Haunt(pkChannel, uiCellIdx, false));
	CreateGhost(pkChannel);
	FV_INFO_MSG("CreateGhost, id:%d, cell:%d\n", m_kEntity.GetEntityID(), uiCellIdx);
}

void FvRealEntity::AvatarUpdateToGhosts(FvVector3& kPos, FvDirection3& kDir)
{
	Haunts::iterator itrB = m_kHaunts.begin();
	Haunts::iterator itrE = m_kHaunts.end();
	while(itrB != itrE)
	{
		FvNetChannel& kChannel = itrB->Channel();
		FvNetBundle& kBundle = kChannel.Bundle();
		kBundle.StartMessage(CellAppInterface::GhostAvatarUpdate);
		kBundle << m_kEntity.GetEntityID();
		CellAppInterface::GhostAvatarUpdateArgs* pkArgs =
			(CellAppInterface::GhostAvatarUpdateArgs*)kBundle.Reserve(sizeof(CellAppInterface::GhostAvatarUpdateArgs));
		pkArgs->pos = kPos;
		pkArgs->dir = kDir;
		pkArgs->isOnGround = true;
		pkArgs->updateNumber = 0;
		kChannel.Send();

		++itrB;
	}
}

void FvRealEntity::SendHistoryEventToGhosts(bool bPropEvent, FvUInt8 uiMessageID, FvBinaryIStream& kMsg, FvEventNumber uiEventNumber, GhostUpdateNumber iGhostUpdateNumber, FvDetailLevel uiLevel)
{
	Haunts::iterator itrB = m_kHaunts.begin();
	Haunts::iterator itrE = m_kHaunts.end();
	while(itrB != itrE)
	{
		FvNetChannel& kChannel = itrB->Channel();
		FvNetBundle& kBundle = kChannel.Bundle();
		kBundle.StartMessage(CellAppInterface::GhostHistoryEvent);
		kBundle << m_kEntity.GetEntityID();
		kBundle << bPropEvent << uiEventNumber << iGhostUpdateNumber << uiLevel << uiMessageID;
		kBundle.AddBlob(kMsg.Retrieve(0), kMsg.RemainingLength());
		kChannel.Send();

		++itrB;
	}
}

void FvRealEntity::SendGhostDataToGhosts(FvUInt8 uiMessageID, FvBinaryIStream& kMsg, GhostUpdateNumber iGhostUpdateNumber)
{
	Haunts::iterator itrB = m_kHaunts.begin();
	Haunts::iterator itrE = m_kHaunts.end();
	while(itrB != itrE)
	{
		FvNetChannel& kChannel = itrB->Channel();
		FvNetBundle& kBundle = kChannel.Bundle();
		kBundle.StartMessage(CellAppInterface::GhostedDataUpdate);
		kBundle << m_kEntity.GetEntityID();
		kBundle << iGhostUpdateNumber;
		kBundle << uiMessageID;
		kBundle.AddBlob(kMsg.Retrieve(0), kMsg.RemainingLength());
		kChannel.Send();

		++itrB;
	}
}

void FvRealEntity::SendVisibleToGhosts(FvUInt16 uiObjMask, float fVisibility)
{
	Haunts::iterator itrB = m_kHaunts.begin();
	Haunts::iterator itrE = m_kHaunts.end();
	while(itrB != itrE)
	{
		FvNetChannel& kChannel = itrB->Channel();
		CellAppInterface::VisibleUpdateArgs& kArgs = CellAppInterface::VisibleUpdateArgs::start(kChannel.Bundle(), m_kEntity.GetEntityID());
		kArgs.fVisibility = fVisibility;
		kArgs.uiObjMask = uiObjMask;
		kChannel.Send();
		++itrB;
	}
}

bool FvRealEntity::IsGhostsWitnessed()
{
	Haunts::iterator itrB = m_kHaunts.begin();
	Haunts::iterator itrE = m_kHaunts.end();
	while(itrB != itrE)
	{
		if(itrB->IsWitnessed())
			return true;

		++itrB;
	}

	return false;
}

void FvRealEntity::UpdateGhostWitnessed(FvUInt16 uiCellIdx, bool& bWitnessedOld, bool& bWitnessedNew)
{
	bWitnessedOld = bWitnessedNew = false;

	Haunts::iterator itrB = m_kHaunts.begin();
	Haunts::iterator itrE = m_kHaunts.end();
	while(itrB != itrE)
	{
		bWitnessedOld = bWitnessedOld || itrB->IsWitnessed();
		if(itrB->CellIdx() == uiCellIdx)
			itrB->ChangeWitnessed();
		bWitnessedNew = bWitnessedNew || itrB->IsWitnessed();
		++itrB;
	}
}

void FvRealEntity::SetWitnessed(bool bWitnessed)
{
	if(m_bWitnessed == bWitnessed)
		return;

	m_bWitnessed = bWitnessed;
	//m_kEntity.OnWitnessed(m_bWitnessed);
}

FvInt16 FvRealEntity::AddTrap(FvUInt16 uiMask, float fVision, float fDisVisibility, FvInt32 iUserData)
{
	if(fVision < 0.0f)
	{
		//printf("%s, %d Add Trap, Vision:%f\n", __FUNCTION__, m_kEntity.GetEntityID(), fVision);
		return -1;
	}

	FvInt16 iTrapID = GetTrapID();
	if(iTrapID == -1)
	{
		//printf("%s, GetTrapID Failed\n", __FUNCTION__);
		return iTrapID;
	}

	//printf("%s, %d Add Trap %d, Vision:%f", __FUNCTION__, m_kEntity.GetEntityID(), iTrapID, fVision);

	Trap* pkTrap = new Trap(m_kEntity);
	pkTrap->Init(iTrapID, uiMask, fVision, fDisVisibility, iUserData);
	m_kTrapList.push_back(pkTrap);

	return iTrapID;
}

void FvRealEntity::DelTrap(FvInt16 iTrapID)
{
	if(iTrapID < 0)
		return;
	int iIdx = FindTrap(iTrapID);
	if(iIdx == -1)
		return;

	//printf("%s, %d Del Trap %d", __FUNCTION__, m_kEntity.GetEntityID(), iTrapID);

	int iCnt = (int)m_kTrapList.size();
	PutTrapID(iTrapID);
	delete m_kTrapList[iIdx];

	if(iIdx < iCnt-1)
		m_kTrapList[iIdx] = m_kTrapList[iCnt-1];
	m_kTrapList.pop_back();
}

int FvRealEntity::FindTrap(FvInt16 iTrapID) const
{
	FV_ASSERT(iTrapID != -1);
	int iCnt = (int)m_kTrapList.size();
	for(int i=0; i<iCnt; ++i)
	{
		if(m_kTrapList[i]->GetTrapID() == iTrapID)
		{
			return i;
		}
	}
	return -1;
}

void FvRealEntity::SetTrapMask(FvInt16 iTrapID, FvUInt16 uiMask)
{
	FV_ASSERT(iTrapID != -1);
	int iIdx = FindTrap(iTrapID);
	FV_ASSERT(iIdx != -1);
	m_kTrapList[iIdx]->SetMask(uiMask);
}

void FvRealEntity::SetTrapVision(FvInt16 iTrapID, float fVision)
{
	FV_ASSERT(iTrapID != -1);
	int iIdx = FindTrap(iTrapID);
	FV_ASSERT(iIdx != -1);
	m_kTrapList[iIdx]->SetVision(fVision);
}

void FvRealEntity::SetTrapDisVisibility(FvInt16 iTrapID, float fDisVisibility)
{
	FV_ASSERT(iTrapID != -1);
	int iIdx = FindTrap(iTrapID);
	FV_ASSERT(iIdx != -1);
	m_kTrapList[iIdx]->SetDisVisibility(fDisVisibility);
}

void FvRealEntity::MoveTraps()
{
	if(m_kTrapList.empty())
		return;

	int iCnt = (int)m_kTrapList.size();
	for(int i=0; i<iCnt; ++i)
	{
		m_kTrapList[i]->Move();
	}
}

void FvRealEntity::CheckTraps()
{
	if(m_kTrapList.empty())
		return;

	int iCnt = (int)m_kTrapList.size();
	for(int i=0; i<iCnt; ++i)
	{
		m_kTrapList[i]->CheckTrap();
	}
}

void FvRealEntity::EntitiesInTrap(FvInt16 iTrapID, FvEntity::Entities& kEntities) const
{
	FV_ASSERT(iTrapID != -1);
	int iIdx = FindTrap(iTrapID);
	FV_ASSERT(iIdx != -1);
	m_kTrapList[iIdx]->EntitiesInTrap(kEntities);
}

void FvRealEntity::GetTraps(std::vector<float>& kTraps)
{
	int iCnt = (int)m_kTrapList.size();
	for(int i=0; i<iCnt; ++i)
	{
		kTraps.push_back(m_kTrapList[i]->GetVision());
	}
}

void FvRealEntity::DeleteAllGhosts()
{
	Haunts::iterator itrB = m_kHaunts.begin();
	Haunts::iterator itrE = m_kHaunts.end();
	while(itrB != itrE)
	{
		DeleteGhost(&(itrB->Channel()));
		FV_INFO_MSG("DestroyGhost, id:%d, cell:%d\n", m_kEntity.GetEntityID(), itrB->CellIdx());
		++itrB;
	}
	m_kHaunts.clear();
}

void FvRealEntity::CreateGhostData(FvBinaryOStream& stream)
{
	//! FvSpaceID iSpaceID;
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
	stream << m_kEntity.GetSpace()->SpaceID()
		<< m_kEntity.GetPos() << m_kEntity.GetDir() << FvTimeStamp(0) << m_kEntity.BaseAddr() << FvEntityManager::Instance().Address()
		<< m_kEntity.GetEntityTypeID() << m_kEntity.GetEntityID() << m_kEntity.GetDBID() << m_kEntity.GetAoIObjMask() << m_kEntity.GetAoIVisibility() << m_kEntity.LastEventNumber() << m_kEntity.LastVolatileUpdateNumber() << m_kEntity.LastGhostUpdateNumber();
	m_kEntity.GetAttrib().SerializeToStreamForGhostData(stream);
}

void FvRealEntity::CloseTrapsBeforeTeleportLocally()
{
	int iCnt = (int)m_kTrapList.size();
	for(int i=0; i<iCnt; ++i)
	{
		m_kTrapList[i]->CloseTrapAndInform();
	}
}

void FvRealEntity::OpenTrapsAfterTeleportLocally()
{
	int iCnt = (int)m_kTrapList.size();
	for(int i=0; i<iCnt; ++i)
	{
		m_kTrapList[i]->OpenTrap();
	}
}

void FvRealEntity::CreateGhost(FvNetChannel* pkChannel)
{
	FV_ASSERT(pkChannel);
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(CellAppInterface::CreateGhost);
	CreateGhostData(kBundle);
	pkChannel->DelayedSend();
}

void FvRealEntity::DeleteGhost(FvNetChannel* pkChannel)
{
	FV_ASSERT(pkChannel);
	pkChannel->Bundle().StartMessage(CellAppInterface::DelGhost);
	pkChannel->Bundle() << m_kEntity.GetEntityID() << FvUInt8(0);
	pkChannel->DelayedSend();
}

FvInt16 FvRealEntity::GetTrapID()
{
	FvInt16 iTrapID(-1);
	if(m_kFreeTrapIDList.empty())
	{
		if(m_iNextTrapID < 0x7FFF)
		{
			iTrapID = m_iNextTrapID;
			++m_iNextTrapID;
		}
	}
	else
	{
		iTrapID = m_kFreeTrapIDList.back();
		m_kFreeTrapIDList.pop_back();
	}

	return iTrapID;
}

void FvRealEntity::PutTrapID(FvInt16 iTrapID)
{
	m_kFreeTrapIDList.push_back(iTrapID);
}

FvRealEntity::Trap::~Trap()
{
	if(m_hTrapObserver != FVAOI_NULL_HANDLE)
	{
		m_kEntity.GetSpace()->GetAoIMgr().Remove(m_hTrapObserver);
		m_hTrapObserver = FVAOI_NULL_HANDLE;
	}
}

bool FvRealEntity::Trap::Init(FvInt16 iTrapID, FvUInt16 uiMask, float fVision, float fDisVisibility, FvInt32 iUserData)
{
	if(iTrapID==-1 || fVision<0.0f)
		return false;

	m_iTrapID = iTrapID;
	m_iUserData = iUserData;
	m_uiMask = uiMask;
	m_fVision = fVision;
	m_fDisVisibility = fDisVisibility;
	const FvVector3& kPos = m_kEntity.GetPos();
	m_hTrapObserver = m_kEntity.GetSpace()->GetAoIMgr().AddObserver(m_kEntity.GetAoIObjHandle(),
		uiMask, kPos.x, kPos.y, fVision, fDisVisibility, false, &m_kEntity);
	return true;
}

void FvRealEntity::Trap::Move()
{
	const FvVector3& kPos = m_kEntity.GetPos();
	m_kEntity.GetSpace()->GetAoIMgr().Move(m_hTrapObserver, kPos.x, kPos.y);
}

void FvRealEntity::Trap::CheckTrap()
{
	FV_ASSERT(m_hTrapObserver != FVAOI_NULL_HANDLE);

	class Listener
	{
	public:
		Listener(FvEntity& kEntity,	FvInt16 iTrapID, FvInt32 iUserData):m_kEntity(kEntity),m_iTrapID(iTrapID),m_iUserData(iUserData) {}
		FvEntity&	m_kEntity;
		FvInt16		m_iTrapID;
		FvInt32		m_iUserData;

		void OnEnter(void* pkObsData, FvAoIExt* pkExt)
		{
			TrapCache* pkCache = (TrapCache*)pkExt;
			m_kEntity.OnEnterTrap(*(pkCache->m_spEntity.Get()), m_iTrapID, m_iUserData);
			//printf("%s, %d Into %d's Trap %d\n", __FUNCTION__, pkCache->m_spEntity->GetEntityID(), m_kEntity.GetEntityID(), m_iTrapID);
		}

		void OnLeave(void* pkObsData, FvAoIExt* pkExt)
		{
			TrapCache* pkCache = (TrapCache*)pkExt;
			m_kEntity.OnLeaveTrap(*(pkCache->m_spEntity.Get()), m_iTrapID, m_iUserData);
			//printf("%s, %d Leave %d's Trap %d\n", __FUNCTION__, pkCache->m_spEntity->GetEntityID(), m_kEntity.GetEntityID(), m_iTrapID);
		}
	};
	Listener kListener(m_kEntity, m_iTrapID, m_iUserData);

#ifndef FV_DEBUG
	m_kEntity.GetSpace()->GetAoIMgr().UpdateTrap<Listener, &Listener::OnEnter, &Listener::OnLeave>(m_hTrapObserver, &kListener);
#else
	FvUInt64 a,b,c;
	m_kEntity.GetSpace()->GetAoIMgr().UpdateTrap<Listener, &Listener::OnEnter, &Listener::OnLeave>(m_hTrapObserver, &kListener, a, b, c);
#endif
}

void FvRealEntity::Trap::EntitiesInTrap(FvEntity::Entities& kEntities) const
{
	kEntities.clear();
	FV_ASSERT(m_hTrapObserver != FVAOI_NULL_HANDLE);

	class Visiter
	{
	public:
		Visiter(FvEntity::Entities& kEntities):m_kEntities(kEntities) {}
		FvEntity::Entities& m_kEntities;

		void OnVisit(void* pkObjData)
		{
			FV_ASSERT(pkObjData);
			m_kEntities.push_back((FvEntity*)pkObjData);
		}
	};
	Visiter kVisiter(kEntities);

	m_kEntity.GetSpace()->GetAoIMgr().QueryVision<Visiter, &Visiter::OnVisit>(m_hTrapObserver, &kVisiter);
}

void FvRealEntity::Trap::OpenTrap()
{
	FV_ASSERT(m_hTrapObserver == FVAOI_NULL_HANDLE);
	const FvVector3& kPos = m_kEntity.GetPos();
	m_hTrapObserver = m_kEntity.GetSpace()->GetAoIMgr().AddObserver(m_kEntity.GetAoIObjHandle(),
		m_uiMask, kPos.x, kPos.y, m_fVision, m_fDisVisibility, false, &m_kEntity);
}

void FvRealEntity::Trap::CloseTrapAndInform()
{
	FV_ASSERT(m_hTrapObserver != FVAOI_NULL_HANDLE);

	class Visiter
	{
	public:
		Visiter(FvEntity& kEntity,	FvInt16 iTrapID, FvInt32 iUserData):m_kEntity(kEntity),m_iTrapID(iTrapID),m_iUserData(iUserData) {}
		FvEntity&	m_kEntity;
		FvInt16		m_iTrapID;
		FvInt32		m_iUserData;

		void OnVisit(void* pkObjData)
		{
			FV_ASSERT(pkObjData);
			m_kEntity.OnLeaveTrap(*(FvEntity*)pkObjData, m_iTrapID, m_iUserData);
		}
	};
	Visiter kVisiter(m_kEntity, m_iTrapID, m_iUserData);

	m_kEntity.GetSpace()->GetAoIMgr().QueryVision<Visiter, &Visiter::OnVisit>(m_hTrapObserver, &kVisiter);
	m_kEntity.GetSpace()->GetAoIMgr().Remove(m_hTrapObserver);
	m_hTrapObserver = FVAOI_NULL_HANDLE;
}

void FvRealEntity::Trap::SetMask(FvUInt16 uiMask)
{
	FV_ASSERT(m_hTrapObserver != FVAOI_NULL_HANDLE);
	if(uiMask != m_uiMask)
	{
		m_uiMask = uiMask;
		m_kEntity.GetSpace()->GetAoIMgr().SetMask(m_hTrapObserver, uiMask);
	}
}

void FvRealEntity::Trap::SetVision(float fVision)
{
	FV_ASSERT(m_hTrapObserver != FVAOI_NULL_HANDLE);
	if(fVision != m_fVision)
	{
		m_fVision = fVision;
		m_kEntity.GetSpace()->GetAoIMgr().SetVision(m_hTrapObserver, fVision);
	}
}

void FvRealEntity::Trap::SetDisVisibility(float fDisVisibility)
{
	FV_ASSERT(m_hTrapObserver != FVAOI_NULL_HANDLE);
	if(fDisVisibility != m_fDisVisibility)
	{
		m_fDisVisibility = fDisVisibility;
		m_kEntity.GetSpace()->GetAoIMgr().SetDisVisibility(m_hTrapObserver, fDisVisibility);
	}
}

template<>
FvBinaryIStream & operator>>( FvBinaryIStream & stream,
							 FvRealEntity::Trap & trap )
{
	FvUInt16 uiEntityCnt;
	stream >> trap.m_iTrapID >> trap.m_iUserData >> trap.m_uiMask >> trap.m_fVision >> trap.m_fDisVisibility >> uiEntityCnt;

	typedef std::vector<FvEntity*> Entities;
	static Entities s_kOldList;

	s_kOldList.clear();
	FvEntity::AddGlobalDummy();

	//! TODO: 放到AoIMgr里面处理,将网络传过来的id列表转换为AoIMgr里的Relate链表(可以保留找不到实体的id),在下一次更新时处理进出消息

	for(FvUInt16 i=0; i<uiEntityCnt; ++i)
	{
		FvEntityID iEntityID;
		stream >> iEntityID;

		FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(iEntityID);
		if(pkEntity)
		{
			if(pkEntity->GetSpaceID() == trap.m_kEntity.GetSpaceID())
			{
				pkEntity->SetDummy(FvEntity::GetGlobalDummy());
				s_kOldList.push_back(pkEntity);
			}
			else
			{
				trap.m_kEntity.OnLeaveTrap(*pkEntity, trap.m_iTrapID, trap.m_iUserData);
			}
		}
		else
		{
			//! TODO: OnEnteredCell()前做这个处理,会有隐患的吧?
			trap.m_kEntity.OnLeaveTrapID(iEntityID, trap.m_iTrapID, trap.m_iUserData);
		}
	}

	FV_ASSERT(trap.m_hTrapObserver == FVAOI_NULL_HANDLE);
	const FvVector3& kPos = trap.m_kEntity.GetPos();
	trap.m_hTrapObserver = trap.m_kEntity.GetSpace()->GetAoIMgr().AddObserver(trap.m_kEntity.GetAoIObjHandle(),
		trap.m_uiMask, kPos.x, kPos.y, trap.m_fVision, trap.m_fDisVisibility, false, &trap.m_kEntity);

	class Listener
	{
	public:
		Listener(FvEntity& kEntity,	FvInt16 iTrapID, FvInt32 iUserData, FvUInt64 uiDummy):m_kEntity(kEntity),m_iTrapID(iTrapID),m_iUserData(iUserData),m_uiDummy(uiDummy) {}
		FvEntity&	m_kEntity;
		FvInt16		m_iTrapID;
		FvInt32		m_iUserData;
		FvUInt64	m_uiDummy;

		void OnEnter(void* pkObsData, FvAoIExt* pkExt)
		{
			TrapCache* pkCache = (TrapCache*)pkExt;
			FvEntity* pkEntity = pkCache->m_spEntity.Get();
			if(pkEntity->GetDummy() != m_uiDummy)
				m_kEntity.OnEnterTrap(*pkEntity, m_iTrapID, m_iUserData);
			pkEntity->SetDummy(m_uiDummy +1);
		}

		void OnLeave(void* pkObsData, FvAoIExt* pkExt)
		{
			FV_ASSERT(0);
		}
	};
	Listener kListener(trap.m_kEntity, trap.m_iTrapID, trap.m_iUserData, FvEntity::GetGlobalDummy());

#ifndef FV_DEBUG
	trap.m_kEntity.GetSpace()->GetAoIMgr().UpdateTrap<Listener, &Listener::OnEnter, &Listener::OnLeave>(trap.m_hTrapObserver, &kListener);
#else
	FvUInt64 a,b,c;
	trap.m_kEntity.GetSpace()->GetAoIMgr().UpdateTrap<Listener, &Listener::OnEnter, &Listener::OnLeave>(trap.m_hTrapObserver, &kListener, a, b, c);
#endif

	FvEntity::AddGlobalDummy();
	for(int i=0; i<(int)s_kOldList.size(); ++i)
	{
		if(s_kOldList[i]->GetDummy() != FvEntity::GetGlobalDummy())
		{
			//! TODO: OnEnteredCell()前做这个处理,会有隐患的吧?
			trap.m_kEntity.OnLeaveTrap(*s_kOldList[i], trap.m_iTrapID, trap.m_iUserData);
		}
	}

	return stream;
}

template<>
FvBinaryOStream & operator<<( FvBinaryOStream & stream,
							 const FvRealEntity::Trap & trap )
{
	stream << trap.m_iTrapID << trap.m_iUserData << trap.m_uiMask << trap.m_fVision << trap.m_fDisVisibility;
	int iEntityCntPos = stream.Size();
	stream.Reserve(sizeof(FvUInt16));

	class Visiter
	{
	public:
		Visiter(FvBinaryOStream& kStream):m_kStream(kStream),m_uiCnt(0) {}
		FvBinaryOStream& m_kStream;
		FvUInt16 m_uiCnt;

		void OnVisit(void* pkObjData)
		{
			FV_ASSERT(pkObjData);
			FvEntity* pkEntity = (FvEntity*)pkObjData;
			if(!pkEntity->IsDestroy())
			{
				m_kStream << pkEntity->GetEntityID();
				++m_uiCnt;
			}
		}
	};
	Visiter kVisiter(stream);

	trap.m_kEntity.GetSpace()->GetAoIMgr().QueryVision<Visiter, &Visiter::OnVisit>(trap.m_hTrapObserver, &kVisiter);
	trap.m_kEntity.GetSpace()->GetAoIMgr().Remove(trap.m_hTrapObserver);
	const_cast<FvRealEntity::Trap*>(&trap)->m_hTrapObserver = FVAOI_NULL_HANDLE;

	//! stream必须为FvMemoryOStream
	*(FvUInt16*)(((char*)((FvMemoryOStream&)stream).Data()) + iEntityCntPos) = kVisiter.m_uiCnt;
	return stream;
}

bool FvRealEntity::BoardVehicle(FvEntityPtr spVehicle, BoardVehicleCallBack* pkCallBack)
{
	if(m_spVehicle)
		return false;

	if(!spVehicle || spVehicle->IsDestroy() || !pkCallBack)
		return false;

	if(spVehicle == &m_kEntity)
		return false;

	FV_ASSERT(!m_pkBoardVehicleCallBack);
	m_pkBoardVehicleCallBack = pkCallBack;
	m_spVehicle = spVehicle;
	m_spVehicle->BoardVehicleReq(m_kEntity.GetEntityID(), FvEntityManager::Instance().Nub().Address());
	return true;
}

void FvRealEntity::AlightVehicle()
{
	if(!m_spVehicle)
		return;

	FV_ASSERT(!m_spVehicle->IsDestroy());
	m_spVehicle->AlightVehicleReq(m_kEntity.GetEntityID());
	m_spVehicle = NULL;

	SyncVehicleIDToRealAndGhosts(FV_NULL_ENTITY);
}

void FvRealEntity::MoveVehicle(FvMemoryOStream& kData)
{
	if(m_kPassengers.empty())
		return;

	Passengers::iterator itrB = m_kPassengers.begin();
	Passengers::iterator itrE = m_kPassengers.end();
	while(itrB != itrE)
	{
		FV_ASSERT(!(*itrB)->IsDestroy());
		(*itrB)->MoveVehicleReq(m_kEntity.GetEntityID(), kData);
		++itrB;
	}
}

void FvRealEntity::CloseVehicle()
{
	if(m_kPassengers.empty())
		return;

	Passengers::iterator itrB = m_kPassengers.begin();
	Passengers::iterator itrE = m_kPassengers.end();
	while(itrB != itrE)
	{
		FV_ASSERT(!(*itrB)->IsDestroy());
		(*itrB)->CloseVehicleReq(m_kEntity.GetEntityID());
		++itrB;
	}

	m_kPassengers.clear();
}

void FvRealEntity::BoardVehicleReq(FvEntityID iPassengerID, const FvNetAddress& kAddr)
{
	FV_ASSERT(!kAddr.IsNone());
	FvEntity* pkPassenger = FvEntityManager::Instance().FindEntity(iPassengerID);
	if(pkPassenger && m_kEntity.OnBoardVehicle(pkPassenger))
	{
		m_kPassengers.push_back(pkPassenger);
		pkPassenger->BoardVehicleRep(true, m_kEntity.GetEntityID());
		return;
	}

	FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kAddr);
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(CellAppInterface::VehicleMsg);
	kBundle << iPassengerID;
	kBundle << FvUInt8(VEHICLE_MSGID_BOARD_REP);
	kBundle << bool(false) << m_kEntity.GetEntityID();
	pkChannel->DelayedSend();
}
void FvRealEntity::BoardVehicleRep(bool bSuccess, FvEntityID iVehicleID)
{
	FV_ASSERT(m_spVehicle && !m_spVehicle->IsDestroy() && m_pkBoardVehicleCallBack);
	FV_ASSERT(m_spVehicle->GetEntityID() == iVehicleID);

	if(bSuccess)
	{
		SyncVehicleIDToRealAndGhosts(iVehicleID);
	}
	else
	{
		m_spVehicle = NULL;
	}

	m_pkBoardVehicleCallBack->HandleCallBack(bSuccess);
	m_pkBoardVehicleCallBack = NULL;
}
void FvRealEntity::AlightVehicleReq(FvEntityID iPassengerID)
{
	if(m_kPassengers.empty())
		return;

	Passengers::iterator itrB = m_kPassengers.begin();
	Passengers::iterator itrE = m_kPassengers.end();
	while(itrB != itrE)
	{
		FV_ASSERT(!(*itrB)->IsDestroy());
		if((*itrB)->GetEntityID() == iPassengerID)
		{
			m_kEntity.OnAlightVehicle((*itrB));
			if(m_kPassengers.size() > 1)
				*itrB = m_kPassengers.back();
			m_kPassengers.pop_back();
			break;
		}
		++itrB;
	}
}
void FvRealEntity::MoveVehicleReq(FvEntityID iVehicleID, FvBinaryIStream& kData)
{
	FV_ASSERT(m_spVehicle && !m_spVehicle->IsDestroy());
	FV_ASSERT(m_spVehicle->GetEntityID() == iVehicleID);

	m_kEntity.OnMoveVehicle(kData);
}
void FvRealEntity::CloseVehicleReq(FvEntityID iVehicleID)
{
	FV_ASSERT(m_spVehicle && !m_spVehicle->IsDestroy());
	FV_ASSERT(m_spVehicle->GetEntityID() == iVehicleID);

	SyncVehicleIDToRealAndGhosts(FV_NULL_ENTITY);

	m_kEntity.OnCloseVehicle();
	m_spVehicle = NULL;
}

void FvRealEntity::SyncVehicleIDToRealAndGhosts(FvEntityID iVehicleID)
{
	m_kEntity.SetVehicleID(iVehicleID);
	m_kEntity.SyncVehicleToObservers();

	//! 同步给Ghost
	Haunts::iterator itrB = m_kHaunts.begin();
	Haunts::iterator itrE = m_kHaunts.end();
	while(itrB != itrE)
	{
		//! FvEntityID iVehicleID

		FvNetChannel& kChannel = itrB->Channel();
		FvNetBundle& kBundle = kChannel.Bundle();
		kBundle.StartMessage(CellAppInterface::VehicleMsg);
		kBundle << m_kEntity.GetEntityID();
		kBundle << FvUInt8(VEHICLE_MSGID_SETID);
		kBundle << iVehicleID;
		kChannel.Send();

		++itrB;
	}
}