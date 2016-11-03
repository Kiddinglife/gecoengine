#include "FvWitness.h"
#include <../FvBase/FvBaseAppIntInterface.h>
#include "FvCellEntityManager.h"
#include "FvSpaceDataTypes.h"
#include "FvSpace.h"


FvWitness::FvWitness(FvRealEntity& owner, FvBinaryIStream& data, CreateRealInfo createWitnessInfo, bool hasChangedSpace)
:m_kReal(owner),m_kEntity(owner.Entity())
{
	if(createWitnessInfo == CREATE_REAL_FROM_INIT)
	{
		FvNetChannel* pkChannel = m_kReal.GetChannel();
		FvNetBundle& kBundle = pkChannel->Bundle();

		int s1 = kBundle.Size();
		int p1 = kBundle.SizeInPackets();
		int n1 = kBundle.NumMessages();

		//! 发送创建Cell包给client
		//! FvSpaceID iSpaceID;
		//! FvEntityID vehicleID;
		//! FvPosition3D pos;
		//! FvDirection3 dir;
		//! Cell和client公有数据
		kBundle.StartMessage(BaseAppIntInterface::CreateCellPlayer);
		kBundle << m_kEntity.GetSpaceID() << m_kEntity.GetVehicleID() << m_kEntity.GetPos() << m_kEntity.GetDir();
		m_kEntity.GetAttrib().SerializeToStreamForOwnCliData(kBundle);

		int s2 = kBundle.Size();
		int p2 = kBundle.SizeInPackets();
		int n2 = kBundle.NumMessages();
		FV_INFO_MSG("CreateCellPk, ID:%d, Size:%d, Packet:%d, Message:%d\n", m_kEntity.GetEntityID(), s2-s1, p2-p1, n2-n1);

		//! 发送SpaceData包给client
		kBundle.StartMessage(BaseAppIntInterface::SpaceData);
		kBundle << m_kEntity.GetSpaceID() << FvEntityManager::Instance().Address() << SPACE_DATA_MAPPING_KEY_CLIENT_SERVER;
		kBundle << m_kEntity.GetSpaceType() << FvMatrix::IDENTITY;
		const FvString& kSpacePath = m_kEntity.GetSpace()->SpacePath();
		kBundle.AddBlob(kSpacePath.data(), kSpacePath.size());

		kBundle.StartMessage(BaseAppIntInterface::SpaceData);
		kBundle << m_kEntity.GetSpaceID() << FvEntityManager::Instance().Address() << SPACE_DATA_TOD_KEY;
		m_kEntity.GetSpace()->SerializeTimeOfDay(kBundle);

		pkChannel->Send();
	}
	else if(createWitnessInfo == CREATE_REAL_FROM_OFFLOAD)
	{
		FvUInt16 uiCnt;
		data >> uiCnt;

		struct AoICacheExt
		{
			AoICache	kAoICache;
			FvEntityID	iVehicleID;
		};
		typedef std::vector<AoICacheExt> AoICacheList;
		static AoICacheList s_OldCaches;

		s_OldCaches.clear();
		FvEntity::AddGlobalDummy();

		//! 恢复Aoi
		for(FvUInt16 i=0; i<uiCnt; ++i)
		{
			FV_ASSERT(!data.Error());
			AoICacheExt kCache;
			FvEntityID iEntityID;
			kCache.kAoICache.SteamIn(data, iEntityID, kCache.iVehicleID);
			FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(iEntityID);
			if(pkEntity)
			{
				if(pkEntity->GetSpaceID() == m_kEntity.GetSpaceID())
				{
					pkEntity->SetDummy(FvEntity::GetGlobalDummy());
					kCache.kAoICache.m_spEntity = pkEntity;
					s_OldCaches.push_back(kCache);
				}
				else
				{
					//! 发送离开Aoi消息
					m_kEntity.OnLeaveAoi(*pkEntity);
					kCache.kAoICache.SendLeaveAoi(m_kReal.GetChannel(), iEntityID);
				}
			}
			else
			{
				//! 发送离开Aoi消息
				m_kEntity.OnLeaveAoiID(iEntityID);
				kCache.kAoICache.SendLeaveAoi(m_kReal.GetChannel(), iEntityID);
			}
		}
		FV_ASSERT(!data.Error());

		const FvVector3& kPos = m_kEntity.GetPos();
		FvAoIHandle hHandle = m_kEntity.GetSpace()->GetAoIMgr().AddObserver(m_kEntity.GetAoIObjHandle(), m_kEntity.GetAoIObsMask(), kPos.x, kPos.y, m_kEntity.GetAoIVision(), m_kEntity.GetAoIDisVisibility(), true, &m_kEntity);
		m_kEntity.SetAoIObsHandle(hHandle);

		class Listener
		{
		public:
			Listener(FvEntity* pkEntity, FvUInt64 uiDummy):m_pkEntity(pkEntity),m_uiDummy(uiDummy) {}
			FvEntity* m_pkEntity;
			FvUInt64 m_uiDummy;

			void OnEnter(void* pkObsData, FvAoIExt* pkExt)
			{
				AoICache* pkCache = (AoICache*)pkExt;
				if(pkCache->m_spEntity->GetDummy() != m_uiDummy)
				{
					//! 进入AoI
					pkCache->InAoI(m_pkEntity);
				}
				else
				{
					//! Copy老的数据
					bool bFind(false);
					for(int i=0; i<(int)s_OldCaches.size(); ++i)
					{
						if(s_OldCaches[i].kAoICache.m_spEntity == pkCache->m_spEntity)
						{
							*pkCache = s_OldCaches[i].kAoICache;
							bFind = true;
							break;
						}
					}
					FV_ASSERT(bFind);
				}
				pkCache->m_spEntity->SetDummy(m_uiDummy +1);
			}

			void OnStand(void* pkObsData, FvAoIExt* pkExt)
			{
				FV_ASSERT(0);
			}

			void OnLeave(void* pkObsData, FvAoIExt* pkExt)
			{
				FV_ASSERT(0);
			}
		};
		Listener kListener(&m_kEntity, FvEntity::GetGlobalDummy());

		FvEntity::AddGlobalDummy();

#ifndef FV_DEBUG
		m_kEntity.GetSpace()->GetAoIMgr().UpdateAoI<Listener,
			&Listener::OnEnter,
			&Listener::OnStand,
			&Listener::OnLeave>(hHandle, &kListener);
#else
		FvUInt64 a,b,c;
		m_kEntity.GetSpace()->GetAoIMgr().UpdateAoI<Listener,
			&Listener::OnEnter,
			&Listener::OnStand,
			&Listener::OnLeave>(hHandle, &kListener, a, b, c);
#endif

		for(int i=0; i<(int)s_OldCaches.size(); ++i)
		{
			if(s_OldCaches[i].kAoICache.m_spEntity->GetDummy() != FvEntity::GetGlobalDummy())
			{
				//! 离开AoI
				s_OldCaches[i].kAoICache.OutAoI(&m_kEntity);
			}
			else
			{
				//! 判断Vehicle的ID,防止遗漏上/下载具消息
				FvEntityID iNowVehicleID = s_OldCaches[i].kAoICache.m_spEntity->GetVehicleID();
				if(s_OldCaches[i].iVehicleID != iNowVehicleID)
				{
					FvNetChannel* pkChannel = m_kReal.GetChannel();
					BaseAppIntInterface::SetVehicleArgs& kArgs = BaseAppIntInterface::SetVehicleArgs::start(pkChannel->Bundle());
					kArgs.passengerID = s_OldCaches[i].kAoICache.m_spEntity->GetEntityID();
					kArgs.vehicleID = iNowVehicleID;
				}
			}
		}
	}
}

FvWitness::~FvWitness()
{

}

void FvWitness::OpenAoIAfterTeleportLocally()
{
	FV_ASSERT(m_kEntity.GetAoIObjHandle() == FVAOI_NULL_HANDLE &&
		m_kEntity.GetAoIObsHandle() == FVAOI_NULL_HANDLE);
	const FvVector3& kPos = m_kEntity.GetPos();
	FvAoIHandle hHandle;
	hHandle = m_kEntity.GetSpace()->GetAoIMgr().AddObject(m_kEntity.GetAoIObjMask(), kPos.x, kPos.y, m_kEntity.GetAoIVisibility(), &m_kEntity);
	m_kEntity.SetAoIObjHandle(hHandle);

	hHandle = m_kEntity.GetSpace()->GetAoIMgr().AddObserver(m_kEntity.GetAoIObjHandle(), m_kEntity.GetAoIObsMask(), kPos.x, kPos.y, m_kEntity.GetAoIVision(), m_kEntity.GetAoIDisVisibility(), true, &m_kEntity);
	m_kEntity.SetAoIObsHandle(hHandle);
}

void FvWitness::CloseAoIBeforeTeleportLocally()
{
	//! 关闭AoI,通知客户端,视野中的实体离开
	class Visiter
	{
	public:
		Visiter(FvEntity& kEntity):m_kEntity(kEntity) {}
		FvEntity&	m_kEntity;

		void OnVisit(void* pkObjData)
		{
			FV_ASSERT(pkObjData);
			m_kEntity.OnLeaveAoi(*(FvEntity*)pkObjData);
		}
	};
	Visiter kVisiter(m_kEntity);

	m_kEntity.GetSpace()->GetAoIMgr().QueryVision<Visiter, &Visiter::OnVisit>(m_kEntity.GetAoIObsHandle(), &kVisiter);
	m_kEntity.GetSpace()->GetAoIMgr().Remove(m_kEntity.GetAoIObsHandle());
	m_kEntity.SetAoIObsHandle(FVAOI_NULL_HANDLE);

	m_kEntity.GetSpace()->GetAoIMgr().Remove(m_kEntity.GetAoIObjHandle());
	m_kEntity.SetAoIObjHandle(FVAOI_NULL_HANDLE);
}

void FvWitness::Offload(FvBinaryOStream& kStream)
{
	int iEntityCntPos = kStream.Size();
	kStream.Reserve(sizeof(FvUInt16));

	class Visiter
	{
	public:
		Visiter(FvBinaryOStream& kStream):m_kStream(kStream),m_uiCnt(0) {}
		FvBinaryOStream& m_kStream;
		FvUInt16 m_uiCnt;

		void OnVisit(FvAoIExt* pkExt)
		{
			AoICache* pkCache = (AoICache*)pkExt;
			pkCache->SteamOut(m_kStream);
			++m_uiCnt;
		}
	};
	Visiter kVisiter(kStream);

	m_kEntity.GetSpace()->GetAoIMgr().QueryVisionExt<Visiter, &Visiter::OnVisit>(m_kEntity.GetAoIObsHandle(), &kVisiter);
	m_kEntity.GetSpace()->GetAoIMgr().Remove(m_kEntity.GetAoIObsHandle());
	m_kEntity.SetAoIObsHandle(FVAOI_NULL_HANDLE);

	//! kStream必须为FvMemoryOStream
	*(FvUInt16*)(((char*)((FvMemoryOStream&)kStream).Data()) + iEntityCntPos) = kVisiter.m_uiCnt;
}

void FvWitness::CheckEventSync()
{
	class Visiter
	{
	public:
		Visiter(FvEntity* pkEntity):m_pkEntity(pkEntity){}
		FvEntity* m_pkEntity;

		void OnVisit(FvAoIExt* pkExt)
		{
			AoICache* pkCache = (AoICache*)pkExt;
			pkCache->EventUpdate(m_pkEntity);
		}
	};
	Visiter kVisiter(&m_kEntity);

	m_kEntity.GetSpace()->GetAoIMgr().QueryVisionExt<Visiter, &Visiter::OnVisit>(m_kEntity.GetAoIObsHandle(), &kVisiter);
}





