#include <FvDebug.h>
#include "FvRobotClientApp.h"
#include "FvRobotMainApp.h"
#include "FvRobotMovementController.h"
#include "FvRobotSpaceDataManager.h"
#include <FvBaseAppExtInterface.h>
#include <FvEntityExport.h>
#include "FvRobotEntity.h"
#include "FvRobotCfg.h"
#include <FvSpaceDataTypes.h>


FV_DECLARE_DEBUG_COMPONENT2( "Bots", 0 )

FvRobotClientApp::FvRobotClientApp(FvNetNub& kMainNub, int iRobotID, const FvString& kTag, const FvString& kName, const FvString& kPassword, const FvVector3& kBirthPos)
:m_kServerConnection()
,m_iRobotID(iRobotID)
,m_kTag(kTag)
,m_iSpaceID(0)
,m_uiSpaceType(0xFFFF)
,m_iCtrlEntityID(0)
,m_iVehicleID(0)
,m_pkCtrlEntity(NULL)
,m_spLoginInProgress(NULL)
,m_bIsDestroyed(false)
,m_bIsDormant(true)
,m_kMainNub(kMainNub)
,m_kUserName(kName)
,m_kUserPasswd(kPassword)
,m_kBirthPos(kBirthPos)
,m_pkMoveCtrller(NULL)
,m_uiUpdateMoveTime(0)
,m_uiUpdateMoveInterval(100)
,m_uiPingPongTime(0)
,m_uiPingPongInterval(15000)
{
	m_pkServerToD = new FvTimeOfDay();
	m_kMainNub.RegisterChildNub(&m_kServerConnection.GetNub(), this);

	SetConnectionLossRatio(FvRobotCfg::Instance().GetPacketLossRatio());
	SetConnectionLatency(FvRobotCfg::Instance().GetMinNetworkLatency(), FvRobotCfg::Instance().GetMaxNetworkLatency());

	this->LogOn();
}

FvRobotClientApp::~FvRobotClientApp()
{
	if(!m_bIsDestroyed)
		this->Destroy();

	delete m_pkServerToD;
}

bool FvRobotClientApp::Tick(float fTime)
{
	if(m_bIsDormant)
		return true;

	m_kServerConnection.ProcessInput();

	if(m_spLoginInProgress != NULL)
	{
		if(m_spLoginInProgress->Done())
		{
			FvLogOnStatus status =
				m_kServerConnection.LogOnComplete(m_spLoginInProgress, this);

			m_spLoginInProgress = NULL;

			if(!status.Succeeded())
			{
				FV_ERROR_MSG( "LogOn failed (%s)\n",
					m_kServerConnection.ErrorMsg().c_str() );
				return false;
			}
			else
			{
				if(m_kServerConnection.Online())
				{
					m_kServerConnection.EnableEntities();
				}
			}
		}
		return true;
	}
	else
	{
		if(!m_kServerConnection.Online())
			return false;

		if(fTime > 0.f)
		{
			if(m_iSpaceID && FvRobotCfg::Instance().GetMoveType() == 2)
			{
				if(m_pkCtrlEntity && m_pkMoveCtrller)
					m_pkMoveCtrller->NextStep(m_pkCtrlEntity->GetSpeed(), fTime, m_pkCtrlEntity->GetPos(), m_pkCtrlEntity->GetDir());
			}

			if(m_pkCtrlEntity)
				m_pkCtrlEntity->Tick(fTime);

			FvUInt32 nTime = GetTickCount();
			if(nTime > m_uiUpdateMoveTime + m_uiUpdateMoveInterval)
			{
				m_uiUpdateMoveTime = nTime;

				if(nTime > m_uiPingPongTime + m_uiPingPongInterval)
				{
					m_uiPingPongTime = nTime;
					m_kServerConnection.GetBundle().Reliable( FV_NET_RELIABLE_DRIVER );
				}

				if(m_pkCtrlEntity && m_pkCtrlEntity->GetSpaceID())
				{
					BaseAppExtInterface::AvatarUpdateImplicitArgs & upArgs =
						BaseAppExtInterface::AvatarUpdateImplicitArgs::start(
						m_kServerConnection.GetBundle());

					upArgs.pos = m_pkCtrlEntity->GetPos();
					upArgs.dir = m_pkCtrlEntity->GetDir();
					upArgs.refNum = 0;
				}
			}

			m_kServerConnection.Send();
		}
	}
	return true;
}

void FvRobotClientApp::LogOn()
{
	if (m_bIsDestroyed)
		return;

	m_bIsDormant = false;

	if ((m_spLoginInProgress != NULL) || m_kServerConnection.Online())
		return;

	FvRobotMainApp& kApp = FvRobotMainApp::Instance();

	m_kServerConnection.Digest(kApp.GetDigest());

	FV_TRACE_MSG("Connecting to server at %s\n", FvRobotCfg::Instance().GetServerName().c_str());

	m_spLoginInProgress = m_kServerConnection.LogOnBegin(
		FvRobotCfg::Instance().GetServerName().c_str(),
		m_kUserName.c_str(),
		m_kUserPasswd.c_str(),
		FvRobotCfg::Instance().GetPublicKey().c_str());
}

void FvRobotClientApp::LogOff()
{
	if (m_kServerConnection.Online())
	{
		m_kServerConnection.Send(); 
		m_kServerConnection.Disconnect();
		m_kMainNub.DeregisterFileDescriptor( m_kServerConnection.GetNub().Socket() );
	}
}

void FvRobotClientApp::DropConnection()
{
	if (m_kServerConnection.Online())
	{
		m_kServerConnection.Disconnect( false );
		m_kMainNub.DeregisterFileDescriptor( m_kServerConnection.GetNub().Socket() );
	}
}

void FvRobotClientApp::SetConnectionLossRatio(float fLossRatio)
{
	if(fLossRatio < 0.0 || fLossRatio > 1.0)
	{
		return;
	}
	m_kServerConnection.GetNub().SetLossRatio(fLossRatio);
}

void FvRobotClientApp::SetConnectionLatency(float fLatencyMin, float fLatencyMax)
{
	if(fLatencyMin >= fLatencyMax)
	{
		return;
	}
	m_kServerConnection.GetNub().SetLatency(fLatencyMin, fLatencyMax);
}

bool FvRobotClientApp::SetMovementController(const FvString& kType, const FvString& kData)
{
	if(m_bIsDestroyed)
		return false;

	if(!m_pkCtrlEntity)
		return false;

	FvRobotMovementController* pNewController =
		FvRobotMainApp::Instance().CreateMovementController(m_pkCtrlEntity->GetSpeed(), m_pkCtrlEntity->GetPos(), kType, kData);

	if(!pNewController)
		return false;

	if(m_pkMoveCtrller)
		delete m_pkMoveCtrller;

	m_pkMoveCtrller = pNewController;
	return true;
}

void FvRobotClientApp::Destroy()
{
	if(!m_bIsDestroyed)
	{
		m_bIsDestroyed = true;

		if(m_kServerConnection.Online())
		{
			this->LogOff();
		}

		ClearEntities();

		m_pkCtrlEntity = NULL;

		m_kMainNub.DeregisterChildNub( &m_kServerConnection.GetNub() );

		if(m_pkMoveCtrller)
		{
			delete m_pkMoveCtrller;
			m_pkMoveCtrller = NULL;
		}
	}
}

void FvRobotClientApp::OnKeyDown(int keyVal, bool bRep)
{
	if(!m_pkCtrlEntity)
		return;

	if(m_iSpaceID &&
		FvRobotCfg::Instance().GetMoveType() == 1)
	{
		const FvRobotMainApp::SpaceInfo* pkSpaceInfo = FvRobotMainApp::Instance().FindSpaceInfo(m_uiSpaceType);
		if(pkSpaceInfo)
		{
			FvVector3& kPos = m_pkCtrlEntity->GetPos();
			int iSpd = FvRobotCfg::Instance().GetMoveSpeed();
			if(keyVal == KV_a)
			{
				kPos.x -= iSpd;
			}
			else if(keyVal == KV_d)
			{
				kPos.x += iSpd;
			}
			else if(keyVal == KV_s)
			{
				kPos.y -= iSpd;
			}
			else if(keyVal == KV_w)
			{
				kPos.y += iSpd;
			}

			float fOffset(1);
			if(kPos.x < pkSpaceInfo->kSpaceRect.x + fOffset)
				kPos.x = pkSpaceInfo->kSpaceRect.x + fOffset;
			else if(kPos.x > pkSpaceInfo->kSpaceRect.y - fOffset)
				kPos.x = pkSpaceInfo->kSpaceRect.y - fOffset;
			else if(kPos.y < pkSpaceInfo->kSpaceRect.z + fOffset)
				kPos.y = pkSpaceInfo->kSpaceRect.z + fOffset;
			else if(kPos.y > pkSpaceInfo->kSpaceRect.w - fOffset)
				kPos.y = pkSpaceInfo->kSpaceRect.w - fOffset;
		}
	}

	m_pkCtrlEntity->OnKeyDown(keyVal, bRep);
}

void FvRobotClientApp::OnKeyUp(int keyVal)
{
	if(!m_pkCtrlEntity)
		return;

	m_pkCtrlEntity->OnKeyUp(keyVal);
}

double FvRobotClientApp::GetToD()
{
	return m_pkServerToD->GetDayTime(m_kServerConnection.ServerTime());
}

float FvRobotClientApp::GetToDScaling() const
{
	return m_pkServerToD->GetTimeScaling();
}

FvEntity* FvRobotClientApp::FindEntity( FvEntityID id )
{
	EntityMap::iterator iter = m_kEntityMap.find( id );
	if (iter != m_kEntityMap.end())
		return iter->second;

	return NULL;
}

void FvRobotClientApp::OnBasePlayerCreate( FvEntityID id, FvEntityTypeID type,
	FvBinaryIStream & data )
{
	FV_ASSERT(!m_pkCtrlEntity);

	FvAllRobotEntityExports* pkEntityExports = FvRobotMainApp::Instance().GetEntityExports();
	FV_ASSERT(pkEntityExports && type<pkEntityExports->uiEntityCnt && pkEntityExports->ppkEntityExport[type]);
	FvRobotEntityExport* pkExport = pkEntityExports->ppkEntityExport[type];
	m_pkCtrlEntity = pkExport->pFunCreateEntity();
	m_pkCtrlEntity->InitBase(pkExport, &m_kServerConnection, id, type, data, m_iRobotID);

	if(!pkExport->kForm.bHasCell)
	{
		m_pkCtrlEntity->SetClientApp(this);
		m_pkCtrlEntity->Initialize();
		m_kEntityMap.insert(std::make_pair(id,m_pkCtrlEntity));
	}

	m_iCtrlEntityID = id;

	SetMovementController(FvRobotCfg::Instance().GetCtrllerType(), FvRobotCfg::Instance().GetCtrllerData());
}

void FvRobotClientApp::OnCellPlayerCreate( FvEntityID id,
	FvSpaceID spaceID, FvEntityID vehicleID, const FvPosition3D & pos,
	float yaw, float pitch,	float roll,
	FvBinaryIStream & data )
{
	FV_ASSERT(m_pkCtrlEntity && m_pkCtrlEntity->GetAttrib().GetExport()->kForm.bHasCell);
	FV_ASSERT(m_iCtrlEntityID==id && !m_iSpaceID && spaceID);
	m_iSpaceID = spaceID;

	m_pkCtrlEntity->InitCell(spaceID, vehicleID, pos, yaw, pitch, roll, data);
	m_pkCtrlEntity->SetClientApp(this);
	m_pkCtrlEntity->Initialize();
	m_kEntityMap.insert(std::make_pair(id,m_pkCtrlEntity));
	m_pkCtrlEntity->EnterWorld( spaceID, vehicleID, false );
	FvRobotMainApp::Instance().AddEntity(m_pkCtrlEntity);

	//! Robot帧率低,设为非频繁模式
	m_kServerConnection.Channel().IsIrregular(true);
}

void FvRobotClientApp::OnEntityEnter( FvEntityID id, FvSpaceID spaceID, FvEntityID )
{

}

void FvRobotClientApp::OnEntityLeave( FvEntityID id, const FvCacheStamps & stamps )
{
	EntityMap::iterator itr = m_kEntityMap.find(id);
	if(itr != m_kEntityMap.end())
	{
		FvEntity* pEntity = itr->second;
		FV_ASSERT(pEntity && pEntity!=m_pkCtrlEntity);
		FvRobotMainApp::Instance().RemoveEntity(pEntity);
		pEntity->LeaveWorld( false );
		pEntity->UnInitialize();
		delete pEntity;
		m_kEntityMap.erase(itr);
	}
}

void FvRobotClientApp::OnEntityCreate( FvEntityID id, FvEntityTypeID type,
	FvSpaceID spaceID, FvEntityID vehicleID, const FvPosition3D & pos,
	float yaw, float pitch,	float roll,
	FvBinaryIStream & data )
{
	FvAllRobotEntityExports* pkEntityExports = FvRobotMainApp::Instance().GetEntityExports();
	FV_ASSERT(pkEntityExports && type<pkEntityExports->uiEntityCnt && pkEntityExports->ppkEntityExport[type]);
	FvRobotEntityExport* pkExport = pkEntityExports->ppkEntityExport[type];
	FvEntity* pEntity = pkExport->pFunCreateEntity();
	pEntity->InitEntity(pkExport, &m_kServerConnection, id, type, spaceID, vehicleID, pos, yaw, pitch, roll, data);
	pEntity->SetClientApp(this);
	pEntity->Initialize();
	m_kEntityMap.insert(std::make_pair(id,pEntity));
	pEntity->EnterWorld( spaceID, vehicleID, false );
	FvRobotMainApp::Instance().AddEntity(pEntity);
}

void FvRobotClientApp::OnEntityProperties( FvEntityID id, FvBinaryIStream & data )
{
	FvEntity* pEntity = FindEntity(id);
	if(pEntity)
	{
		pEntity->OnEntityPropertys(data);
	}
}

void FvRobotClientApp::OnEntityProperty( FvEntityID entityID, int propertyID,
	FvBinaryIStream & data )
{
	FvEntity* pEntity = FindEntity(entityID);
	if(pEntity)
	{
		pEntity->OnEntityProperty(propertyID, data);
	}
	else
	{
		FV_ERROR_MSG("%s, Can't find Entity:%d\n", __FUNCTION__, entityID);
		data.Finish();
	}
}

void FvRobotClientApp::OnEntityMethod( FvEntityID entityID, int methodID,
	FvBinaryIStream & data )
{
	FvEntity* pEntity = FindEntity(entityID);
	if(pEntity)
	{
		pEntity->OnEntityMethod(methodID, data);
	}
	else
	{
		FV_ERROR_MSG("%s, Can't find Entity:%d\n", __FUNCTION__, entityID);
		data.Finish();
	}
}

void FvRobotClientApp::OnEntityMove( FvEntityID entityID, FvSpaceID spaceID, FvEntityID vehicleID,
	const FvPosition3D & pos,	float yaw, float pitch, float roll,
	bool isVolatile )
{
	FvEntity* pkEntity = FindEntity(entityID);
	FV_ASSERT(pkEntity);

	if(spaceID != pkEntity->GetSpaceID())
	{
		FvRobotMainApp::Instance().RemoveEntity(pkEntity);
		pkEntity->SetSpaceID(spaceID);
		FvRobotMainApp::Instance().AddEntity(pkEntity);
	}

	pkEntity->SetPos(pos);
}

void FvRobotClientApp::SetTime( FvTimeStamp gameTime,
		float initialTimeOfDay, float gameSecondsPerSecond )
{

}

void FvRobotClientApp::SpaceData( FvSpaceID spaceID, FvSpaceEntryID& entryID, FvUInt16 key,
	const FvString & data )
{
	FvRobotSpaceData * pSpace = FvRobotSpaceDataManager::Instance().FindOrAddSpaceData( spaceID );

	if (!pSpace)
		return;

	FvRobotSpaceData::EntryStatus entryStat = pSpace->DataEntry( entryID, key, data );

	if (key == SPACE_DATA_TOD_KEY)
	{
		if(key == SPACE_DATA_TOD_KEY)
		{
			FvMemoryOStream kOS;
			kOS.AddBlob(data.c_str(), data.length());
			kOS >> *m_pkServerToD;
		}
	}
	else if (key == SPACE_DATA_MAPPING_KEY_CLIENT_SERVER ||
			 key == SPACE_DATA_MAPPING_KEY_CLIENT_ONLY)
	{
		m_uiSpaceType = *(FvUInt16*)data.data();
		FvRobotMainApp::Instance().SetSpaceType(spaceID, m_uiSpaceType);
	}
	else
	{
		if (entryStat == FvRobotSpaceData::DATA_ADDED)
		{
		}
		else if (entryStat == FvRobotSpaceData::DATA_DELETED)
		{
		}
		else if (entryStat == FvRobotSpaceData::DATA_MODIFIED)
		{
		}
	}
}

void FvRobotClientApp::SpaceGone( FvSpaceID spaceID )
{

}

void FvRobotClientApp::OnProxyData( FvUInt16 proxyDataID, FvBinaryIStream & data )
{

}

void FvRobotClientApp::OnEntitiesReset( bool keepPlayerOnBase )
{
	if(keepPlayerOnBase)
	{
		EntityMap::iterator itr = m_kEntityMap.begin();
		while(itr != m_kEntityMap.end())
		{
			FvEntity* pEntity = itr->second;
			if(pEntity != m_pkCtrlEntity)
			{
				FvRobotMainApp::Instance().RemoveEntity(pEntity);
				pEntity->LeaveWorld( false );
				pEntity->UnInitialize();
				delete pEntity;
			}
			++itr;
		}
		m_kEntityMap.clear();
		m_kEntityMap.insert(std::make_pair(m_iCtrlEntityID,m_pkCtrlEntity));
	}
	else
	{
		EntityMap::iterator itr = m_kEntityMap.begin();
		while(itr != m_kEntityMap.end())
		{
			FvEntity* pEntity = itr->second;
			if(pEntity->GetSpaceID())
			{
				FvRobotMainApp::Instance().RemoveEntity(pEntity);
				pEntity->LeaveWorld(false);
			}
			pEntity->UnInitialize();
			delete pEntity;
			++itr;
		}
		m_kEntityMap.clear();
		m_pkCtrlEntity = NULL;
		m_iCtrlEntityID = -1;
	}

	m_iSpaceID = 0;
	m_uiSpaceType = 0xFFFF;
}

void FvRobotClientApp::OnStreamComplete( FvUInt16 id, const FvString &desc,
	FvBinaryIStream &data )
{

}

int FvRobotClientApp::HandleInputNotification( int )
{
	Tick(0.0f);

	return 0;
}

void FvRobotClientApp::ClearEntities()
{
	EntityMap::iterator itrB,itrE;
	itrB = m_kEntityMap.begin();
	itrE = m_kEntityMap.end();
	while(itrB != itrE)
	{
		FvEntity* pkEntity = itrB->second;
		FV_ASSERT(pkEntity);
		if(pkEntity->GetSpaceID())
			FvRobotMainApp::Instance().RemoveEntity(pkEntity);
		pkEntity->UnInitialize();
		delete pkEntity;
		++itrB;
	}
	m_kEntityMap.clear();
}



