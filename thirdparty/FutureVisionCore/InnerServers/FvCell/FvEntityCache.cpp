/**
#include "FvEntityCache.h"
#include "FvCellEntity.h"
#include "FvCellEntityManager.h"
#include <../FvBase/FvBaseAppIntInterface.h>
#include "FvSpace.h"


FvEntityID	FvEntityCache::ms_iLostEntityID = 0;
FvSpace*	FvEntityCache::ms_pkSpace = NULL;

FvEntityCache::FvEntityCache()
:m_uiLastEventNumber(0)
,m_uiLastVolatileUpdateNumber(0)
,m_fPriority(0)
,m_uiDetailLevel(0)
{
	m_aiLodEventNumbers[0] = 0;
}

FvEntityCache::~FvEntityCache()
{

}

bool FvEntityCache::Init(FvEntity* pkEntity, FvPriority fAoiPriority, FvNetChannel* pkChannel)
{
	FV_ASSERT(pkEntity);
	m_spEntity = pkEntity;

	m_uiLastEventNumber = pkEntity->LastEventNumber();
	m_uiLastVolatileUpdateNumber = pkEntity->LastVolatileUpdateNumber();
	m_uiDetailLevel = pkEntity->GetEntityDes()->LodLevels().Size();
	FV_ASSERT( m_uiDetailLevel <= MAX_LOD_LEVELS );


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
	SendEnterAoi(pkChannel);

	return true;
}

bool FvEntityCache::UpdatePriorityAndLoD(FvEntity* pkEntity, const FvVector3& kPos, FvPriority fAoiPriority, FvNetChannel* pkChannel)
{
	//! 已销毁
	if(m_spEntity->IsDestroy() ||
		m_spEntity->GetSpaceID() != pkEntity->GetSpaceID() ||//! 不在同一个Space
		!m_spEntity->CanBeSeen())//! 是否可见
	{
		//! 发送离开Aoi包
		pkEntity->OnLeaveAoi(GetEntity());
		SendLeaveAoi(pkChannel);
		return false;
	}


	//! 检测是否出Aoi
	const FvVector3& kMyPos = m_spEntity->GetPos();
	float diffX = kMyPos.x - kPos.x;
	float diffY = kMyPos.y - kPos.y;
	FvPriority distSQ = diffX * diffX + diffY * diffY;
	if(distSQ > fAoiPriority)
	{
		//! 发送离开Aoi包
		pkEntity->OnLeaveAoi(GetEntity());
		SendLeaveAoi(pkChannel);
		return false;
	}

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
		SendUpdateLod(m_uiDetailLevel-1, uiNewDetailLevel, pkChannel);

	m_uiDetailLevel = uiNewDetailLevel;
	m_fPriority = distSQ;

	return true;
}

void FvEntityCache::SendEnterAoi(FvNetChannel* pkChannel)
{
	FV_ASSERT(pkChannel);

	//! 直接发送创建实体
	FvNetBundle& kBundle = pkChannel->Bundle();

	int s1 = kBundle.Size();
	int p1 = kBundle.SizeInPackets();
	int n1 = kBundle.NumMessages();

	kBundle.StartMessage(BaseAppIntInterface::CreateEntity);
	kBundle << m_spEntity->GetEntityID() << m_spEntity->GetEntityTypeID() << m_spEntity->GetPos() << m_spEntity->GetDir();

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

void FvEntityCache::SendLeaveAoi(FvNetChannel* pkChannel)
{
	FV_ASSERT(pkChannel);

	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::LeaveAoI);
	kBundle << m_spEntity->GetEntityID();
}

void FvEntityCache::SendLeaveAoi(FvNetChannel* pkChannel, FvEntityID iEntityID)
{
	FV_ASSERT(pkChannel);

	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::LeaveAoI);
	kBundle << iEntityID;
}

void FvEntityCache::SendUpdateLod(FvDetailLevel uiFrom, FvDetailLevel uiTo, FvNetChannel* pkChannel)
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

void FvEntityCache::SendEventUpdate(FvNetChannel* pkChannel)
{
	FV_ASSERT(pkChannel);

	FvEventHistory& kEventHistory = m_spEntity->GetEventHistory();
	if(kEventHistory.empty())
		return;

	FvEventNumber m_uiNeedEventNumber = m_uiLastEventNumber +1;
	FvEventHistory::const_iterator itrB = kEventHistory.begin();
	FvEventHistory::const_iterator itrE = kEventHistory.end();

	//! 需要整体更新
	if(EventNumberLessThan(m_uiNeedEventNumber, (*itrB)->Number()))
	{
		SendUpdateLod(m_spEntity->GetEntityDes()->LodLevels().Size()-1, m_uiDetailLevel, pkChannel);
		return;
	}

	FvNetBundle& kBundle = pkChannel->Bundle();

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

bool FvEntityCache::EventNumberLessThan(FvEventNumber a, FvEventNumber b)
{
	return (( a - b ) & 0xFFFFFFFF) > 0x80000000;
}

FvBinaryIStream & operator>>( FvBinaryIStream & stream,
									FvEntityCache & entityCache )
{
	FvEntityID iEntityID;
	FvUInt8 uiSeenLods;
	stream >> iEntityID
		>> entityCache.m_uiLastEventNumber
		>> entityCache.m_uiLastVolatileUpdateNumber
		>> entityCache.m_fPriority
		>> entityCache.m_uiDetailLevel
		>> uiSeenLods;
	for(FvUInt8 i=0; i<uiSeenLods; ++i)
	{
		stream >> entityCache.m_aiLodEventNumbers[entityCache.m_uiDetailLevel + i];
	}

	FV_ASSERT(FvEntityCache::ms_pkSpace);
	entityCache.m_spEntity = FvEntityCache::ms_pkSpace->FindEntity(iEntityID);
	if(!entityCache.m_spEntity)
		FvEntityCache::ms_iLostEntityID = iEntityID;

	return stream;
}

FvBinaryOStream & operator<<( FvBinaryOStream & stream,
									const FvEntityCache & entityCache )
{
	FV_ASSERT(!entityCache.m_spEntity->IsDestroy());
	FV_ASSERT(entityCache.m_spEntity->GetEntityDes()->LodLevels().Size() > entityCache.m_uiDetailLevel);
	FvUInt8 uiSeenLods = entityCache.m_spEntity->GetEntityDes()->LodLevels().Size() - entityCache.m_uiDetailLevel;
	stream << entityCache.m_spEntity->GetEntityID()
		<< entityCache.m_uiLastEventNumber
		<< entityCache.m_uiLastVolatileUpdateNumber
		<< entityCache.m_fPriority
		<< entityCache.m_uiDetailLevel
		<< uiSeenLods;
	for(FvUInt8 i=0; i<uiSeenLods; ++i)
	{
		stream << entityCache.m_aiLodEventNumbers[entityCache.m_uiDetailLevel + i];
	}

	return stream;
}

**/