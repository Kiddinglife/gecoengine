#include "FvCellEntity.h"
#include "FvCellEntityManager.h"
#include "FvRealEntity.h"
#include "FvWitness.h"
#include "FvSpace.h"
#include "FvCell.h"
#include <../FvBase/FvBaseAppIntInterface.h>
#include <../FvCellAppManager/FvCellAppManagerInterface.h>
#include "FvSpaceDataTypes.h"
#include <FvEntityExport.h>
#include <FvEntityDefUtility.h>
#include "FvControllerImplement.h"
#include "FvCellRPCCallBack.h"

FvMailBox FvEntity::ms_kBaseMB;
FvClientsMailBox FvEntity::ms_kAllClients;
FvClientsMailBox FvEntity::ms_kOtherClients;
FvUInt64 FvEntity::m_uiGlobalDummy = 0;

FV_INLINE bool DirIsEqual(const FvDirection3& dir1, const FvDirection3& dir2)
{
	return dir1.Yaw()==dir2.Yaw() && dir1.Pitch()==dir2.Pitch() && dir1.Roll()==dir2.Roll();
}


FvEntity::FvEntity()
:m_iEntityID(0)
,m_iDBID(0)
,m_kPos(FvVector3::ZERO)
,m_kDir()
,m_pkSpace(NULL)
,m_kBaseAddr(FvNetAddress::NONE)
,m_pkRealChannel(NULL)
,m_pkReal(NULL)
,m_uiLastEventNumber(0)
,m_uiLastVolatileUpdateNumber(0)
,m_uiLastGhostUpdateNumber(0)
,m_pkBaseMB(NULL)
,m_pkAllClients(NULL)
,m_pkOtherClients(NULL)
#pragma warning (push)
#pragma warning (disable: 4355)
,m_kAttribEventCallBack(this)
#pragma warning (pop)
,m_bWaitForcePosAck(false)
,m_iEntitiesIdx(-1)
,m_iRealEntitiesIdx(-1)
,m_hAoIObject(FVAOI_NULL_HANDLE)
,m_hAoIObserver(FVAOI_NULL_HANDLE)
,m_uiAoIObjMask(0)
,m_uiAoIObsMask(0)
,m_fAoIVisibility(80.0f)
,m_fAoIVision(80.0f)
,m_fAoIDisVisibility(0.0f)
,m_uiDummy(0)
,m_pkVehicle(NULL)
,m_uiDestroyEntityIdx(0xFFFFFFFF)
,m_uiRPCCallBackID(0)
{
}

FvEntity::~FvEntity()
{
	{
		GhostEventCacheList::iterator itrB = m_kGhostEventCacheList.begin();
		GhostEventCacheList::iterator itrE = m_kGhostEventCacheList.end();
		while(itrB != itrE)
		{
			GhostEventCache* pkNow = *itrB;
			if(pkNow->pkEvent)
			{
				delete pkNow->pkEvent;
				pkNow->pkEvent = NULL;
			}
			delete pkNow;
			++itrB;
		}
		m_kGhostEventCacheList.clear();
	}

	if(m_pkReal)
	{
		delete m_pkReal;
		m_pkReal = NULL;
	}

	m_kAttrib.DestroyRealData();

	if(m_pkBaseMB)
	{
		delete m_pkBaseMB;
		m_pkBaseMB = NULL;
	}

	if(m_pkAllClients)
	{
		delete m_pkAllClients;
		m_pkAllClients = NULL;
	}

	if(m_pkOtherClients)
	{
		delete m_pkOtherClients;
		m_pkOtherClients = NULL;
	}
}

bool FvEntity::Init(FvEntityID iEntityID, FvDatabaseID iDBID, FvCellEntityExport* pkExport,
					const FvVector3& kPos, const FvDirection3& kDir, FvSpace* pkSpace, const FvNetAddress& kBaseAddr)
{
	m_iEntityID = iEntityID;
	m_iDBID = iDBID;
	m_kPos = kPos;
	m_kDir = kDir;
	m_pkSpace = pkSpace;
	m_kBaseAddr = kBaseAddr;
	m_kAttrib.Init(m_iEntityID, this, pkExport);
	if(!m_kBaseAddr.IsNone())
	{
		m_pkBaseMB = new FvMailBox();
		m_pkBaseMB->Set(m_iEntityID, pkExport->uiTypeID, m_kBaseAddr, FvEntityMailBoxRef::BASE);
	}
	if(pkExport->kForm.bHasClient)
	{
		m_pkAllClients = new FvClientsMailBox(this, true);
		m_pkOtherClients = new FvClientsMailBox(this, false);
	}

	return true;
}

FvEntityTypeID FvEntity::GetEntityTypeID() const
{
	return m_kAttrib.GetExport()->uiTypeID;
}

void FvEntity::SetPosAndDir(const FvVector3& pos, const FvDirection3& dir)
{
	bool bChg(false);

	if(m_kPos != pos)
	{
		bChg = true;
		m_kPos = pos;

		m_pkSpace->GetAoIMgr().Move(m_hAoIObject, m_kPos.x, m_kPos.y);
		if(m_hAoIObserver)
			m_pkSpace->GetAoIMgr().Move(m_hAoIObserver, m_kPos.x, m_kPos.y);
		if(m_pkReal)
			m_pkReal->MoveTraps();
	}

	if(!DirIsEqual(m_kDir, dir))
	{
		bChg = true;
		m_kDir = dir;
	}

	if(bChg)
		SyncPosDir();
}

void FvEntity::SetDir(const FvDirection3& dir)
{
	bool bChg(false);

	if(!DirIsEqual(m_kDir, dir))
	{
		bChg = true;
		m_kDir = dir;
	}

	if(bChg)
		SyncPosDir();
}

void FvEntity::SetPos(const FvVector3& pos)
{
	bool bChg(false);

	if(m_kPos != pos)
	{
		bChg = true;
		m_kPos = pos;

		m_pkSpace->GetAoIMgr().Move(m_hAoIObject, m_kPos.x, m_kPos.y);
		if(m_hAoIObserver)
			m_pkSpace->GetAoIMgr().Move(m_hAoIObserver, m_kPos.x, m_kPos.y);
		if(m_pkReal)
			m_pkReal->MoveTraps();
	}

	if(bChg)
		SyncPosDir();
}

bool FvEntity::Dump(const char* fileName)
{
	return true;
	//if(!fileName)
	//	return false;

	//if(!m_pkEntityDes || !m_pkDic)
	//	return false;

	//FvMemoryOStream stream;

	////! entity>>stream
	//if(!m_pkEntityDes->AddDictionaryToStream(m_pkDic, stream, FvEntityDescription::FV_CELL_DATA))
	//	return false;

	////! stream>>section
	//FvXMLSectionPtr spSection = FvXMLSection::CreateSection("root");
	//if(!m_pkEntityDes->ReadStreamToSection(stream, FvEntityDescription::FV_CELL_DATA, spSection))
	//	return false;

	//return spSection->Save(fileName);
}

void FvEntity::Dump(FvXMLSectionPtr pSection, FvInt32 dataDomains)
{
	//FvMemoryOStream stream;
	//bool bRet(false);
	//bRet = m_pkEntityDes->AddDictionaryToStream(m_pkDic, stream, dataDomains);
	//FV_ASSERT(bRet);
	//bRet = m_pkEntityDes->ReadStreamToSection(stream, dataDomains, pSection);
	//FV_ASSERT(bRet);
	//FV_ASSERT(stream.RemainingLength() == 0);
}

void FvEntity::SetAoIObjMask(FvUInt16 uiMask)
{
	if(!IsReal())
		return;
	if(m_hAoIObject != FVAOI_NULL_HANDLE)
	{
		if(m_uiAoIObjMask != uiMask)
		{
			m_uiAoIObjMask = uiMask;
			m_pkSpace->GetAoIMgr().SetMask(m_hAoIObject, m_uiAoIObjMask);
			m_pkReal->SendVisibleToGhosts(m_uiAoIObjMask, m_fAoIVisibility);
		}
	}
	else
	{
		m_uiAoIObjMask = uiMask;
	}
}

void FvEntity::SetAoIObsMask(FvUInt16 uiMask)
{
	if(!IsReal())
		return;
	if(m_hAoIObserver != FVAOI_NULL_HANDLE)
	{
		if(m_uiAoIObsMask != uiMask)
		{
			m_uiAoIObsMask = uiMask;
			m_pkSpace->GetAoIMgr().SetMask(m_hAoIObserver, m_uiAoIObsMask);
		}
	}
	else
	{
		m_uiAoIObsMask = uiMask;
	}
}

void FvEntity::SetAoIVisibility(float fVisibility)
{
	if(!IsReal())
		return;
	if(m_hAoIObject != FVAOI_NULL_HANDLE)
	{
		if(m_fAoIVisibility != fVisibility)
		{
			m_fAoIVisibility = fVisibility;
			m_pkSpace->GetAoIMgr().SetVisibility(m_hAoIObject, m_fAoIVisibility);
			m_pkReal->SendVisibleToGhosts(m_uiAoIObjMask, m_fAoIVisibility);
		}
	}
	else
	{
		m_fAoIVisibility = fVisibility;
	}
}

void FvEntity::SetAoIVision(float fVision)
{
	if(!IsReal())
		return;
	if(m_hAoIObserver != FVAOI_NULL_HANDLE)
	{
		if(m_fAoIVision != fVision)
		{
			m_fAoIVision = fVision;
			m_pkSpace->GetAoIMgr().SetVision(m_hAoIObserver, m_fAoIVision);
		}
	}
	else
	{
		m_fAoIVision = fVision;
	}
}

void FvEntity::SetAoIDisVisibility(float fDisVisibility)
{
	if(!IsReal())
		return;
	if(m_hAoIObserver != FVAOI_NULL_HANDLE)
	{
		if(m_fAoIDisVisibility != fDisVisibility)
		{
			m_fAoIDisVisibility = fDisVisibility;
			m_pkSpace->GetAoIMgr().SetDisVisibility(m_hAoIObserver, m_fAoIDisVisibility);
		}
	}
	else
	{
		m_fAoIDisVisibility = fDisVisibility;
	}
}

bool FvEntity::BoardVehicle(FvEntityPtr spVehicle, BoardVehicleCallBack* pkCallBack)
{
	if(!IsReal())
		return false;

	//! TODO:需要锁住自己,不能下线,不能迁移,不能传送,保证回调可用
	return m_pkReal->BoardVehicle(spVehicle, pkCallBack);
}

void FvEntity::AlightVehicle()
{
	if(!IsReal())
		return;

	return m_pkReal->AlightVehicle();
}

void FvEntity::MoveVehicle(FvMemoryOStream& kData)
{
	if(!IsReal())
		return;

	return m_pkReal->MoveVehicle(kData);
}

void FvEntity::CloseVehicle()
{
	if(!IsReal())
		return;

	return m_pkReal->CloseVehicle();
}

void FvEntity::OnlineLock()
{
	if(!IsReal())
		return;

	m_kRefCnt.AddUsrRefCnt();
	if(IsDestroyEntity())
		FvEntityManager::Instance().RemoveDestroyEntity(this);
}

void FvEntity::OnlineUnLock()
{
	if(!IsReal())
		return;

	m_kRefCnt.DelUsrRefCnt();
	if(m_kRefCnt.CanDestroy())
		FvEntityManager::Instance().AddDestroyEntity(this);
}

bool FvEntity::IsToDRunning() const
{
	return m_pkSpace->IsToDRunning();
}

void FvEntity::StartToD()
{
	m_pkSpace->StartToD();
}

void FvEntity::StopToD()
{
	m_pkSpace->StopToD();
}

void FvEntity::SetToD(double fDayTime, float fTimeScaling)
{
	m_pkSpace->SetToD(fDayTime, fTimeScaling);
}

double FvEntity::GetToD()
{
	return m_pkSpace->GetToD();
}

float FvEntity::GetToDScaling() const
{
	return m_pkSpace->GetToDScaling();
}

int FvEntity::AddTimer(float fInitialOffset, float fRepeatOffset, FvUInt32 uiUserArg)
{
	return 0;
}

void FvEntity::DelTimer(int iID)
{

}

void FvEntity::Destroy()
{
	if(IsDestroy())
		return;

	m_kRefCnt.SetDestroy();

	if(IsReal())
	{
		OnDestroy();
		if(m_kRefCnt.CanDestroy())
			FvEntityManager::Instance().AddDestroyEntity(this);
	}
	else
	{
		UnInitializeGhost();

		FV_ASSERT(m_hAoIObject != FVAOI_NULL_HANDLE);
		m_pkSpace->GetAoIMgr().Remove(m_hAoIObject);
		m_hAoIObject = FVAOI_NULL_HANDLE;
		FV_ASSERT(m_hAoIObserver == FVAOI_NULL_HANDLE);

		m_pkSpace->DelEntity(this);
	}
}

void FvEntity::DestroySpace()
{
	FvNetChannel* pkChannel = FvEntityManager::Instance().CellAppMgr();
	FV_ASSERT(pkChannel);
	CellAppMgrInterface::ShutDownSpaceArgs& kArgs = CellAppMgrInterface::ShutDownSpaceArgs::start(pkChannel->Bundle());
	FV_ASSERT(m_pkSpace);
	kArgs.spaceID = m_pkSpace->SpaceID();
	pkChannel->DelayedSend();
}

void FvEntity::EntitiesInRange(Entities& kEntities, float fRange, const char* pcEntityTypeName, const FvVector3* pkPos) const
{
	if(!IsReal() || fRange <= 0.0f)
		return;

	EntitiesInRange(kEntities, fRange, FvEntityManager::Instance().EntityNameToIndex(pcEntityTypeName), pkPos);
}

void FvEntity::EntitiesInRange(Entities& kEntities, float fRange, FvEntityTypeID uiEntityType, const FvVector3* pkPos) const
{
	if(!IsReal() || fRange <= 0.0f)
		return;

	kEntities.clear();
	FvVector3 kPos = pkPos ? *pkPos : GetPos();

	class Visiter
	{
	public:
		Visiter(Entities& kEntities, FvEntityTypeID uiEntityType):m_kEntities(kEntities),m_uiEntityType(uiEntityType) {}
		FvEntity::Entities& m_kEntities;
		FvEntityTypeID m_uiEntityType;

		void OnVisit(void* pkObjData)
		{
			FV_ASSERT(pkObjData);
			FvEntity* pkEntity = (FvEntity*)pkObjData;
			m_kEntities.push_back(pkEntity);
		}

		void OnVisitWithType(void* pkObjData)
		{
			FV_ASSERT(pkObjData);
			FvEntity* pkEntity = (FvEntity*)pkObjData;
			if(pkEntity->GetEntityTypeID() == m_uiEntityType)
				m_kEntities.push_back(pkEntity);
		}
	};
	Visiter kVisiter(kEntities, uiEntityType);

	if(uiEntityType != FvEntityTypeID(-1))
		m_pkSpace->GetAoIMgr().QueryArea<Visiter, &Visiter::OnVisitWithType>(m_hAoIObject, kPos.x, kPos.y, fRange, &kVisiter);
	else
		m_pkSpace->GetAoIMgr().QueryArea<Visiter, &Visiter::OnVisit>(m_hAoIObject, kPos.x, kPos.y, fRange, &kVisiter);
}

void FvEntity::EntitiesInAoi(Entities& kEntities) const
{
	if(!HasWitness())
		return;

	class Visiter
	{
	public:
		Visiter(Entities& kEntities):m_kEntities(kEntities) {}
		FvEntity::Entities& m_kEntities;

		void OnVisit(void* pkObjData)
		{
			FV_ASSERT(pkObjData);
			m_kEntities.push_back((FvEntity*)pkObjData);
		}
	};
	Visiter kVisiter(kEntities);

	m_pkSpace->GetAoIMgr().QueryVision<Visiter, &Visiter::OnVisit>(m_hAoIObserver, &kVisiter);
}

void FvEntity::EntitiesInTrap(FvInt16 iTrapID, Entities& kEntities) const
{
	if(!IsReal())
		return;

	m_pkReal->EntitiesInTrap(iTrapID, kEntities);
}

bool FvEntity::IsReal() const
{
	FV_ASSERT(!(m_pkRealChannel && m_pkReal));
	FV_ASSERT(m_pkRealChannel || m_pkReal);
	return m_pkReal;
}

void FvEntity::Teleport(const FvMailBox& kNearbyMBRef, const FvVector3& kPosition, const FvDirection3& kDirection)
{
	//printf("%s, Pos[%f,%f,%f]\n", __FUNCTION__, kPosition.x, kPosition.y, kPosition.z);

	if(IsDestroyEntity())
	{
		FV_ERROR_MSG("%s, Entity(EntityID:%d, DBID:%I64d) is waiting for Destroy, Can't do Teleport\n", __FUNCTION__, GetEntityID(), GetDBID());
		return;
	}

	if(FvEntityManager::Instance().IsGlobalMode())
	{
		FV_ERROR_MSG("%s, GlobalEntity:%d Can't do Teleport\n", __FUNCTION__, GetEntityID());
		//printf("%s, GlobalEntity:%d Can't do Teleport\n", __FUNCTION__, GetEntityID());
		return;
	}

	FV_ASSERT(IsReal());

	//! TODO: 传送时暂时不能有载具/乘客
	if(m_pkReal->HasVehicle() || m_pkReal->HasPassenger())
	{
		FV_ERROR_MSG("%s, On Vehicle or Has Passengers\n", __FUNCTION__);
		OnTeleportFailure();
		return;
	}

	//! 确认MB是CellMB
	if(kNearbyMBRef.GetComponent() != FvEntityMailBoxRef::CELL)
	{
		FV_ERROR_MSG("%s, MailBox isn't Cell\n", __FUNCTION__);
		//printf("%s, MailBox isn't Cell\n", __FUNCTION__);
		OnTeleportFailure();
		return;
	}

	//! 查找传送目标
	FvEntity* pkNearbyEntity(NULL);
	pkNearbyEntity = FvEntityManager::Instance().FindEntity(kNearbyMBRef.GetEntityID());
	if(!pkNearbyEntity)
	{
		//! 如果传送地址和本Cell相同
		if(kNearbyMBRef.GetAddr() == FvEntityManager::Instance().Address())
		{
			//! 找不到目标
			FV_ERROR_MSG("%s, Can't Find NearbyEntity:%d\n", __FUNCTION__, kNearbyMBRef.GetEntityID());
			//printf("%s, Can't Find NearbyEntity:%d\n", __FUNCTION__, kNearbyMBRef.GetID());
			OnTeleportFailure();
			return;
		}
		else
		{
			//! 需要远程传送
		}
	}
	else
	{
		//! 如果传送目标和自己在同一个Space,则直接改变位置
		if(pkNearbyEntity->GetSpaceID() == GetSpaceID())
		{
			//! 判断坐标是否越界
			const SpaceInfo* pkSpaceInfo = FvEntityManager::Instance().FindSpaceInfo(pkNearbyEntity->GetSpace()->SpaceType());
			FV_ASSERT(pkSpaceInfo);
			if(pkSpaceInfo->m_kSpaceRect.IsPointIn(kPosition))
			{
				FV_INFO_MSG("%s, NearbyEntity is in the Same Space, Set Pos and Dir directly\n", __FUNCTION__);
				//printf("%s, NearbyEntity is in the Same Space, Set Pos and Dir directly\n", __FUNCTION__);
				SetPosAndDir(kPosition, kDirection);

				//! 纠正位置
				FvNetChannel* pkChannel = m_pkReal->GetChannel();
				FvNetBundle& kBundle = pkChannel->Bundle();
				BaseAppIntInterface::ForcedPositionArgs& kArgs = BaseAppIntInterface::ForcedPositionArgs::start(kBundle);
				kArgs.id		= GetEntityID();
				kArgs.spaceID	= GetSpaceID();
				kArgs.vehicleID	= GetVehicleID();
				kArgs.position	= GetPos();
				kArgs.direction	= GetDir();
				pkChannel->DelayedSend();
			}
			else
			{
				//printf("%s, Teleport Pos(%.1f, %.1f, %.1f) isn't in Space(%.1f, %.1f, %.1f, %.1f), Type:%d\n", __FUNCTION__,
				FV_ERROR_MSG("%s, Teleport Pos(%.1f, %.1f, %.1f) isn't in Space(%.1f, %.1f, %.1f, %.1f), Type:%d\n", __FUNCTION__,
					kPosition.x, kPosition.y, kPosition.z,
					pkSpaceInfo->m_kSpaceRect.LeftBorder(),
					pkSpaceInfo->m_kSpaceRect.RightBorder(),
					pkSpaceInfo->m_kSpaceRect.BottomBorder(),
					pkSpaceInfo->m_kSpaceRect.TopBorder(),
					pkSpaceInfo->m_uiSpaceType);
				OnTeleportFailure();
			}
		}
		else
		{
			//! 判断坐标是否越界
			const SpaceInfo* pkSpaceInfo = FvEntityManager::Instance().FindSpaceInfo(pkNearbyEntity->GetSpace()->SpaceType());
			FV_ASSERT(pkSpaceInfo);
			if(pkSpaceInfo->m_kSpaceRect.IsPointIn(kPosition))
			{
				//! 本地迁移
				FV_INFO_MSG("%s, TeleportLocally from Space:%d to Space:%d\n", __FUNCTION__, GetSpaceID(), pkNearbyEntity->GetSpaceID());
				//printf("%s, TeleportLocally from Space:%d to Space:%d\n", __FUNCTION__, GetSpaceID(), pkNearbyEntity->GetSpaceID());
				TeleportLocally(pkNearbyEntity->GetSpaceID(), kPosition, kDirection);
			}
			else
			{
				//printf("%s, Teleport Pos(%.1f, %.1f, %.1f) isn't in Space(%.1f, %.1f, %.1f, %.1f), Type:%d\n", __FUNCTION__,
				FV_ERROR_MSG("%s, Teleport Pos(%.1f, %.1f, %.1f) isn't in Space(%.1f, %.1f, %.1f, %.1f), Type:%d\n", __FUNCTION__,
					kPosition.x, kPosition.y, kPosition.z,
					pkSpaceInfo->m_kSpaceRect.LeftBorder(),
					pkSpaceInfo->m_kSpaceRect.RightBorder(),
					pkSpaceInfo->m_kSpaceRect.BottomBorder(),
					pkSpaceInfo->m_kSpaceRect.TopBorder(),
					pkSpaceInfo->m_uiSpaceType);
				OnTeleportFailure();
			}
		}

		return;
	}

	//! 远程传送------------------------------------------------------------------------------

	//! 传送前,更新AoI/Trap,处理掉已离开的实体
	if(1)
	{
		if(HasWitness())
			CheckAoI();
		CheckTraps();
	}

	//! 先删除所有Ghost
	m_pkReal->DeleteAllGhosts();

	//! 发送传送包
	//! FvEntityID	NearbyEntityID
	//! FvVector3	pos
	//! kDirection	dir
	//! FvUInt16 uiGhostSize
	//! CreateGhost Data
	//! Real Data

	static FvMemoryOStream kStream(2048);//! 这里不用channel的bundle,为了避免流化中又使用channel的bundle进行流化的问题
	FV_ASSERT(kStream.Size() == 0);

	//! Nearby EntityID
	kStream << kNearbyMBRef.GetEntityID() << kPosition << kDirection;

	//! CreateGhost Data
	int s0 = kStream.Size();
	kStream << FvUInt16(0);	//! 先占位
	int s1 = kStream.Size();
	m_pkReal->CreateGhostData(kStream);
	int s2 = kStream.Size();
	FV_ASSERT(s2-s1 <= 0xFFFF);
	*(FvUInt16*)(((char*)kStream.Data())+s0) = FvUInt16(s2-s1);

	//! Real Data
	ConvertRealToGhost(kStream, kNearbyMBRef.GetAddr(), true);

	//! 删除Ghost
	Destroy();

	//! 发送
	FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kNearbyMBRef.GetAddr());
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(CellAppInterface::OnloadTeleportedEntity);
	kBundle.Transfer(kStream, kStream.Size());
	pkChannel->DelayedSend();

	kStream.Reset();
}

void FvEntity::WriteToDB()
{
	if(FvEntityManager::Instance().IsGlobalMode())
	{
		FV_ERROR_MSG("%s, GlobalEntity:%d Can't Write to DB\n", __FUNCTION__, GetEntityID());
		return;
	}

	FV_ASSERT(IsReal());
	FvNetChannel* pkChannel = m_pkReal->GetChannel();
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::WriteToDB);
	m_kAttrib.SerializeToStreamForCellDBData(kBundle);
	WritePosDirToStream(kBundle);
	//kBundle << FvTimeStamp(0);
	pkChannel->Send();
}

FvInt16 FvEntity::AddTrap(FvUInt16 uiMask, float fVision, float fDisVisibility, FvInt32 iUserData)
{
	FV_ASSERT(IsReal());
	return m_pkReal->AddTrap(uiMask, fVision, fDisVisibility, iUserData);
}

void FvEntity::DelTrap(FvInt16 iTrapID)
{
	FV_ASSERT(IsReal());
	m_pkReal->DelTrap(iTrapID);
}

void FvEntity::SetTrapMask(FvInt16 iTrapID, FvUInt16 uiMask)
{
	FV_ASSERT(IsReal());
	m_pkReal->SetTrapMask(iTrapID, uiMask);
}

void FvEntity::SetTrapVision(FvInt16 iTrapID, float fVision)
{
	FV_ASSERT(IsReal());
	m_pkReal->SetTrapVision(iTrapID, fVision);
}

void FvEntity::SetTrapDisVisibility(FvInt16 iTrapID, float fDisVisibility)
{
	FV_ASSERT(IsReal());
	m_pkReal->SetTrapDisVisibility(iTrapID, fDisVisibility);
}

void FvEntity::SetVisible(bool bVal)
{
	if(bVal)
		SetAoIVisibility(80.0f);
	else
		SetAoIVisibility(0.0f);
}

void FvEntity::DestroyEntity( const CellAppInterface::DestroyEntityArgs & args )
{
	Destroy();
}

void FvEntity::AvatarUpdateImplicit( const CellAppInterface::AvatarUpdateImplicitArgs & args )
{
	if(m_bWaitForcePosAck)
		return;

	bool bChg(false);

	if(m_kPos != args.pos)
	{
		bChg = true;
		m_kPos = args.pos;

		m_pkSpace->GetAoIMgr().Move(m_hAoIObject, m_kPos.x, m_kPos.y);
		if(m_hAoIObserver)
			m_pkSpace->GetAoIMgr().Move(m_hAoIObserver, m_kPos.x, m_kPos.y);
		if(m_pkReal)
			m_pkReal->MoveTraps();
	}

	if(!DirIsEqual(m_kDir, args.dir))
	{
		bChg = true;
		m_kDir = args.dir;
	}

	if(bChg)
		SyncPosDir();
}

void FvEntity::AvatarUpdateExplicit( const CellAppInterface::AvatarUpdateExplicitArgs & args )
{

}

void FvEntity::AckPhysicsCorrection( const CellAppInterface::AckPhysicsCorrectionArgs & args )
{
	OnForcePositionAck();
	m_bWaitForcePosAck = false;
}

void FvEntity::EnableWitness( FvBinaryIStream & data, int length )
{
	FV_ASSERT(IsReal());

	bool bCreate;
	data >> bCreate;

	FV_INFO_MSG("%s, bEnable:%d\n", __FUNCTION__, bCreate);

	if(bCreate)
	{
		FV_ASSERT(!HasWitness());
		m_pkReal->EnableWitness(data);
		OnGetWitness();
	}
	else
	{
		FV_ASSERT(HasWitness());
		OnLoseWitness();
		m_pkReal->DisableWitness();
	}
}

void FvEntity::WitnessCapacity( const CellAppInterface::WitnessCapacityArgs & args )
{

}

void FvEntity::RequestEntityUpdate( FvBinaryIStream & data, int length )
{

}

void FvEntity::WriteToDBRequest( const FvNetAddress & srcAddr, FvNetUnpackedMessageHeader & header, FvBinaryIStream & stream )
{
	FV_ASSERT(!FvEntityManager::Instance().IsGlobalMode());
	FV_ASSERT(IsReal());
	FvNetChannel* pkChannel = m_pkReal->GetChannel();
	FV_ASSERT(pkChannel && header.m_iReplyID!=FV_NET_REPLY_ID_NONE);
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartReply(header.m_iReplyID);
	kBundle << FvUInt8(1);//! 1表示成功
	m_kAttrib.SerializeToStreamForCellDBData(kBundle);
	WritePosDirToStream(kBundle);
	//kBundle << FvTimeStamp(0);
	pkChannel->Send();
}

void FvEntity::Witnessed( const CellAppInterface::WitnessedArgs & args )
{
	//if(IsReal())
	//{
	//	//! 更新ghost
	//	if(m_pkSpace->GetCellInfo()->m_uiIdx != args.cellIdx)
	//	{
	//		if(m_kObserverList.empty())
	//		{
	//			bool bWitnessedOld, bWitnessedNew;
	//			m_pkReal->UpdateGhostWitnessed(args.cellIdx, bWitnessedOld, bWitnessedNew);
	//			if(bWitnessedOld && !bWitnessedNew)
	//			{
	//				m_pkReal->SetWitnessed(false);
	//			}
	//			else if(!bWitnessedOld && bWitnessedNew)
	//			{
	//				m_pkReal->SetWitnessed(true);
	//			}
	//		}
	//	}
	//	else//! 因为迁移导致real/ghost互换,ghost->real'->自己,触发Witness更新
	//	{
	//		if(!m_pkReal->IsGhostsWitnessed())
	//		{
	//			if(m_kObserverList.empty())
	//			{
	//				m_pkReal->SetWitnessed(false);
	//			}
	//			else
	//			{
	//				m_pkReal->SetWitnessed(true);
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//	//! 中转给Real
	//	FV_ASSERT(m_pkRealChannel);
	//	CellAppInterface::WitnessedArgs& kArgs = CellAppInterface::WitnessedArgs::start(m_pkRealChannel->Bundle(), GetEntityID());
	//	kArgs.cellIdx = args.cellIdx;
	//	m_pkRealChannel->DelayedSend();
	//}
}

void FvEntity::CallBaseMethod( FvBinaryIStream & data, int length )
{
	FvInt32 iMethodIdx;
	data >> iMethodIdx;
	CallBaseMethodByIdx(iMethodIdx, data);
}

void FvEntity::CallClientMethod( FvBinaryIStream & data, int length )
{
	if(IsReal())
	{
		FvInt32 iMethodIdx;
		data >> iMethodIdx;
		CallClientMethodByIdx(iMethodIdx, data);
	}
	else
	{
		//! 中转给Real
		FV_ASSERT(m_pkRealChannel);
		FvNetBundle& kBundle = m_pkRealChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::CallClientMethod);
		kBundle << GetEntityID();
		kBundle.Transfer(data, data.RemainingLength());
		m_pkRealChannel->DelayedSend();
	}
}

void FvEntity::RunExposedMethod( int type, FvBinaryIStream & data, int length, const FvMailBox* pkMB )
{
	if(pkMB)
	{
		static FvObjBlob kMB;
		kMB.Reset();
		kMB << *pkMB;
		m_kAttrib.OnMethodFromClientWithCallBack(type, data, kMB);
	}
	else
	{
		m_kAttrib.OnMethodFromClient(type, data);
	}
}

void FvEntity::RunScriptMethod( FvBinaryIStream & data, int length )
{
	if(IsReal())
	{
		FvInt32 iMethodIdx;
		data >> iMethodIdx;
		RunScriptMethodByIdx(iMethodIdx, data);
	}
	else
	{
		//! 中转给Real
		FV_ASSERT(m_pkRealChannel);
		FvNetBundle& kBundle = m_pkRealChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::RunScriptMethod);
		kBundle << GetEntityID();
		kBundle.Transfer(data, data.RemainingLength());
		m_pkRealChannel->DelayedSend();
	}
}

void FvEntity::DelControlledBy( const CellAppInterface::DelControlledByArgs & args )
{

}

void FvEntity::ForwardedBaseEntityPacket( FvBinaryIStream & data, int length )
{
	FvEntityManager::Instance().Nub().ProcessPacketFromStream(m_kBaseAddr, data);
}

void FvEntity::VehicleMsg( FvBinaryIStream & data )
{
	if(IsReal())
	{
		FvUInt8 uiMsgID;
		data >> uiMsgID;

		if(uiMsgID == FvRealEntity::VEHICLE_MSGID_BOARD_REQ)
		{
			//! FvEntityID iPassengerID
			//! FvNetAddress kAddr
			FvEntityID iPassengerID(-1);
			FvNetAddress kAddr;
			data >> iPassengerID >> kAddr;
			BoardVehicleReq(iPassengerID, kAddr);
		}
		else if(uiMsgID == FvRealEntity::VEHICLE_MSGID_BOARD_REP)
		{
			//! bool bSuccess
			//! FvEntityID iVehicleID
			bool bSuccess(false);
			FvEntityID iVehicleID(-1);
			data >> bSuccess >> iVehicleID;
			BoardVehicleRep(bSuccess, iVehicleID);
		}
		else if(uiMsgID == FvRealEntity::VEHICLE_MSGID_ALIGHT)
		{
			//! FvEntityID iPassengerID
			FvEntityID iPassengerID(-1);
			data >> iPassengerID;
			AlightVehicleReq(iPassengerID);
		}
		else if(uiMsgID == FvRealEntity::VEHICLE_MSGID_MOVE)
		{
			//! FvEntityID iVehicleID
			//! FvBinaryIStream kData
			FvEntityID iVehicleID(-1);
			data >> iVehicleID;
			MoveVehicleReq(iVehicleID, data);
		}
		else if(uiMsgID == FvRealEntity::VEHICLE_MSGID_CLOSE)
		{
			//! FvEntityID iVehicleID
			FvEntityID iVehicleID(-1);
			data >> iVehicleID;
			CloseVehicleReq(iVehicleID);
		}
		else
		{
			FV_ASSERT(0);
		}
	}
	else
	{
		FvUInt8 uiMsgID;
		data >> uiMsgID;

		if(uiMsgID == FvRealEntity::VEHICLE_MSGID_SETID)
		{
			//! FvEntityID iVehicleID
			FvEntityID iVehicleID(-1);
			data >> iVehicleID;
			SetVehicleID(iVehicleID);
			SyncVehicleToObservers();
		}
		else
		{
			//! 中转给Real
			FV_ASSERT(m_pkRealChannel);
			FvNetBundle& kBundle = m_pkRealChannel->Bundle();
			kBundle.StartMessage(CellAppInterface::VehicleMsg);
			kBundle << GetEntityID() << uiMsgID;
			kBundle.Transfer(data, data.RemainingLength());
			m_pkRealChannel->DelayedSend();
		}
	}
}

void FvEntity::RPCCallBack( FvBinaryIStream & data )
{
	FvUInt32 uiCBID;
	data >> uiCBID;
	FV_ASSERT(uiCBID < 0xFFFFFF00);

	if(IsReal())
	{
		//! TODO: 处理回调:Cell
		OnRPCResult(uiCBID, data);

		m_kRefCnt.DelSysRefCnt();
		if(m_kRefCnt.CanDestroy())
			FvEntityManager::Instance().AddDestroyEntity(this);
	}
	else
	{
		//! 中转给Real
		FV_ASSERT(m_pkRealChannel);
		FvNetBundle& kBundle = m_pkRealChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::RPCCallBack);
		kBundle << GetEntityID() << uiCBID;
		kBundle.Transfer(data, data.RemainingLength());
		m_pkRealChannel->DelayedSend();
	}
}

void FvEntity::RPCCallBackException( const CellAppInterface::RPCCallBackExceptionArgs & args )
{
	FvUInt32 uiCBID = args.uiCBID;
	FV_ASSERT(uiCBID < 0xFFFFFF00);

	if(IsReal())
	{
		//! TODO: 处理回调异常:Cell
		OnRPCException(uiCBID);

		m_kRefCnt.DelSysRefCnt();
		if(m_kRefCnt.CanDestroy())
			FvEntityManager::Instance().AddDestroyEntity(this);
	}
	else
	{
		//! 中转给Real
		FV_ASSERT(m_pkRealChannel);
		FvNetBundle& kBundle = m_pkRealChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::RPCCallBackException);
		CellAppInterface::RPCCallBackExceptionArgs kArgs;
		kArgs.uiCBID = uiCBID;
		kBundle << GetEntityID() << kArgs;
		m_pkRealChannel->DelayedSend();
	}
}

void FvEntity::GMCmd( FvBinaryIStream & data, int length )
{
	if(IsReal())
	{
		FvUInt16 uiMethodID;
		data >> uiMethodID;
		MethodCnts& kMethods = m_kAttrib.GetExport()->kMethodCnts;
		if(data.Error() || kMethods.uiCellExpCnt <= uiMethodID || kMethods.pkCellExpID[uiMethodID].uiGMLevel == 0xFFFF)
		{
			FV_ERROR_MSG("GMCmd data Err\n");
			data.Finish();
			return;
		}
		//! 判断GM等级
		if(!CanDoGMCmd(kMethods.pkCellExpID[uiMethodID].uiGMLevel))
		{
			FV_ERROR_MSG("GMCmd level too high\n");
			data.Finish();
			return;
		}
		m_kAttrib.GetExport()->pFunEntityMethodsEntry(this, kMethods.pkCellExpID[uiMethodID].uiExpToIdx, data);
	}
	else
	{
		//! 中转给Real
		FV_ASSERT(m_pkRealChannel);
		FvNetBundle& kBundle = m_pkRealChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::GMCmd);
		kBundle << GetEntityID();
		kBundle.Transfer(data, length);
		m_pkRealChannel->DelayedSend();
	}
}

void FvEntity::DelGhost( const CellAppInterface::DelGhostArgs & args )
{
	FV_ASSERT(!IsReal());
	Destroy();
}

void FvEntity::GhostVolatileInfo(	const CellAppInterface::GhostVolatileInfoArgs & args )
{

}

void FvEntity::GhostAvatarUpdate(	const CellAppInterface::GhostAvatarUpdateArgs & args )
{
	FV_ASSERT(!IsReal());

	if(m_kPos != args.pos)
	{
		m_kPos = args.pos;
		m_pkSpace->GetAoIMgr().Move(m_hAoIObject, m_kPos.x, m_kPos.y);
	}
	m_kDir = args.dir;

	//! 同步给观察者
	class Visiter
	{
	public:
		Visiter(FvEntityID iEntityID, FvVector3& kPos, FvDirection3& kDir):m_iEntityID(iEntityID),m_kPos(kPos),m_kDir(kDir) {}
		FvEntityID m_iEntityID;
		FvVector3& m_kPos;
		FvDirection3& m_kDir;

		void OnVisit(void* pkObsData)
		{
			FvEntity* pkEntity = (FvEntity*)pkObsData;
			pkEntity->AvatarUpdateForOtherClient(m_iEntityID, 0, m_kPos, m_kDir, 0);
		}
	};
	Visiter kVisiter(m_iEntityID, m_kPos, m_kDir);

	m_pkSpace->GetAoIMgr().QueryDVision<Visiter, &Visiter::OnVisit>(m_hAoIObject, &kVisiter);
}

void FvEntity::GhostHistoryEvent( FvBinaryIStream & data, int length )
{
	bool bPropEvent;
	FvEventNumber uiLastEventNumber;
	GhostUpdateNumber uiGhostUpdateNumber;
	FvDetailLevel uiLevel;
	FvUInt8 uiMessageID;
	data >> bPropEvent >> uiLastEventNumber >> uiGhostUpdateNumber >> uiLevel >> uiMessageID;

	//! 属性更新消息
	if(bPropEvent)
	{
		FvHistoryEvent::Level kLevel(uiLevel);
		FvHistoryEvent* pkEvent = new FvHistoryEvent(uiMessageID | 0x80, uiLastEventNumber, data.Retrieve(0), data.RemainingLength(), kLevel, GetEntityID());

		FvEventNumber uiNeedEventNumber = m_uiLastEventNumber +1;
		GhostUpdateNumber uiNeedGhostUpdateNumber = m_uiLastGhostUpdateNumber +1;
		if(uiLastEventNumber==uiNeedEventNumber && uiGhostUpdateNumber==uiNeedGhostUpdateNumber)
		{
			m_uiLastEventNumber = uiNeedEventNumber;
			m_uiLastGhostUpdateNumber = uiNeedGhostUpdateNumber;

			m_kEventHistory.Add(pkEvent);
			DoGhostDataUpdate(uiMessageID, data);

			//! 检查保存的数据
			CheckGhostEventCache();
		}
		else
		{
			//! 先保存起来
			GhostEventCache* pkCache = new GhostEventCache(uiGhostUpdateNumber, uiMessageID, data.Retrieve(0), data.RemainingLength(), uiLastEventNumber, pkEvent);
			data.Finish();
			InsertGhostEventCache(pkCache);
		}
	}
	else//! 方法调用消息
	{
		FvHistoryEvent::Level kLevel(FLT_MAX);
		FvHistoryEvent* pkEvent = new FvHistoryEvent(uiMessageID | 0xC0, uiLastEventNumber, data.Retrieve(0), data.RemainingLength(), kLevel, GetEntityID());
		data.Finish();

		FvEventNumber uiNeedEventNumber = m_uiLastEventNumber +1;
		GhostUpdateNumber uiNeedGhostUpdateNumber = m_uiLastGhostUpdateNumber;//! 不增加
		if(uiLastEventNumber==uiNeedEventNumber && uiGhostUpdateNumber==uiNeedGhostUpdateNumber)
		{
			m_uiLastEventNumber = uiNeedEventNumber;
			m_uiLastGhostUpdateNumber = uiNeedGhostUpdateNumber;

			m_kEventHistory.Add(pkEvent);

			//! 检查保存的数据
			CheckGhostEventCache();
		}
		else
		{
			//! 先保存起来
			GhostEventCache* pkCache = new GhostEventCache(uiGhostUpdateNumber, uiMessageID, data.Retrieve(0), data.RemainingLength(), uiLastEventNumber, pkEvent);
			data.Finish();
			InsertGhostEventCache(pkCache);
		}
	}
}

void FvEntity::GhostedDataUpdate( FvBinaryIStream & data, int length )
{
	GhostUpdateNumber uiGhostUpdateNumber;
	FvUInt8 uiMessageID;
	data >> uiGhostUpdateNumber >> uiMessageID;

	GhostUpdateNumber uiNeedGhostUpdateNumber = m_uiLastGhostUpdateNumber +1;
	if(uiGhostUpdateNumber == uiNeedGhostUpdateNumber)
	{
		m_uiLastGhostUpdateNumber = uiNeedGhostUpdateNumber;
		DoGhostDataUpdate(uiMessageID, data);

		//! 检查保存的数据
		CheckGhostEventCache();
	}
	else
	{
		//! TODO: 有隐患
		//! 假设Cell1,Cell2上有A的Ghost,Cell3上是Real
		//! Real往Cell2上迁移,因为Cell1和Cell3之间Channel比较忙
		//! 迁移到Cell2上完成后给Cell1发消息时,此前Cell3发给Cell1的消息还未收到
		//! 所以Cell1上需要做消息保存
		//! 但是如果A又从Cell2迁移到Cell1,Cell3发给Cell1的消息还未到
		//! 这时Cell1上的消息队列是不完整的,如何处理?
		//! 这之后收到Cell3发给Cell1的消息,又如何处理?
		//! 先保存起来
		GhostEventCache* pkCache = new GhostEventCache(uiGhostUpdateNumber, uiMessageID, data.Retrieve(0), data.RemainingLength());
		data.Finish();
		InsertGhostEventCache(pkCache);
	}
}

void FvEntity::GhostedOtherClientDataUpdate( FvBinaryIStream & data, int length )
{

}

void FvEntity::AoiPriorityUpdate(	const CellAppInterface::AoiPriorityUpdateArgs & args )
{

}

void FvEntity::GhostSetReal( const CellAppInterface::GhostSetRealArgs & args )
{
	FV_ASSERT(m_pkRealChannel);
	m_pkRealChannel = FvEntityManager::Instance().FindOrCreateChannel(args.owner);
}

void FvEntity::GhostSetNextReal( const CellAppInterface::GhostSetNextRealArgs & args )
{

}

void FvEntity::GhostControllerExist( FvBinaryIStream & data, int length )
{

}

void FvEntity::GhostControllerUpdate( FvBinaryIStream & data, int length )
{

}

void FvEntity::CheckGhostWitnessed( const CellAppInterface::CheckGhostWitnessedArgs & args )
{

}

void FvEntity::Onload( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	ConvertGhostToReal(data);
}

void FvEntity::VisibleUpdate(const CellAppInterface::VisibleUpdateArgs& args)
{
	FV_ASSERT(!IsReal() && m_hAoIObject != FVAOI_NULL_HANDLE);
	if(m_uiAoIObjMask != args.uiObjMask)
	{
		m_uiAoIObjMask = args.uiObjMask;
		m_pkSpace->GetAoIMgr().SetMask(m_hAoIObject, m_uiAoIObjMask);
	}
	if(m_fAoIVisibility != args.fVisibility)
	{
		m_fAoIVisibility = args.fVisibility;
		m_pkSpace->GetAoIMgr().SetVisibility(m_hAoIObject, m_fAoIVisibility);
	}
}

FvSpaceID FvEntity::GetSpaceID() const
{
	return m_pkSpace->SpaceID();
}

FvUInt16 FvEntity::GetSpaceType() const
{
	return m_pkSpace->SpaceType();
}

bool FvEntity::IsSpaceLoaded() const
{
	return m_pkSpace->IsSpaceLoaded();
}

void FvEntity::CheckEventSync()
{
	FV_ASSERT(HasWitness());
	m_pkReal->pWitness()->CheckEventSync();
	m_pkReal->GetChannel()->Send();
}

bool FvEntity::CheckTransition()
{
	//! TODO: 可以记录pos是否改变,如未改变则不需要检测
	FV_ASSERT(IsReal());

	if(IsDestroyEntity())
		return false;

	if(m_pkSpace->GetCell()->IsInCell(m_kPos))
		return false;

	FvSpace::CellInfo* pkCellInfo = m_pkSpace->GetCellAt(m_kPos.x, m_kPos.y);
	if(!pkCellInfo)
	{
		//FV_ASSERT(pkCellInfo);
		FV_ERROR_MSG("%s, Pos(%f,%f) is Not in Space(%d)\n", __FUNCTION__, m_kPos.x, m_kPos.y, m_pkSpace->SpaceType());
		return false;
	}

	//! 迁移前,更新AoI/Trap,处理掉已离开的实体
	if(1)
	{
		if(HasWitness())
			CheckAoI();
		CheckTraps();
	}

	//! 如果传送目的地没有ghost则先创建ghost
	m_pkReal->CreateGhostIfNo(pkCellInfo->m_uiIdx, pkCellInfo->m_kAddr);

	static FvMemoryOStream kStream(2048);//! 这里不用channel的bundle,为了避免流化中又使用channel的bundle进行流化的问题
	FV_ASSERT(kStream.Size() == 0);

	ConvertRealToGhost(kStream, pkCellInfo->m_kAddr, false);

	FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(pkCellInfo->m_kAddr);
	pkChannel->Bundle().StartMessage(CellAppInterface::Onload);
	pkChannel->Bundle().Transfer(kStream, kStream.Size());
	pkChannel->DelayedSend();
	kStream.Reset();
	return true;
}

void FvEntity::CheckGhosts()
{
	//! TODO: 可以记录pos是否改变,如未改变则不需要检测
	FV_ASSERT(IsReal());

	if(IsDestroyEntity())
		return;

	m_pkReal->CheckGhosts();
}

void FvEntity::CheckAoI()
{
	FV_ASSERT(HasWitness());

	class Listener
	{
	public:
		Listener(FvEntity* pkEntity):m_pkEntity(pkEntity){}
		FvEntity* m_pkEntity;

		void OnEnter(void* pkObsData, FvAoIExt* pkExt)
		{
			AoICache* pkCache = (AoICache*)pkExt;
			pkCache->InAoI(m_pkEntity);
		}

		void OnStand(void* pkObsData, FvAoIExt* pkExt)
		{
			AoICache* pkCache = (AoICache*)pkExt;
			pkCache->UpdateAoI(m_pkEntity);
		}

		void OnLeave(void* pkObsData, FvAoIExt* pkExt)
		{
			AoICache* pkCache = (AoICache*)pkExt;
			pkCache->OutAoI(m_pkEntity);
		}
	};
	Listener kListener(this);

#ifndef FV_DEBUG
	m_pkSpace->GetAoIMgr().UpdateAoI<Listener,
		&Listener::OnEnter,
		&Listener::OnStand,
		&Listener::OnLeave>(m_hAoIObserver, &kListener);
#else
	FvUInt64 a,b,c;
	m_pkSpace->GetAoIMgr().UpdateAoI<Listener,
		&Listener::OnEnter,
		&Listener::OnStand,
		&Listener::OnLeave>(m_hAoIObserver, &kListener, a, b, c);
#endif

	SendToClient();
}

void FvEntity::CheckTraps()
{
	FV_ASSERT(IsReal());
	m_pkReal->CheckTraps();
}

bool FvEntity::HasWitness() const
{
	if(m_pkReal && m_pkReal->pWitness())
		return true;
	else
		return false;
}

bool FvEntity::InitReal(FvBinaryIStream& stream, FvUInt8 uiInitFlg)
{
	FvCellEntityExport* pkExport = m_kAttrib.GetExport();
	FV_ASSERT(pkExport && !m_pkRealChannel && !m_pkReal);
	m_kCellMB.Set(m_iEntityID, pkExport->uiTypeID, FvEntityManager::Instance().Nub().Address(), FvEntityMailBoxRef::CELL);
	m_kAttrib.CreateRealData();
	m_kAttrib.DeserializeFromStreamForCellData(stream);

	m_pkReal = new FvRealEntity(*this);
	m_pkReal->Init(NULL, CREATE_REAL_FROM_INIT);
	m_pkSpace->AddEntity(this);

	PreInitialize();

	FV_ASSERT(m_hAoIObject == FVAOI_NULL_HANDLE);
	m_hAoIObject = m_pkSpace->GetAoIMgr().AddObject(m_uiAoIObjMask, m_kPos.x, m_kPos.y, m_fAoIVisibility, this);

	Initialize(uiInitFlg);
	m_kAttrib.SetAttribEventCallBack(&m_kAttribEventCallBack);
	m_kAttrib.AttachOwnerForCellData();

	return true;
}

bool FvEntity::InitReal(const FvAllData* pkAllData, FvUInt8 uiInitFlg)
{
	FvCellEntityExport* pkExport = m_kAttrib.GetExport();
	FV_ASSERT(pkExport && pkAllData && !m_pkRealChannel && !m_pkReal);
	m_kCellMB.Set(m_iEntityID, pkExport->uiTypeID, FvEntityManager::Instance().Nub().Address(), FvEntityMailBoxRef::CELL);
	m_kAttrib.CreateRealData();
	InitAttrib(pkAllData);

	m_pkReal = new FvRealEntity(*this);
	m_pkReal->Init(NULL, CREATE_REAL_FROM_INIT);
	m_pkSpace->AddEntity(this);

	PreInitialize();

	FV_ASSERT(m_hAoIObject == FVAOI_NULL_HANDLE);
	m_hAoIObject = m_pkSpace->GetAoIMgr().AddObject(m_uiAoIObjMask, m_kPos.x, m_kPos.y, m_fAoIVisibility, this);

	Initialize(uiInitFlg);
	m_kAttrib.SetAttribEventCallBack(&m_kAttribEventCallBack);
	m_kAttrib.AttachOwnerForCellData();

	return true;
}

bool FvEntity::InitGhost(FvBinaryIStream& stream, FvNetAddress& kRealAddr)
{
	FvCellEntityExport* pkExport = m_kAttrib.GetExport();
	FV_ASSERT(pkExport && !m_pkRealChannel && !m_pkReal);
	m_kCellMB.Set(m_iEntityID, pkExport->uiTypeID, kRealAddr, FvEntityMailBoxRef::CELL);//! TODO: 迁移之后需要把Real的新地址更新给所有Ghost
	m_pkRealChannel = FvEntityManager::Instance().FindOrCreateChannel(kRealAddr);
	stream >> m_uiLastEventNumber >> m_uiLastVolatileUpdateNumber >> m_uiLastGhostUpdateNumber;
	m_kAttrib.DeserializeFromStreamForGhostData(stream);
	m_pkSpace->AddEntity(this);
	InitializeGhost();
	m_kAttrib.AttachOwnerForGhostData();

	FV_ASSERT(m_hAoIObject == FVAOI_NULL_HANDLE);
	m_hAoIObject = m_pkSpace->GetAoIMgr().AddObject(m_uiAoIObjMask, m_kPos.x, m_kPos.y, m_fAoIVisibility, this);

	return true;
}

bool FvEntity::ConvertRealToGhost(FvBinaryOStream& stream, const FvNetAddress& kRealAddr, bool bTeleport)
{
	FV_ASSERT(IsReal());
	m_kAttrib.SetAttribEventCallBack(NULL);

	OnLeavingCell();

	stream << GetEntityID();
	stream << FvUInt8(0);//! flag: 0:请求迁移, 1:传送失败后的恢复
	FV_ASSERT(m_uiDestroyEntityIdx == 0xFFFFFFFF);
	stream << m_uiRPCCallBackID << m_kRefCnt;
	m_kAttrib.SerializeToStreamForRealData(stream);
	m_pkReal->Offload(stream, kRealAddr, bTeleport);
	Serializer(stream);//! FvController的序列化
	m_kAttrib.DestroyRealData();
	delete m_pkReal;
	m_pkReal = NULL;
	m_pkRealChannel = FvEntityManager::Instance().FindOrCreateChannel(kRealAddr);
	m_pkSpace->RealToGhost(this);
	m_kCellMB.SetAddr(kRealAddr);

	OnLeftCell();
	return true;
}

bool FvEntity::ConvertGhostToReal(FvBinaryIStream& stream)
{
	OnEnteringCell();

	//! flag: 0:请求迁移, 1:传送失败后的恢复
	FvUInt8 uiFlag(0);
	stream >> uiFlag;

	FV_ASSERT(!IsReal() && m_kAttrib.GetExport());
	FV_ASSERT(m_uiDestroyEntityIdx == 0xFFFFFFFF);
	stream >> m_uiRPCCallBackID >> m_kRefCnt;
	m_kAttrib.CreateRealData();
	m_kAttrib.DeserializeFromStreamForRealData(stream);
	m_pkRealChannel = NULL;
	m_pkReal = new FvRealEntity(*this);
	m_pkReal->Init(&stream, CREATE_REAL_FROM_OFFLOAD);
	m_pkSpace->GhostToReal(this);
	m_kCellMB.SetAddr(FvEntityManager::Instance().Nub().Address());
	Unserializer(stream);//! FvController的反序列化

	OnEnteredCell();

	m_kAttrib.SetAttribEventCallBack(&m_kAttribEventCallBack);
	m_kAttrib.AttachOwnerForRealData();

	//! 传送失败
	if(uiFlag == 1)
		OnTeleportFailure();

	return true;
}

void FvEntity::AvatarUpdateForOtherClient(FvEntityID iEntityID, FvUInt32 uiTime, FvVector3& kPos, FvDirection3& kDir, FvUInt8 uiFlag)
{
	FV_ASSERT(HasWitness());
	m_pkReal->AvatarUpdateForOtherClient(iEntityID, uiTime, kPos, kDir, uiFlag);
}

void FvEntity::SendToClient()
{
	if(IsReal())
		m_pkReal->GetChannel()->Send();
}

void FvEntity::AddOwnClientPropertyEvent(FvUInt8 uiMessageID, FvBinaryIStream& kMsg)
{
	if(m_kBaseAddr.IsNone() || !HasWitness())
		return;

	//! 直接发送给自己
	FvNetChannel* pkChannel = m_pkReal->GetChannel();
	FV_ASSERT(pkChannel);
	FvNetBundle& kBundle = pkChannel->Bundle();
	static FvNetInterfaceElement kInterface = BaseAppIntInterface::EntityMessage;
	kInterface.SetID(uiMessageID | 0x80);
	kBundle.StartMessage(kInterface);
	kBundle << GetEntityID();
	kBundle.AddBlob(kMsg.Retrieve(0), kMsg.RemainingLength());
	pkChannel->DelayedSend();
}

void FvEntity::AddGhostPropertyEvent(FvUInt8 uiMessageID, FvBinaryIStream& kMsg, bool bOthersCli)
{
	if(bOthersCli)
	{
		++m_uiLastEventNumber;
		++m_uiLastGhostUpdateNumber;

		//FvHistoryEvent::Level kLevel((FvDetailLevel)pkDataDes->DetailLevel());
		FvHistoryEvent::Level kLevel((FvDetailLevel)0);
		FvHistoryEvent* pkEvent = new FvHistoryEvent(uiMessageID | 0x80, m_uiLastEventNumber, kMsg.Retrieve(0), kMsg.RemainingLength(), kLevel, GetEntityID());
		m_kEventHistory.Add(pkEvent);

		m_pkReal->SendHistoryEventToGhosts(true, uiMessageID, kMsg, m_uiLastEventNumber, m_uiLastGhostUpdateNumber, kLevel.m_uiDetail);
	}
	else
	{
		++m_uiLastGhostUpdateNumber;

		m_pkReal->SendGhostDataToGhosts(uiMessageID, kMsg, m_uiLastGhostUpdateNumber);
	}
}

void FvEntity::DoGhostDataUpdate(FvUInt8 uiMessageID, FvBinaryIStream& kMsg)
{
	m_kAttrib.OnProperty(uiMessageID, kMsg);
}

void FvEntity::TrimHistoryEvent()
{
	m_kEventHistory.Trim();
}

void FvEntity::InsertGhostEventCache(GhostEventCache* pkCache)
{
	FV_ASSERT(pkCache);

	bool bInsert(false);
	GhostEventCacheList::iterator itrB = m_kGhostEventCacheList.begin();
	GhostEventCacheList::iterator itrE = m_kGhostEventCacheList.end();
	while(itrB != itrE)
	{
		GhostEventCache* pkNow = *itrB;
		if(pkNow->uiGhostUpdateNumber == pkCache->uiGhostUpdateNumber)
		{
			FV_ERROR_MSG("%s, EventNumber:%d is Exist\n", __FUNCTION__, pkCache->uiGhostUpdateNumber);
			FV_ASSERT(0);
			return;
		}
		else if(GhostUpdateNumberLessThan(pkNow->uiGhostUpdateNumber, pkCache->uiGhostUpdateNumber))
		{
			++itrB;
		}
		else
		{
			m_kGhostEventCacheList.insert(itrB, pkCache);
			bInsert = true;
			break;
		}
	}

	if(!bInsert)
	{
		m_kGhostEventCacheList.push_back(pkCache);
	}

	FV_INFO_MSG("%s, Add:%d, Cnt:%d\n", __FUNCTION__, pkCache->uiGhostUpdateNumber, m_kGhostEventCacheList.size());
}

void FvEntity::CheckGhostEventCache()
{
	if(m_kGhostEventCacheList.empty())
		return;

	GhostEventCacheList::iterator itrB = m_kGhostEventCacheList.begin();
	GhostEventCacheList::iterator itrE = m_kGhostEventCacheList.end();
	while(itrB != itrE)
	{
		GhostEventCache* pkNow = *itrB;
		if(pkNow->pkEvent)
		{
			//! 属性更新
			if((pkNow->pkEvent->Number() & 0x80) == 0x80)
			{
				FvEventNumber uiNeedEventNumber = m_uiLastEventNumber +1;
				GhostUpdateNumber uiNeedGhostUpdateNumber = m_uiLastGhostUpdateNumber +1;
				if(pkNow->uiLastEventNumber==uiNeedEventNumber && pkNow->uiGhostUpdateNumber==uiNeedGhostUpdateNumber)
				{
					FV_INFO_MSG("%s, Del:%d, Cnt:%d\n", __FUNCTION__, pkNow->uiGhostUpdateNumber, m_kGhostEventCacheList.size());

					m_uiLastEventNumber = uiNeedEventNumber;
					m_uiLastGhostUpdateNumber = uiNeedGhostUpdateNumber;

					m_kEventHistory.Add(pkNow->pkEvent);
					pkNow->pkEvent = NULL;
					FvMemoryIStream mis(pkNow->pcGhostData, pkNow->iGhostDataLen);
					DoGhostDataUpdate(pkNow->uiMessageID, mis);

					delete pkNow;
					itrB = m_kGhostEventCacheList.erase(itrB);
				}
				else
				{
					break;
				}
			}
			else//! 方法调用
			{
				FvEventNumber uiNeedEventNumber = m_uiLastEventNumber +1;
				GhostUpdateNumber uiNeedGhostUpdateNumber = m_uiLastGhostUpdateNumber;//! 不增加
				if(pkNow->uiLastEventNumber==uiNeedEventNumber && pkNow->uiGhostUpdateNumber==uiNeedGhostUpdateNumber)
				{
					FV_INFO_MSG("%s, Del:%d, Cnt:%d\n", __FUNCTION__, pkNow->uiGhostUpdateNumber, m_kGhostEventCacheList.size());

					m_uiLastEventNumber = uiNeedEventNumber;
					m_uiLastGhostUpdateNumber = uiNeedGhostUpdateNumber;

					m_kEventHistory.Add(pkNow->pkEvent);
					pkNow->pkEvent = NULL;

					delete pkNow;
					itrB = m_kGhostEventCacheList.erase(itrB);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			GhostUpdateNumber uiNeedGhostUpdateNumber = m_uiLastGhostUpdateNumber +1;
			if(pkNow->uiGhostUpdateNumber==uiNeedGhostUpdateNumber)
			{
				FV_INFO_MSG("%s, Del:%d, Cnt:%d\n", __FUNCTION__, pkNow->uiGhostUpdateNumber, m_kGhostEventCacheList.size());

				m_uiLastGhostUpdateNumber = uiNeedGhostUpdateNumber;

				FvMemoryIStream mis(pkNow->pcGhostData, pkNow->iGhostDataLen);
				DoGhostDataUpdate(pkNow->uiMessageID, mis);

				delete pkNow;
				itrB = m_kGhostEventCacheList.erase(itrB);
			}
			else
			{
				break;
			}
		}
	}
}

void FvEntity::CallBaseMethodByIdx( FvInt32 iMethodIdx, FvBinaryIStream& data )
{
	//! 转给Base
	if(m_kBaseAddr.IsNone())
	{
		FV_ERROR_MSG("%s, no Base can Call\n", __FUNCTION__);
		CallRPCCallbackException(iMethodIdx, data);
		data.Finish();
		return;
	}

	FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kBaseAddr);
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::CallBaseMethod);
	kBundle << GetEntityID() << iMethodIdx;
	kBundle.Transfer(data, data.RemainingLength());
	pkChannel->DelayedSend();
}

void FvEntity::CallClientMethodByIdx( FvInt32 iMethodIdx, FvBinaryIStream& data )
{
	FV_ASSERT(IsReal());

	if(m_kBaseAddr.IsNone())
	{
		//FV_ERROR_MSG("%s, no Base can Call for id:%d\n", __FUNCTION__, GetEntityID());	//! modify by Uman, 20100507, 暂时关闭
		data.Finish();
		return;
	}

	if(!HasWitness())
	{
		FV_ERROR_MSG("%s, no Client can Call for id:%d\n", __FUNCTION__, GetEntityID());
		data.Finish();
		return;
	}

	//! 直接发送给自己
	FV_ASSERT(iMethodIdx < m_kAttrib.GetExport()->kMethodCnts.uiClientCnt);

	int baseId, extId;
	FvDataCompressID<6>(m_kAttrib.GetExport()->kMethodCnts.uiClientCnt, iMethodIdx, baseId, extId);

	FvNetChannel* pkChannel = m_pkReal->GetChannel();
	FV_ASSERT(pkChannel);
	FvNetBundle& kBundle = pkChannel->Bundle();
	static FvNetInterfaceElement kInterface = BaseAppIntInterface::EntityMessage;
	kInterface.SetID(baseId | 0xC0);
	kBundle.StartMessage(kInterface);
	kBundle << GetEntityID();
	if(extId != -1) kBundle << FvUInt8(extId);
	kBundle.Transfer(data, data.RemainingLength());
	pkChannel->DelayedSend();
}

void FvEntity::RunScriptMethodByIdx( FvInt32 iMethodIdx, FvBinaryIStream& data )
{
	FV_ASSERT(IsReal());
	m_kAttrib.OnMethodFromServer(iMethodIdx, data);
}

void FvEntity::CallClientsMethodEvent(FvUInt8 uiMessageID, FvBinaryIStream& kMsg, bool bAllClients)
{
	FV_ASSERT(IsReal());

	//! 记录到消息队列
	++m_uiLastEventNumber;

	FvHistoryEvent::Level kLevel(FLT_MAX);
	FvHistoryEvent* pkEvent = new FvHistoryEvent(uiMessageID | 0xC0, m_uiLastEventNumber, kMsg.Retrieve(0), kMsg.RemainingLength(), kLevel, GetEntityID());
	m_kEventHistory.Add(pkEvent);

	//! 发送给Ghost
	m_pkReal->SendHistoryEventToGhosts(false, uiMessageID, kMsg, m_uiLastEventNumber, m_uiLastGhostUpdateNumber, kLevel.m_uiDetail);

	//! 发给自己
	if(bAllClients)
	{
		if(!HasWitness())
		{
			//FV_ERROR_MSG("%s, no Client can Call for id:%d\n", __FUNCTION__, GetEntityID());
			kMsg.Finish();
			return;
		}

		FvNetChannel* pkChannel = m_pkReal->GetChannel();
 		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();
		static FvNetInterfaceElement kInterface = BaseAppIntInterface::EntityMessage;
		kInterface.SetID(uiMessageID | 0xC0);
		kBundle.StartMessage(kInterface);
		kBundle << GetEntityID();
		kBundle.Transfer(kMsg, kMsg.RemainingLength());
		pkChannel->DelayedSend();
	}
}

FvNetChannel* FvEntity::GetChannelForBaseMethod(FvInt32 iMethodIdx)
{
	//! 转给Base
	if(m_kBaseAddr.IsNone())
	{
		FV_ERROR_MSG("%s, no Base can Call\n", __FUNCTION__);
		return NULL;
	}

	FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kBaseAddr);
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::CallBaseMethod);
	kBundle << GetEntityID() << iMethodIdx;
	return pkChannel;
}

FvNetChannel* FvEntity::GetChannelForClientMethod(FvInt32 iMethodIdx)
{
	FV_ASSERT(IsReal());

	if(m_kBaseAddr.IsNone())
	{
		//FV_ERROR_MSG("%s, no Base can Call for id:%d\n", __FUNCTION__, GetEntityID());	//! modify by Uman, 20100507, 暂时关闭
		return NULL;
	}

	if(!HasWitness())
	{
		FV_ERROR_MSG("%s, no Client can Call for id:%d\n", __FUNCTION__, GetEntityID());
		return NULL;
	}

	//! 直接发送给自己
	FV_ASSERT(iMethodIdx < m_kAttrib.GetExport()->kMethodCnts.uiClientCnt);

	int baseId, extId;
	FvDataCompressID<6>(m_kAttrib.GetExport()->kMethodCnts.uiClientCnt, iMethodIdx, baseId, extId);

	FvNetChannel* pkChannel = m_pkReal->GetChannel();
	FV_ASSERT(pkChannel);
	FvNetBundle& kBundle = pkChannel->Bundle();
	static FvNetInterfaceElement kInterface = BaseAppIntInterface::EntityMessage;
	kInterface.SetID(baseId | 0xC0);
	kBundle.StartMessage(kInterface);
	kBundle << GetEntityID();
	if(extId != -1) kBundle << FvUInt8(extId);
	return pkChannel;
}

void FvEntity::GetTraps(std::vector<float>& kTraps)
{
	FV_ASSERT(IsReal());
	return m_pkReal->GetTraps(kTraps);
}

void FvEntity::TeleportFinish(FvSpaceID iOldSpaceID)
{
	FV_ASSERT(IsReal());

	FvNetChannel* pkChannel = m_pkReal->GetChannel();
	FvNetBundle& kBundle = pkChannel->Bundle();

	//! 同步spaceData
	if(iOldSpaceID != GetSpaceID())
	{
		kBundle.StartMessage(BaseAppIntInterface::SpaceData);
		kBundle << GetSpaceID() << FvEntityManager::Instance().Address() << SPACE_DATA_MAPPING_KEY_CLIENT_SERVER;
		kBundle << GetSpaceType() << FvMatrix::IDENTITY;
		kBundle.AddBlob(m_pkSpace->SpacePath().data(), m_pkSpace->SpacePath().size());

		kBundle.StartMessage(BaseAppIntInterface::SpaceData);
		kBundle << GetSpaceID() << FvEntityManager::Instance().Address() << SPACE_DATA_TOD_KEY;
		m_pkSpace->SerializeTimeOfDay(kBundle);
	}

	//! 纠正位置
	BaseAppIntInterface::ForcedPositionArgs& kArgs = BaseAppIntInterface::ForcedPositionArgs::start(kBundle);
	kArgs.id		= GetEntityID();
	kArgs.spaceID	= GetSpaceID();
	kArgs.vehicleID	= GetVehicleID();
	kArgs.position	= GetPos();
	kArgs.direction	= GetDir();

	pkChannel->DelayedSend();

	m_bWaitForcePosAck = true;
	OnForcePosition();
}

void FvEntity::TeleportLocally(FvSpaceID iSpaceID, const FvVector3& kPosition, const FvDirection3& kDirection)
{
	FV_ASSERT(IsReal());

	//! 传送前,更新AoI/Trap,处理掉已离开的实体
	if(1)
	{
		if(HasWitness())
			CheckAoI();
		CheckTraps();
	}

	m_kAttrib.SetAttribEventCallBack(NULL);

	OnLeavingCell();

	//! 删除所有Ghost
	m_pkReal->DeleteAllGhosts();

	//! 关闭Traps
	m_pkReal->CloseTrapsBeforeTeleportLocally();

	//! 关闭AoI
	if(HasWitness())
		m_pkReal->pWitness()->CloseAoIBeforeTeleportLocally();

	OnLeftCell();

	m_kPos = kPosition;
	m_kDir = kDirection;

	OnEnteringCell();

	//! 切换Space
	m_pkSpace->DelEntityForTeleport(this);
	FvSpace* pkOldSpace = m_pkSpace;
	m_pkSpace = FvEntityManager::Instance().FindSpace(iSpaceID);
	FV_ASSERT(m_pkSpace);
	m_pkSpace->AddEntityForTeleport(this);

	//! 开启AoI
	if(HasWitness())
		m_pkReal->pWitness()->OpenAoIAfterTeleportLocally();

	//! 开启Traps
	m_pkReal->OpenTrapsAfterTeleportLocally();

	OnEnteredCell();

	m_kAttrib.SetAttribEventCallBack(&m_kAttribEventCallBack);

	//! 传送后处理
	TeleportFinish(pkOldSpace->SpaceID());
}

void FvEntity::SyncPosDir()
{
	//! 同步给观察者
	class Visiter
	{
	public:
		Visiter(FvEntityID iEntityID, FvVector3& kPos, FvDirection3& kDir):m_iEntityID(iEntityID),m_kPos(kPos),m_kDir(kDir) {}
		FvEntityID m_iEntityID;
		FvVector3& m_kPos;
		FvDirection3& m_kDir;

		void OnVisit(void* pkObsData)
		{
			FvEntity* pkEntity = (FvEntity*)pkObsData;
			pkEntity->AvatarUpdateForOtherClient(m_iEntityID, 0, m_kPos, m_kDir, 0);
		}
	};
	Visiter kVisiter(m_iEntityID, m_kPos, m_kDir);

	m_pkSpace->GetAoIMgr().QueryDVision<Visiter, &Visiter::OnVisit>(m_hAoIObject, &kVisiter);

	//! 同步给Ghost
	FV_ASSERT(IsReal());
	m_pkReal->AvatarUpdateToGhosts(m_kPos, m_kDir);
}

void FvEntity::OpenAoI()
{
	FV_ASSERT(m_hAoIObserver == FVAOI_NULL_HANDLE && m_hAoIObject != FVAOI_NULL_HANDLE);
	m_hAoIObserver = m_pkSpace->GetAoIMgr().AddObserver(m_hAoIObject, m_uiAoIObsMask, m_kPos.x, m_kPos.y, m_fAoIVision, m_fAoIDisVisibility, true, this);
}

void FvEntity::CloseAoI()
{
	FV_ASSERT(m_hAoIObserver != FVAOI_NULL_HANDLE);
	m_pkSpace->GetAoIMgr().Remove(m_hAoIObserver);
	m_hAoIObserver = FVAOI_NULL_HANDLE;
}

FvNetChannel* FvEntity::GetRealChannel() const
{
	FV_ASSERT(m_pkReal);
	return m_pkReal->GetChannel();
}

void FvEntity::WritePosDirToStream(FvBinaryOStream& kOS)
{
	FV_ASSERT(m_pkSpace);
	kOS << m_kPos << m_kDir << m_pkSpace->SpaceID() << m_pkSpace->SpaceType();
}

FvUInt32 FvEntity::NewRPCCallBackID()
{
	FV_ASSERT(m_uiRPCCallBackID<0xFFFFFF00);
	m_kRefCnt.AddSysRefCnt();
	if(IsDestroyEntity())
		FvEntityManager::Instance().RemoveDestroyEntity(this);
	return ++m_uiRPCCallBackID;
}

void FvEntity::RealDestroy()
{
	FV_ASSERT(IsReal());
	FV_ASSERT(m_kRefCnt.CanDestroy());

	//! 清除MailBox,这之后不能调用自己的函数
	m_kCellMB.Clear();
	if(m_pkBaseMB)
		m_pkBaseMB->Clear();
	if(m_pkAllClients)
		m_pkAllClients->Clear();
	if(m_pkOtherClients)
		m_pkOtherClients->Clear();

	m_kAttrib.SetAttribEventCallBack(NULL);
	UnInitialize();

	m_pkReal->Destroy();

	FV_ASSERT(m_hAoIObject != FVAOI_NULL_HANDLE);
	m_pkSpace->GetAoIMgr().Remove(m_hAoIObject);
	m_hAoIObject = FVAOI_NULL_HANDLE;
	FV_ASSERT(m_hAoIObserver == FVAOI_NULL_HANDLE);

	m_pkSpace->DelEntity(this);
}

void FvEntity::SendSpaceDataToClient(const FvSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kData)
{
	FV_ASSERT(IsReal());
	if(!HasWitness())
		return;
	FvNetChannel* pkChannel = m_pkReal->GetChannel();
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(BaseAppIntInterface::SpaceData);
	kBundle << GetSpaceID() << kEntryID << uiKey;
	kBundle.AddBlob(kData.c_str(), kData.length());
	pkChannel->DelayedSend();
}

bool FvEntity::GhostUpdateNumberLessThan(GhostUpdateNumber a, GhostUpdateNumber b)
{
	return (( a - b ) & 0xFFFF) > 0x8000;
}

void FvEntity::SetVehicleID(FvEntityID iVehicleID)
{
	FV_ASSERT(!m_pkVehicle && iVehicleID!=FV_NULL_ENTITY || m_pkVehicle && iVehicleID==FV_NULL_ENTITY);
	if(m_pkVehicle)
	{
		m_pkVehicle = NULL;
	}
	else
	{
		m_pkVehicle = FvEntityManager::Instance().FindEntity(iVehicleID);
		FV_ASSERT(m_pkVehicle);
	}
}

void FvEntity::BoardVehicleReq(FvEntityID iPassengerID, const FvNetAddress& kAddr)
{
	if(IsReal())
	{
		m_pkReal->BoardVehicleReq(iPassengerID, kAddr);
	}
	else
	{
		//! FvEntityID iPassengerID
		//! FvNetAddress kAddr

		//! 中转给Real
		FV_ASSERT(m_pkRealChannel);
		FvNetBundle& kBundle = m_pkRealChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::VehicleMsg);
		kBundle << GetEntityID();
		kBundle << FvUInt8(FvRealEntity::VEHICLE_MSGID_BOARD_REQ);
		kBundle << iPassengerID << kAddr;
		m_pkRealChannel->DelayedSend();
	}
}
void FvEntity::BoardVehicleRep(bool bSuccess, FvEntityID iVehicleID)
{
	if(IsReal())
	{
		m_pkReal->BoardVehicleRep(bSuccess, iVehicleID);
	}
	else
	{
		//! bool bSuccess
		//! FvEntityID iVehicleID

		//! 中转给Real
		FV_ASSERT(m_pkRealChannel);
		FvNetBundle& kBundle = m_pkRealChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::VehicleMsg);
		kBundle << GetEntityID();
		kBundle << FvUInt8(FvRealEntity::VEHICLE_MSGID_BOARD_REP);
		kBundle << bSuccess << iVehicleID;
		m_pkRealChannel->DelayedSend();
	}
}
void FvEntity::AlightVehicleReq(FvEntityID iPassengerID)
{
	if(IsReal())
	{
		m_pkReal->AlightVehicleReq(iPassengerID);
	}
	else
	{
		//! FvEntityID iPassengerID

		//! 中转给Real
		FV_ASSERT(m_pkRealChannel);
		FvNetBundle& kBundle = m_pkRealChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::VehicleMsg);
		kBundle << GetEntityID();
		kBundle << FvUInt8(FvRealEntity::VEHICLE_MSGID_ALIGHT);
		kBundle << iPassengerID;
		m_pkRealChannel->DelayedSend();
	}
}
void FvEntity::MoveVehicleReq(FvEntityID iVehicleID, FvBinaryIStream& kData)
{
	if(IsReal())
	{
		m_pkReal->MoveVehicleReq(iVehicleID, kData);
	}
	else
	{
		//! FvEntityID iVehicleID
		//! FvBinaryIStream kData

		//! 中转给Real
		FV_ASSERT(m_pkRealChannel);
		FvNetBundle& kBundle = m_pkRealChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::VehicleMsg);
		kBundle << GetEntityID();
		kBundle << FvUInt8(FvRealEntity::VEHICLE_MSGID_MOVE);
		kBundle << iVehicleID;
		kBundle.AddBlob(kData.Retrieve(0), kData.RemainingLength());
		m_pkRealChannel->DelayedSend();
	}
}
void FvEntity::CloseVehicleReq(FvEntityID iVehicleID)
{
	if(IsReal())
	{
		m_pkReal->CloseVehicleReq(iVehicleID);
	}
	else
	{
		//! FvEntityID iVehicleID

		//! 中转给Real
		FV_ASSERT(m_pkRealChannel);
		FvNetBundle& kBundle = m_pkRealChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::VehicleMsg);
		kBundle << GetEntityID();
		kBundle << FvUInt8(FvRealEntity::VEHICLE_MSGID_CLOSE);
		kBundle << iVehicleID;
		m_pkRealChannel->DelayedSend();
	}
}
void FvEntity::SyncVehicleToObservers()
{
	//! 同步给观察者
	class Visiter
	{
	public:
		Visiter(FvEntityID iPassengerID, FvEntityID iVehicleID):m_iPassengerID(iPassengerID), m_iVehicleID(iVehicleID) {}
		FvEntityID m_iPassengerID;
		FvEntityID m_iVehicleID;

		void OnVisit(void* pkObsData)
		{
			FvEntity* pkEntity = (FvEntity*)pkObsData;
			pkEntity->SetVehicleIDForOtherClient(m_iPassengerID, m_iVehicleID);
		}
	};
	Visiter kVisiter(GetEntityID(), m_pkVehicle ? m_pkVehicle->GetEntityID() : FV_NULL_ENTITY);

	FV_ASSERT(m_hAoIObject != FVAOI_NULL_HANDLE);
	m_pkSpace->GetAoIMgr().QueryDVision<Visiter, &Visiter::OnVisit>(m_hAoIObject, &kVisiter);
}
void FvEntity::SetVehicleIDForOtherClient(FvEntityID iPassengerID, FvEntityID iVehicleID)
{
	FV_ASSERT(IsReal() && HasWitness());

	//! 这个消息必须要发送到客户端,不能丢
	//! 这个消息不用消息cache的方式[如:移动/属性/方法消息],而采用直接放到channel的方式,外加迁移时做检查,以确保这个消息肯定能发送到客户端
	FvNetChannel* pkChannel = m_pkReal->GetChannel();
	BaseAppIntInterface::SetVehicleArgs& kArgs = BaseAppIntInterface::SetVehicleArgs::start(pkChannel->Bundle());
	kArgs.passengerID = iPassengerID;
	kArgs.vehicleID = iVehicleID;
	//pkChannel->Send();
}

void CellAttribEventCallBack::HandleEvent(FvUInt8 uiMessageID, FvBinaryIStream& kIS, bool bOwnCli, bool bGhost, bool bOthersCli, bool bDB)
{
	FV_ASSERT(m_pkEntity && m_pkEntity->IsReal());

	if(bOwnCli)
		m_pkEntity->AddOwnClientPropertyEvent(uiMessageID, kIS);
	if(bGhost)
		m_pkEntity->AddGhostPropertyEvent(uiMessageID, kIS, bOthersCli);
}

FvEntity::GhostEventCache::GhostEventCache(GhostUpdateNumber ghostNumber, FvUInt8 msgID, const void* pcData, int len)
:uiGhostUpdateNumber(ghostNumber),uiMessageID(msgID),iGhostDataLen(len),uiLastEventNumber(0),pkEvent(NULL)
{
	FV_ASSERT(len>0 && pcData);
	pcGhostData = new char[len];
	memcpy(pcGhostData, pcData, len);
}

FvEntity::GhostEventCache::GhostEventCache(GhostUpdateNumber ghostNumber, FvUInt8 msgID, const void* pcData, int len, FvEventNumber eventNumber, FvHistoryEvent* pkEvent_)
:uiGhostUpdateNumber(ghostNumber),uiMessageID(msgID),iGhostDataLen(len),uiLastEventNumber(eventNumber),pkEvent(pkEvent_)
{
	FV_ASSERT(len>0 && pcData);
	pcGhostData = new char[len];
	memcpy(pcGhostData, pcData, len);
}

FvEntity::GhostEventCache::~GhostEventCache()
{
	if(pcGhostData)
	{
		delete [] pcGhostData;
		pcGhostData = NULL;
	}
	FV_ASSERT(!pkEvent);
}

