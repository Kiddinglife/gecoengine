#include "FvCellEntityManager.h"
#include <FvServerResource.h>
#include <FvEntityDefConstants.h>
#include <FvLogicDllManager.h>
#include "FvCellAppInterface.h"
#include <../FvCellAppManager/FvCellAppManagerInterface.h>
#include <../FvBase/FvBaseAppIntInterface.h>
#include <../FvDBManager/FvDBInterface.h>
#include <../FvGlobal/FvGlobalAppInterface.h>
#include "FvSpace.h"
#include "FvCell.h"
#include <FvBSPProxy.h>
#include <FvZoneSpace.h>
#include <FvZoneManager.h>
#include <FvZoneSerializer.h>
#include <FvUniqueID.h>
#include <FvTimeKeeper.h>
#include <FvServerConfig.h>
#include <FvEntityExport.h>
#include "FvCellRPCCallBack.h"
#include <set>


//#define __DEV_MODE__	//! 定义开发模式,可以只重启Base/Cell


void IntSignalHandler( int /*sigNum*/ )
{
	FvEntityManager * pApp = FvEntityManager::pInstance();

	if (pApp != NULL)
	{
		//pApp->GetIdxChannelFinder().Dump();
		CellAppInterface::ShutDownArgs args;
		args.isSigInt = true;
		pApp->ShutDown(args);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityChannelFinder::EntityChannelFinder(FvNetNub& nub_)
:m_kNub(nub_)
{

}

EntityChannelFinder::~EntityChannelFinder()
{
	IdxChannelMap::iterator itrB = m_kChannelMap.begin();
	IdxChannelMap::iterator itrE = m_kChannelMap.end();
	while(itrB != itrE)
	{
		itrB->second->Condemn();
		++itrB;
	}
	m_kChannelMap.clear();
}

FvNetChannel* EntityChannelFinder::Find( FvNetChannelID id, const FvNetPacket * pPacket, bool & rHasBeenHandled )
{
	IdxChannelMap::iterator itr = m_kChannelMap.find(id);
	if(itr != m_kChannelMap.end())
	{
		return itr->second;
	}
	else
	{
		TransportChannelMap::iterator itr = m_kTransportChannelMap.find(id);
		if(itr != m_kTransportChannelMap.end())
		{
			//FV_INFO_MSG("ForwardedBaseEntityPacket Out:%s\n", itr->second->m_kTransportSrcAddr.c_str());
			FvNetChannel* pCellChannel = m_kNub.FindChannel(itr->second, true);
			pCellChannel->Bundle().StartMessage(CellAppInterface::ForwardedBaseEntityPacket);
			pCellChannel->Bundle() << id;
			pCellChannel->Bundle().AddBlob(pPacket->Data(), pPacket->TotalSize());
			pCellChannel->DelayedSend();
			rHasBeenHandled = true;
		}

		return NULL;
	}
}

FvNetChannel* EntityChannelFinder::Find(FvNetChannelID id)
{
	IdxChannelMap::iterator itr = m_kChannelMap.find(id);
	if(itr != m_kChannelMap.end())
		return itr->second;
	else
		return NULL;
}

FvNetChannel* EntityChannelFinder::MakeChannel(FvNetChannelID idx, const FvNetAddress& kBaseAddr)
{
	FvNetChannel* pChannel = new FvNetChannel( m_kNub, kBaseAddr, FvNetChannel::INTERNAL, 1.0f, NULL, idx);
	pChannel->IsIrregular(true);
	bool bRet = m_kChannelMap.insert(std::make_pair(idx, pChannel)).second;
	FV_ASSERT(bRet);
	return pChannel;
}

FvNetChannel* EntityChannelFinder::TransportIn(FvNetChannelID idx, const FvNetAddress& kBaseAddr, FvBinaryIStream& stream)
{
	FvNetChannel* pChannel = new FvNetChannel( m_kNub, kBaseAddr, FvNetChannel::INTERNAL, 1.0f, NULL, idx);
	pChannel->InitFromStream(stream, kBaseAddr);
	pChannel->IsIrregular(true);
	bool bRet = m_kChannelMap.insert(std::make_pair(idx, pChannel)).second;
	FV_ASSERT(bRet);
	return pChannel;
}

bool EntityChannelFinder::TransportOut(FvNetChannelID id, const FvNetAddress& des, FvBinaryOStream& stream)
{
	IdxChannelMap::iterator itr = m_kChannelMap.find(id);
	if(itr != m_kChannelMap.end())
	{
		FvNetChannel* pkChannel = itr->second;

		pkChannel->AddToStream(stream);

		m_kTransportChannelMap[id] = des;

		itr->second->Destroy();
		m_kChannelMap.erase(itr);

		return true;
	}

	return false;
}

void EntityChannelFinder::DeleteChannel(FvNetChannelID id)
{
	IdxChannelMap::iterator itr = m_kChannelMap.find(id);
	if(itr != m_kChannelMap.end())
	{
		itr->second->Condemn();
		m_kChannelMap.erase(itr);
	}

	m_kTransportChannelMap.erase(id);
}

FvNetAddress* EntityChannelFinder::FindAddr(FvEntityID id)
{
	TransportChannelMap::iterator itr = m_kTransportChannelMap.find(id);
	if(itr != m_kTransportChannelMap.end())
		return &(itr->second);
	else
		return NULL;
}

void EntityChannelFinder::Dump()
{
	//! dump channels
	FvXMLSectionPtr spSection = FvXMLSection::CreateSection("root");
	FvXMLSectionPtr spChannel;

	IdxChannelMap::iterator itrB = m_kChannelMap.begin();
	IdxChannelMap::iterator itrE = m_kChannelMap.end();
	while(itrB != itrE)
	{
		char channelID[16] = {0};
		sprintf_s(channelID, sizeof(channelID), "%d", itrB->first);
		spChannel = spSection->NewSection(channelID);
		if(spChannel)
			itrB->second->Dump(spChannel);

		++itrB;
	}

	SYSTEMTIME nowTime;
	GetLocalTime(&nowTime);
	char timeStr[64] = {0};
	sprintf(timeStr, "CellApp_%04d%02d%02d_%02d%02d.dump", nowTime.wYear, nowTime.wMonth, nowTime.wDay, nowTime.wHour, nowTime.wMinute);
	spSection->Save(timeStr);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FvUInt16 SpaceInfo::FindCellIdx(float fX, float fY) const
{
	FV_ASSERT(m_kKDTree.m_pkNodes);
	SpaceKDTree::Node* pkNode = m_kKDTree.m_pkNodes;

	for(;;)
	{
		float fValue(fY);
		if(pkNode->m_bX)
			fValue = fX;

		if(fValue < pkNode->m_fKey)
		{
			if(pkNode->m_bLeftLeaf)
				return pkNode->GetLeftValue();
			pkNode = m_kKDTree.m_pkNodes + pkNode->GetLeftValue();
		}
		else
		{
			if(pkNode->m_bRightLeaf)
				return pkNode->GetRightValue();
			pkNode = m_kKDTree.m_pkNodes + pkNode->GetRightValue();
		}
	}
}

bool CellData::FindGhostGroup(float fX, float fY, FvUInt32& uiSize, FvUInt16*& pkIDs) const
{
	if(!m_kKDTree.m_pkIDGroups)
		return false;

	GhostKDTree::Node* pkNodeHead;
	GhostKDTree::Node* pkNode;
	float fValue(fY);

	if(fX < m_kKDTree.m_kGhostRect.x)
	{
		pkNodeHead = pkNode = m_kKDTree.m_pkNodes[0];
	}
	else if(fX > m_kKDTree.m_kGhostRect.y)
	{
		pkNodeHead = pkNode = m_kKDTree.m_pkNodes[1];
	}
	else if(fY < m_kKDTree.m_kGhostRect.z)
	{
		fValue = fX;
		pkNodeHead = pkNode = m_kKDTree.m_pkNodes[2];
	}
	else if(fY > m_kKDTree.m_kGhostRect.w)
	{
		fValue = fX;
		pkNodeHead = pkNode = m_kKDTree.m_pkNodes[3];
	}
	else
	{
		return false;
	}

	if(!pkNode)
		return false;

	FvUInt16 uiIDIdx(0);
	for(;;)
	{
		if(fValue < pkNode->m_fKey)
		{
			if(pkNode->m_bLeftLeaf)
			{
				uiIDIdx = pkNode->GetLeftValue();
				break;
			}
			pkNode = pkNodeHead + pkNode->GetLeftValue();
		}
		else
		{
			if(pkNode->m_bRightLeaf)
			{
				uiIDIdx = pkNode->GetRightValue();
				break;
			}
			pkNode = pkNodeHead + pkNode->GetRightValue();
		}
	}

	uiSize = m_kKDTree.m_pkIDGroups[uiIDIdx].m_uiSize;
	pkIDs = m_kKDTree.m_pkIDGroups[uiIDIdx].m_pkAddr;
	return true;
}

AoICfg::AoICfg()
{
	//! 赋默认值
	m_fGridSize = 20.0f;
	m_uiRefPtLvl = 1;
	m_fMinMoveStep = 0.1f;
	m_uiObjInitSize = 1024;
	m_uiObjIncrSize = 512;
	m_uiPosInitSize = 1024;
	m_uiPosIncrSize = 512;
	m_uiEvtInitSize = 1024;
	m_uiEvtIncrSize = 512;
	m_uiAoIExtInitSize = 1024;
	m_uiAoIExtIncrSize = 512;
	m_uiTrapExtInitSize = 1024;
	m_uiTrapExtIncrSize = 512;
}

SpaceInfo::SpaceInfo()
:m_pkCellDatas(NULL)
{
}

SpaceInfo::~SpaceInfo()
{
	FV_SAFE_DELETE_ARRAY(m_pkCellDatas);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RegisterToCellAppMgrReplyHandler : public FvNetReplyMessageHandler
{
public:
	RegisterToCellAppMgrReplyHandler()
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		FV_ASSERT(data.RemainingLength() == sizeof(FvCellAppInitData));
		FvCellAppInitData initData;
		data >> initData;
		FvEntityManager::Instance().FinishInit(initData);

		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("RegisterToCellAppMgrReplyHandler Exception\n");
		delete this;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FV_SINGLETON_STORAGE(FvEntityManager)

FvEntityManager::FvEntityManager(FvNetNub& nub)
:m_iCellAppID(FvCellAppID(-1))
,m_kNub(nub)
,m_pkCellAppMgr(NULL)
,m_kBaseAppAddr(FvNetAddress::NONE)
,m_pkEntityExports(NULL)
,m_uiLoadSpaceGeometryIdx(0xFF)
,m_pkSpaceCfgBuf(NULL)
,m_kIdxChannelFinder(nub)
,m_bStarted(false)
,m_fLoad(0.0f)
,m_uiDllTickLastTime(0)
,m_uiTime(0)
,m_pkTimeKeeper(NULL)
,m_iUpdateHertz(DEFAULT_GAME_UPDATE_HERTZ)
,m_pkCellAppData(NULL)
,m_pkGlobalData(NULL)
,m_bGlobalInit(false)
,m_bLocalMailBoxAsRemote(false)
,m_spGlobalIDs(NULL)
{
	FvServerConfig::Update( "gameUpdateHertz", m_iUpdateHertz );

	float fSystemManagePeriodInSeconds = FvServerConfig::Get( "cellApp/systemManagePeriod", 2.f );
	m_iSystemManagePeriod = int( floorf( fSystemManagePeriodInSeconds * m_iUpdateHertz + 0.5f ) );
	float fDllUpdatePeriodInSeconds = FvServerConfig::Get( "cellApp/dllUpdatePeriod", 30.f );
	m_iDllUpdatePeriod = int( floorf( fDllUpdatePeriodInSeconds * m_iUpdateHertz + 0.5f ) );
	float fTimeSyncPeriodInSeconds = FvServerConfig::Get( "cellApp/timeSyncPeriod", 60.f );
	m_iSyncTimePeriod = int( floorf( fTimeSyncPeriodInSeconds * m_iUpdateHertz + 0.5f ) );
	m_bLocalMailBoxAsRemote = FvServerConfig::Get( "cellApp/LocalMailBoxAsRemote", false );

	FV_INFO_MSG( "SystemManage Period = %f\n", fSystemManagePeriodInSeconds);
	FV_INFO_MSG( "Dll Update Period = %f\n", fDllUpdatePeriodInSeconds);
	FV_INFO_MSG( "Time Sync Period = %f\n", fTimeSyncPeriodInSeconds);
	FV_INFO_MSG( "LocalMailBoxAsRemote = %d\n", m_bLocalMailBoxAsRemote);

	FvBSPProxyManager::Create();
	FvZoneManager::Create();
	FvBGTaskManager::Instance().StartThreads(1);
}

FvEntityManager::~FvEntityManager()
{
	FvBGTaskManager::Instance().StopAll();
	FvBGTaskManager::Instance().Fini();
	FvZoneManager::Destory();
	FvBSPProxyManager::Destory();

	if(m_pkTimeKeeper)
	{
		delete m_pkTimeKeeper;
		m_pkTimeKeeper = NULL;
	}

#ifndef __DEV_MODE__
	FV_ASSERT(m_kSpaceMap.empty());
	FV_ASSERT(m_kAllEntityMap.empty());

	{
		SpaceInfoList::iterator itrB = m_kSpaceInfoList.begin();
		SpaceInfoList::iterator itrE = m_kSpaceInfoList.end();
		while(itrB != itrE)
		{
			if(*itrB)
				delete *itrB;
			++itrB;
		}
		m_kSpaceInfoList.clear();
	}
#endif

	FV_SAFE_DELETE_ARRAY(m_pkSpaceCfgBuf);

	FvLogicDllManager::Instance().UnInitializeLogic();
}

class EntityMessageHandler : public FvNetInputMessageHandler
{
protected:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FV_ASSERT(header.m_pkChannel->IsIndexed());

		if((header.m_uiIdentifier & 0xC0) == 0xC0)
		{
			FvEntityID uiEntityID(header.m_pkChannel->id());
			FvEntity* pkObj = FvEntityManager::Instance().FindEntity(uiEntityID);
			FV_ASSERT(pkObj);
			if((header.m_uiIdentifier & 0x20) == 0x20)
				pkObj->RunExposedMethod(header.m_uiIdentifier & ~0xC0, data, data.RemainingLength(), &(pkObj->BaseMBInEntity()));
			else
				pkObj->RunExposedMethod(header.m_uiIdentifier & ~0xC0, data, data.RemainingLength(), NULL);
		}
		else
		{
			FvEntityID uiEntityID;
			data >> uiEntityID;
			FvEntity* pkObj = FvEntityManager::Instance().FindEntity(uiEntityID);
			if(pkObj)
			{
				if((header.m_uiIdentifier & 0x20) == 0x20)
				{
					FvEntity* pkOwner = FvEntityManager::Instance().FindEntity(header.m_pkChannel->id());
					FV_ASSERT(pkOwner);
					pkObj->RunExposedMethod(header.m_uiIdentifier & ~0x80, data, data.RemainingLength(), &(pkOwner->BaseMBInEntity()));
				}
				else
				{
					pkObj->RunExposedMethod(header.m_uiIdentifier & ~0x80, data, data.RemainingLength(), NULL);
				}
			}
			else
			{
				if((header.m_uiIdentifier & 0x20) == 0x20)
				{
					FvEntity* pkOwner = FvEntityManager::Instance().FindEntity(header.m_pkChannel->id());
					FV_ASSERT(pkOwner);
					FvRPCCallback<int> kCB(pkOwner->BaseMBInEntity(), 0);
					FvUInt8 uiCBCnt;
					data >> uiCBCnt >> uiCBCnt;
					FV_ASSERT(uiCBCnt);
					for(FvUInt8 i=0; i<uiCBCnt; ++i)
					{
						FvUInt32 uiCBID;
						data >> uiCBID;
						kCB.SetID(uiCBID);
						kCB.Exception();//! TODO: 统一成一个消息返回
					}
				}
			}
		}
	}
};

EntityMessageHandler g_EntityMessageHandler;

bool FvEntityManager::Init( int argc, char *argv[] )
{
	//! 载入本CellApp的地形数据
	if(!LoadSpaceGeometry(argc, argv))
	{
		FV_ERROR_MSG("%s, LoadSpaceGeometry Failed\n", __FUNCTION__);
		return false;
	}

	if (m_kNub.Socket() == -1)
	{
		FV_ERROR_MSG( "Failed to create Nub on internal interface.\n" );
		return false;
	}

	FV_INFO_MSG( "Nub Address = %s\n", m_kNub.Address().c_str() );

	if(!IsGlobalMode())
	{
		if (!FV_INIT_ANONYMOUS_CHANNEL_CLIENT( m_kDBMgr, m_kNub,
			CellAppInterface, DBInterface, 0 ))
		{
			FV_INFO_MSG( "CellApp::init: Database not ready yet.\n" );
		}
		else
		{
			InitEntityIDClient();
		}
	}

	CellAppInterface::RegisterWithNub( m_kNub );

	for(FvNetMessageID id = 128; id != 255; ++id)
	{
		m_kNub.ServeInterfaceElement( CellAppInterface::RunExposedMethod,
			id, &g_EntityMessageHandler );
	}

	m_kNub.RegisterChannelFinder(&m_kIdxChannelFinder);

	FvNetReason reason = CellAppInterface::RegisterWithMachined( m_kNub, 0 );
	if (reason != FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG("CellApp::init: Unable to register with nub. "
			"Is machined running?\n");
		return false;
	}

	{
		FvNetNub::InterfaceInfoVec kGlobalApps;
		reason = m_kNub.FindInterfaces(0, "GlobalAppInterface", 0, kGlobalApps);

		if (reason == FV_NET_REASON_SUCCESS)
		{
			if(kGlobalApps.size() > 1)
			{
				FV_ERROR_MSG( "CellApp::init: "
					"Found too many GlobalApp Interface: %d\n", kGlobalApps.size() );

				return false;
			}
			else
			{
				InitGlobalEntityIDClientAndRegister(kGlobalApps.front().kAddr, true);
			}
		}
		else if (reason == FV_NET_REASON_TIMER_EXPIRED)
		{
			FV_INFO_MSG( "CellApp::init: GlobalApp not ready yet.\n" );
		}
		else
		{
			FV_ERROR_MSG( "CellApp::init: "
				"Failed to find GlobalApp interface: %s\n",
				NetReasonToString( (FvNetReason &)reason ) );

			return false;
		}

		m_kNub.RegisterBirthListener( CellAppInterface::HandleGlobalAppBirth, "GlobalAppInterface", 0 );
	}

	{
		FvNetAddress cellAppMgrAddr;
		reason = m_kNub.FindInterface( "CellAppMgrInterface", 0, cellAppMgrAddr );

		if (reason == FV_NET_REASON_SUCCESS)
		{
			m_pkCellAppMgr = FindOrCreateChannel(cellAppMgrAddr);
		}
		else if (reason == FV_NET_REASON_TIMER_EXPIRED)
		{
			FV_INFO_MSG( "CellApp::init: CellAppMgr not ready yet.\n" );
		}
		else
		{
			FV_ERROR_MSG( "CellApp::init: "
				"Failed to find CellAppMgr interface: %s\n",
				NetReasonToString( (FvNetReason &)reason ) );

			return false;
		}

		m_kNub.RegisterBirthListener( CellAppInterface::HandleCellAppMgrBirth,
			"CellAppMgrInterface" );
	}

	void* pkEntityExports(NULL);
	if(!FvLogicDllManager::Instance().DllExport(pkEntityExports))
	{
		FV_ERROR_MSG( "CellApp::Init: "
			"Get Dll Export Failed\n");
		return false;
	}
	m_pkEntityExports = (FvAllCellEntityExports*)pkEntityExports;
/**
	FvString kEntityDefMD5Digest;
	if(!FvServerConfig::Update( "EntityDefMD5Digest", kEntityDefMD5Digest ))
	{
		FV_ERROR_MSG( "CellApp::Init: "
			"Get EntityDefMD5Digest from Server XML Failed\n");
		return false;
	}

	if(m_pkEntityExports->kMD5Digest.Quote() != kEntityDefMD5Digest)
	{
		FV_ERROR_MSG( "CellApp::Init: "
			"EntityDefMD5Digest in Dll isn't Equal to the one from Server XML\n");
		return false;
	}
**/
	FvObjUDO::SetAllUDOExports(m_pkEntityExports->pkUDOExports);

	if(!FvLogicDllManager::Instance().InitializeLogic())
		return false;

	signal( SIGINT, ::IntSignalHandler );
#ifndef _WIN32
	signal( SIGHUP, ::IntSignalHandler );
#endif //ndef _WIN32

	//FV_INIT_WATCHER_DOC( "cellapp" );

	//FV_REGISTER_WATCHER( 0, "cellapp",
	//	"Cell App", "cellApp", m_kNub );

#ifdef __DEV_MODE__
	if(!IsGlobalMode())
	{
		class ClearDBReplyHandler : public FvNetReplyMessageHandler
		{
		public:
			ClearDBReplyHandler()
			{

			}

		private:
			virtual void HandleMessage( const FvNetAddress & source,
				FvNetUnpackedMessageHeader & header,
				FvBinaryIStream & data,
				void * arg )
			{
				data.Finish();
				delete this;
			}

			virtual void HandleException( const FvNetNubException & ne,
				void * arg )
			{
				delete this;
			}
		};

		//! 通知DB清空logon表
		if(GetDBMgr().Channel().IsEstablished())
		{
			FV_INFO_MSG("Clear DB\n");

			char sqlCmd[256] = {0};
			sprintf(sqlCmd, "TRUNCATE TABLE futurevisionlogons");
			int len = strlen(sqlCmd);
			FvNetBundle& bundle = GetDBMgr().Bundle();
			bundle.StartRequest(DBInterface::ExecuteRawCommand, new ClearDBReplyHandler());
			memcpy(bundle.Reserve(len), sqlCmd, len);
			GetDBMgr().Send();
		}
	}
#endif

	AddToCellAppMgr();

	return true;
}

bool FvEntityManager::FinishInit( const FvCellAppInitData & initData )
{
	FV_INFO_MSG("Add to Mgr Reply, id:%d, bReady:%d, time:%d, BaseAppAddr:%s\n", initData.id, initData.isReady, initData.time, initData.baseAppAddr.c_str());

	m_iCellAppID = initData.id;
	m_kBaseAppAddr = initData.baseAppAddr;
	m_uiTime = initData.time;
	//initData.isReady;

	InitGlobalID();

	if(initData.isReady)
	{
		OnStart();
	}

	return true;
}

FvEntity* FvEntityManager::FindEntity(FvEntityID id) const
{
	EntityMap::const_iterator itr = m_kAllEntityMap.find(id);
	if(itr != m_kAllEntityMap.end())
		return itr->second.GetObject();
	else
		return NULL;
}

FvCell* FvEntityManager::FindCell( FvSpaceID id ) const
{
	SpaceMap::const_iterator itr = m_kSpaceMap.find(id);
	if(itr != m_kSpaceMap.end())
		return itr->second->GetCell();
	else
		return NULL;
}

FvSpace* FvEntityManager::FindSpace( FvSpaceID id ) const
{
	SpaceMap::const_iterator itr = m_kSpaceMap.find(id);
	if(itr != m_kSpaceMap.end())
		return itr->second;
	else
		return NULL;
}

FvEntity* FvEntityManager::CreateEntity(const char* pcEntityTypeName, const FvAllData& kAllData, FvUInt8 uiInitFlg)
{
	FvCellEntityExport* pkExport = m_pkEntityExports->FindByName(pcEntityTypeName);
	if(!pkExport)
		return NULL;
	return CreateEntity(pkExport->uiTypeID, kAllData, uiInitFlg);
}

FvEntity* FvEntityManager::CreateEntity(FvEntityTypeID uiEntityType, const FvAllData& kAllData, FvUInt8 uiInitFlg)
{
	const FvAllData* pkAllData = &kAllData;

	if(uiEntityType >= m_pkEntityExports->uiEntityCnt)
	{
		FV_ERROR_MSG("%s, EntityType:%d too Big\n", __FUNCTION__, uiEntityType);
		return NULL;
	}
	else if(!m_pkEntityExports->ppkEntityExport[uiEntityType])
	{
		FV_ERROR_MSG("%s, EntityType(%d) has global script Or has no cell script\n", __FUNCTION__, uiEntityType);
		return NULL;
	}

	if(pkAllData->GetType() != uiEntityType)
	{
		FV_ERROR_MSG("%s, AllData is NULL or EntityType != AllDataType\n", __FUNCTION__);
		return NULL;
	}

	FvCell* pkCell = FindCell(pkAllData->m_iSpaceID);
	if(!pkCell)
	{
		FV_ERROR_MSG("%s, Can't Find Space, id:%d\n", __FUNCTION__, pkAllData->m_iSpaceID);
		return NULL;
	}

	const SpaceInfo* pkSpaceInfo = FvEntityManager::Instance().FindSpaceInfo(pkCell->SpaceType());
	FV_ASSERT(pkSpaceInfo);
	if(!pkSpaceInfo->m_kSpaceRect.IsPointIn(pkAllData->m_kPos))
	{
		FV_ERROR_MSG("%s, Pos(%.1f,%.1f,%.1f) is Not in Space\n", __FUNCTION__, pkAllData->m_kPos.x, pkAllData->m_kPos.y, pkAllData->m_kPos.z);
		return NULL;
	}

	//! TODO: 先屏蔽,如果不在本Cell里,则迁移过去
	//if(!pkCell->IsInCell(kPosition))
	//	return NULL;

	FvEntityID iEntityID = NewEntityID();
	if(iEntityID == FV_NULL_ENTITY)
		return NULL;

	FvCellEntityExport* pkExport = m_pkEntityExports->ppkEntityExport[uiEntityType];
	FvEntity* pRole = pkExport->pFunCreateEntity();
	pRole->Init(iEntityID, 0, pkExport, pkAllData->m_kPos, pkAllData->m_kDir, pkCell->GetSpace(), FvNetAddress::NONE);//! 没有Base部分
	pRole->InitReal(pkAllData, uiInitFlg);

	return pRole;
}

void FvEntityManager::CreateEntityOnBaseApp(const char* pcEntityTypeName, const FvAllData* pkAllData, FvUInt8 uiInitFlg)
{
	//! Global模式下无法创建Base,因为没有BaseApp可以用
	if(IsGlobalMode())
	{
		FV_ERROR_MSG("%s, Can't CreateEntity in Global mode\n", __FUNCTION__);
		return;
	}

	FvCellEntityExport* pkExport = m_pkEntityExports->FindByName(pcEntityTypeName);
	if(!pkExport)
		return;

	CreateEntityOnBaseApp(pkExport->uiTypeID, pkAllData, uiInitFlg);
}

void FvEntityManager::CreateEntityOnBaseApp(FvEntityTypeID uiEntityType, const FvAllData* pkAllData, FvUInt8 uiInitFlg)
{
	//! Global模式下无法创建Base,因为没有BaseApp可以用
	if(IsGlobalMode())
	{
		FV_ERROR_MSG("%s, Can't CreateEntity in Global mode\n", __FUNCTION__);
		return;
	}

	if(uiEntityType >= m_pkEntityExports->uiEntityCnt)
	{
		FV_ERROR_MSG("%s, EntityType:%d is too Big\n", __FUNCTION__, uiEntityType);
		return;
	}
	else if(!m_pkEntityExports->ppkEntityExport[uiEntityType])
	{
		FV_ERROR_MSG("%s, EntityType(%d) has global script Or has no cell script\n", __FUNCTION__, uiEntityType);
		return;
	}

	if(!pkAllData || pkAllData->GetType()!=uiEntityType)
	{
		FV_ERROR_MSG("%s, AllData is NULL or EntityType != AllDataType\n", __FUNCTION__);
		return;
	}

	//! CellApp
	//FvEntityTypeID type;
	//bool bGlobal;
	//FvUInt8 uiInitFlg;
	//(CELL|BASE)的数据
	//FvVector3(pos)
	//FvVector3(dir)
	//FvInt32(spaceID)
	//FvUInt16(spaceType)

	FV_ASSERT(!m_kBaseAppAddr.IsNone());
	FvNetChannel& kChannel = *FindOrCreateChannel(m_kBaseAppAddr);
	FvNetBundle& kBundle = kChannel.Bundle();
	kBundle.StartMessage(BaseAppIntInterface::CreateBaseWithCellData);
	kBundle << uiEntityType << false << uiInitFlg;//! Global CellApp不能创建Base对象,因为没有BaseApp可以用
	pkAllData->SerializeToStream(kBundle);
	kChannel.DelayedSend();
}

void FvEntityManager::DelBaseAppData(const char* pcKey)
{

}

bool FvEntityManager::IsGlobalMode()
{
	return m_kNub.GetUserID() == 0 ? true : false;
}

FvMailBox* FvEntityManager::FindGlobalBase(const char* pcLabel) const
{
	if(!pcLabel || !pcLabel[0])
		return NULL;

	return FindGlobalBase(pcLabel);
}

FvMailBox* FvEntityManager::FindGlobalBase(const FvString& kLabel) const
{
	FvGlobalBases::const_iterator itr = m_kGlobalBases.find(kLabel);
	if(itr != m_kGlobalBases.end())
		return const_cast<FvMailBox*>(&(itr->second));
	else
		return NULL;
}

FvMailBox* FvEntityManager::FindGlobalMailBox(const char* pcLabel) const
{
	if(!pcLabel || !pcLabel[0])
		return NULL;

	GlobalMailBoxMap::const_iterator itr = m_kGlobalMailBoxMap.find(pcLabel);
	if(itr != m_kGlobalMailBoxMap.end())
		return const_cast<FvMailBox*>(&(itr->second));
	else
		return NULL;
}

FvMailBox* FvEntityManager::FindGlobalMailBox(const FvString& kLabel) const
{
	if(kLabel.empty())
		return NULL;

	GlobalMailBoxMap::const_iterator itr = m_kGlobalMailBoxMap.find(kLabel);
	if(itr != m_kGlobalMailBoxMap.end())
		return const_cast<FvMailBox*>(&(itr->second));
	else
		return NULL;
}

FvEntityTypeID FvEntityManager::EntityNameToIndex(const char* pcEntityTypeName) const
{
	FvCellEntityExport* pkExport = m_pkEntityExports->FindByName(pcEntityTypeName);
	if(!pkExport)
	{
		FV_ERROR_MSG("%s, EntityTypeName(%s) is't Exist\n", __FUNCTION__, pcEntityTypeName);
		return FV_INVALID_ENTITY_TYPE_ID;
	}
	return pkExport->uiTypeID;
}

FvUInt64 FvEntityManager::NewGlobalID(FvUInt8 uiIdx)
{
	FvUInt64 uiGlobalID(0);
	bool bSave(false);

	int iCnt(m_kGlobalIDs.size());
	bool bExist(false);
	for(int i=0; i<iCnt; ++i)
	{
		if(m_kGlobalIDs[i].first == uiIdx)
		{
			bExist = true;
			uiIdx = m_kGlobalIDs[i].second.m_uiNowID;
			m_kGlobalIDs[i].second.m_uiNowID++;
			if(m_kGlobalIDs[i].second.m_uiNowID >= m_kGlobalIDs[i].second.m_uiMaxID)
			{
				bSave = true;
				m_kGlobalIDs[i].second.m_uiMaxID += 100;
			}
			break;
		}
	}

	if(!bExist)
	{
		bSave = true;
		uiGlobalID = 0;
		m_kGlobalIDs.push_back(std::make_pair(uiIdx, GlobalID(1, 100)));
	}

	if(bSave)
		SaveGlobalID();

	return (((FvUInt64)m_iCellAppID)<<56) | uiGlobalID;
}

void FvEntityManager::Startup( const CellAppInterface::StartupArgs & args )
{
	m_kBaseAppAddr = args.baseAppAddr;
	OnStart();
}

void FvEntityManager::SetGameTime( const CellAppInterface::SetGameTimeArgs & args )
{
	m_uiTime = args.gameTime;
}

void FvEntityManager::SetBaseApp( const CellAppInterface::SetBaseAppArgs & args )
{
	FV_ASSERT(!IsGlobalMode());
	m_kBaseAppAddr = args.baseAppAddr;
}

void FvEntityManager::ShutDown( const CellAppInterface::ShutDownArgs & args )
{

}

void FvEntityManager::ControlledShutDown( const CellAppInterface::ControlledShutDownArgs & args )
{

}

void FvEntityManager::AddCell( FvBinaryIStream & data )
{
	//! FvSpaceID iSpaceID;
	//! FvUInt16 uiSpaceType;
	//! FvUInt16 uiCellIdx;

	FvSpaceID iSpaceID;
	FvUInt16 uiSpaceType;
	FvUInt16 uiCellIdx;
	data >> iSpaceID >> uiSpaceType >> uiCellIdx;

	FV_INFO_MSG("%s, spaceID:%d, spaceType:%d, cellIdx:%d\n", __FUNCTION__, iSpaceID, uiSpaceType, uiCellIdx);

	const SpaceInfo* pkSpaceInfo = FindSpaceInfo(uiSpaceType);
	FV_ASSERT(pkSpaceInfo);
	FvSpace* pkSpace = new FvSpace(iSpaceID, uiSpaceType, uiCellIdx, pkSpaceInfo->m_kSpacePath);
	bool bRet = m_kSpaceMap.insert(std::make_pair(iSpaceID, pkSpace)).second;
	FV_ASSERT(bRet);
}

void FvEntityManager::SetSharedData( FvBinaryIStream & data )
{

}

void FvEntityManager::DelSharedData( FvBinaryIStream & data )
{

}

void FvEntityManager::AddGlobalBase(FvBinaryIStream & data)
{
	FvString kLabel;
	FvMailBox kMB;
	data >> kLabel >> kMB;
	if(data.Error())
	{
		FV_WARNING_MSG("%s, stream Err\n", __FUNCTION__);
		return;
	}

	if(!m_kGlobalBases.insert(std::make_pair(kLabel, kMB)).second)
	{
		FV_WARNING_MSG("%s, Label(%s) repeated\n", __FUNCTION__, kLabel.c_str());
		return;
	}

	FV_INFO_MSG("%s, Label(%s)\n", __FUNCTION__, kLabel.c_str());
}

void FvEntityManager::DelGlobalBase(FvBinaryIStream & data)
{
	FvString kLabel;
	data >> kLabel;
	if(data.Error())
	{
		FV_WARNING_MSG("%s, stream Err\n", __FUNCTION__);
		return;
	}

	if(m_kGlobalBases.erase(kLabel) == 0)
	{
		FV_WARNING_MSG("%s, Label(%s) Nonexistent\n", __FUNCTION__, kLabel.c_str());
		return;
	}

	FV_INFO_MSG("%s, Label(%s)\n", __FUNCTION__, kLabel.c_str());
}

void FvEntityManager::AddGlobalMailBox(FvBinaryIStream& kData)
{
	FvString kLabel;
	FvMailBox kMB;
	kData >> kLabel >> kMB;

	m_kGlobalMailBoxMap.insert(std::make_pair(kLabel, kMB));
}

void FvEntityManager::DelGlobalMailBox(FvBinaryIStream& kData)
{
	FvString kLabel;
	kData >> kLabel;

	GlobalMailBoxMap::iterator itr = m_kGlobalMailBoxMap.find(kLabel);
	if(itr != m_kGlobalMailBoxMap.end())
		m_kGlobalMailBoxMap.erase(itr);
}

void FvEntityManager::OnloadTeleportedEntity( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	//! FvEntityID	NearbyEntityID
	//! FvVector3	pos
	//! kDirection	dir
	//! FvUInt16 uiGhostSize
	//! CreateGhost Data
	//! Real Data
	FvEntityID iNearbyEntityID;
	FvUInt16 uiGhostSize;
	FvVector3 kNewPos;
	FvDirection3 kNewDir;
	data >> iNearbyEntityID >> kNewPos >> kNewDir >> uiGhostSize;

	//printf("%s, Pos[%f,%f,%f]\n", __FUNCTION__, kNewPos.x, kNewPos.y, kNewPos.z);

	//! 查找实体
	FvEntity* pkNearbyEntity = FvEntityManager::Instance().FindEntity(iNearbyEntityID);
	//! 找到NearbyEntity
	if(pkNearbyEntity)
	{
		FvSpace* pkSpace = pkNearbyEntity->GetSpace();
		FV_ASSERT(pkSpace);

		//! 检查坐标是否在Space内
		const SpaceInfo* pkSpaceInfo = FvEntityManager::Instance().FindSpaceInfo(pkSpace->SpaceType());
		FV_ASSERT(pkSpaceInfo);
		if(pkSpaceInfo->m_kSpaceRect.IsPointIn(kNewPos))
		{
			//! 创建Ghost
			int s1 = data.RemainingLength();
			FvSpaceID iSpaceID;
			FvVector3 kOldPos;
			FvDirection3 kOldDir;
			data >> iSpaceID >> kOldPos >> kOldDir;
			pkSpace->CreateGhost(kNewPos, kNewDir, data);
			int s2 = data.RemainingLength();
			FV_ASSERT(s2 + uiGhostSize == s1);

			//! 创建Real
			FvEntityID iEntityID;
			data >> iEntityID;
			FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(iEntityID);
			FV_ASSERT(pkEntity);
			pkEntity->ConvertGhostToReal(data);
			FV_ASSERT(data.RemainingLength() == 0);

			//! 传送后处理
			pkEntity->TeleportFinish(iSpaceID);

			return;
		}
		else
		{
			//printf("%s, Teleport Pos(%.1f, %.1f, %.1f) isn't in Space(%.1f, %.1f, %.1f, %.1f), Type:%d\n", __FUNCTION__,
			FV_ERROR_MSG("%s, Teleport Pos(%.1f, %.1f, %.1f) isn't in Space(%.1f, %.1f, %.1f, %.1f), Type:%d\n", __FUNCTION__,
							kNewPos.x, kNewPos.y, kNewPos.z,
							pkSpaceInfo->m_kSpaceRect.LeftBorder(),
							pkSpaceInfo->m_kSpaceRect.RightBorder(),
							pkSpaceInfo->m_kSpaceRect.BottomBorder(),
							pkSpaceInfo->m_kSpaceRect.TopBorder(),
							pkSpace->SpaceType());
		}
	}
	else//! 找不到NearbyEntity
	{
		FV_ERROR_MSG("%s, Teleport NearbyEntity(%d) Cann't Find\n", __FUNCTION__, iNearbyEntityID);
		//printf("%s, Teleport NearbyEntity(%d) Cann't Find\n", __FUNCTION__, iNearbyEntityID);
	}

	//! 迁移失败,在发起端恢复实体
	FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(srcAddr);
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(CellAppInterface::CreateGhost);
	kBundle.Transfer(data, uiGhostSize);
	kBundle.StartMessage(CellAppInterface::Onload);
	//! 修改flag,标记传送失败
	kBundle.Transfer(data, sizeof(FvEntityID));
	FvUInt8 uiFlag;
	data >> uiFlag;
	uiFlag = 1;//! flag: 0:请求迁移, 1:传送失败后的恢复
	kBundle << uiFlag;
	kBundle.Transfer(data, data.RemainingLength());
	pkChannel->DelayedSend();

	//printf("%s, Failed\n", __FUNCTION__);
}

void FvEntityManager::CreateSpaceIfNecessary( FvBinaryIStream & data )
{
	FV_ASSERT(0);
}

void FvEntityManager::HandleCellAppMgrBirth(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);

	if(!m_pkCellAppMgr && !kAddr.IsNone())
	{
		FV_INFO_MSG( "CellApp::HandleCellAppMgrBirth: "
			"CellAppMgr is now ready.\n" );

		m_pkCellAppMgr = FindOrCreateChannel(kAddr);
		AddToCellAppMgr();
	}
}

void FvEntityManager::HandleGlobalAppBirth(const InterfaceListenerMsg& kMsg)
{
	FV_INFO_MSG("%s\n", __FUNCTION__);

	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);
	InitGlobalEntityIDClientAndRegister(kAddr, false);	//! TODO: 异步获取id,需要等一段时间才能使用
}

void FvEntityManager::HandleCellAppDeath(const CellAppInterface::HandleCellAppDeathArgs & args)
{

}

void FvEntityManager::HandleBaseAppDeath( FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
	FV_ASSERT(!IsGlobalMode());
	data.Finish();
}

void FvEntityManager::RunScript( FvBinaryIStream & data )
{

}

void FvEntityManager::CallWatcher( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data )
{

}

const SpaceInfo* FvEntityManager::FindSpaceInfo(FvUInt16 uiSpaceType) const
{
	if(uiSpaceType >= m_kSpaceInfoList.size())
		return NULL;
	else
		return m_kSpaceInfoList[uiSpaceType];
}

void FvEntityManager::EraseSpace(FvSpace* pkSpace)
{
	FV_ASSERT(pkSpace);
	m_kSpaceMap.erase(pkSpace->SpaceID());
	delete pkSpace;
}

void FvEntityManager::AddEntity(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity);
	bool bRet = m_kAllEntityMap.insert(std::make_pair(pkEntity->GetEntityID(), pkEntity)).second;
	FV_ASSERT(bRet);
}

void FvEntityManager::DelEntity(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity);
	int s = (int)m_kAllEntityMap.erase(pkEntity->GetEntityID());
	FV_ASSERT(s == 1);
}

bool FvEntityManager::FindCellMidPt(FvUInt16 uiSpaceType, FvUInt16 uiCellIdx, FvVector3& kPos) const
{
	const SpaceInfo* pkSpaceInfo = FindSpaceInfo(uiSpaceType);
	if(!pkSpaceInfo)
		return false;
	if(pkSpaceInfo->m_uiCellCnt <= uiCellIdx)
		return false;

	FvCellRect& kRect = pkSpaceInfo->m_pkCellDatas[uiCellIdx].m_kRect;
	kPos.x = (kRect.LeftBorder() + kRect.RightBorder()) * 0.5f;
	kPos.y = (kRect.BottomBorder() + kRect.TopBorder()) * 0.5f;
	return true;
}

void FvEntityManager::AddDestroyEntity(FvEntity* pkEntity)
{
	pkEntity->SetDestroyEntityIdx(FvUInt32(m_kDestroyEntities.size()));
	m_kDestroyEntities.push_back(pkEntity);
}

void FvEntityManager::RemoveDestroyEntity(FvEntity* pkEntity)
{
	FvUInt32 uiIdx = pkEntity->GetDestroyEntityIdx();
	pkEntity->ClearDestroyEntityIdx();
	FV_ASSERT(uiIdx < m_kDestroyEntities.size());
	if(!m_kDestroyEntities.empty())
	{
		m_kDestroyEntities[uiIdx] = m_kDestroyEntities.back();
		m_kDestroyEntities[uiIdx]->SetDestroyEntityIdx(uiIdx);
	}
	m_kDestroyEntities.pop_back();
}

const FvEntityManager::ZoneEntityInfoList* FvEntityManager::FindZoneEntityInfoList(FvUInt16 uiSpaceType) const
{
	ZoneEntityInfoSpaceMap::const_iterator itr = m_kZoneEntityInfoSpaceMap.find(uiSpaceType);
	if(itr != m_kZoneEntityInfoSpaceMap.end())
		return &(itr->second);
	else
		return NULL;
}

int FvEntityManager::HandleTimeout( FvNetTimerID id, void * arg )
{
	switch(reinterpret_cast<FvUIntPtr>(arg))
	{
	case TIMEOUT_GAME_TICK:
		{
			++m_uiTime;

			if(m_uiTime % m_iDllUpdatePeriod == 0)
			{
				FvUInt64 nowTime = Timestamp();
				float dTime = float(nowTime-m_uiDllTickLastTime) / StampsPerSecond();
				m_uiDllTickLastTime = nowTime;

				FvLogicDllManager::Instance().TickDll(dTime);
			}

			if(m_uiTime % m_iSyncTimePeriod == 0)
			{
				if(m_pkTimeKeeper)
					m_pkTimeKeeper->SynchroniseWithMaster();
			}

			if(m_uiTime % m_iSystemManagePeriod == 0)
			{
				if(0)
				{
					static int s_Stats[10] = {0};
					int* pLoopStats = FvEntityManager::Nub().LoopStats();
					FV_ASSERT(pLoopStats);

					FV_INFO_MSG("Try:%d, Get:%d, Err:%d, Sel:%d, Call:%d, Resched:%d\n",
						pLoopStats[FvNetNub::RECV_TRYS]-s_Stats[FvNetNub::RECV_TRYS],
						pLoopStats[FvNetNub::RECV_GETS]-s_Stats[FvNetNub::RECV_GETS],
						pLoopStats[FvNetNub::RECV_ERRS]-s_Stats[FvNetNub::RECV_ERRS],
						pLoopStats[FvNetNub::RECV_SELS]-s_Stats[FvNetNub::RECV_SELS],
						pLoopStats[FvNetNub::TIMER_CALLS]-s_Stats[FvNetNub::TIMER_CALLS],
						pLoopStats[FvNetNub::TIMER_RESCHEDS]-s_Stats[FvNetNub::TIMER_RESCHEDS]);

					s_Stats[FvNetNub::RECV_TRYS]		= pLoopStats[FvNetNub::RECV_TRYS];
					s_Stats[FvNetNub::RECV_GETS]		= pLoopStats[FvNetNub::RECV_GETS];
					s_Stats[FvNetNub::RECV_ERRS]		= pLoopStats[FvNetNub::RECV_ERRS];
					s_Stats[FvNetNub::RECV_SELS]		= pLoopStats[FvNetNub::RECV_SELS];
					s_Stats[FvNetNub::TIMER_CALLS]		= pLoopStats[FvNetNub::TIMER_CALLS];
					s_Stats[FvNetNub::TIMER_RESCHEDS]	= pLoopStats[FvNetNub::TIMER_RESCHEDS];
				}

				InformOfLoadToCellAppMgr();
			}

			if(!m_kDestroyEntities.empty())
			{
				FvUInt32 uiSize(m_kDestroyEntities.size());
				for(FvUInt32 i=0; i<uiSize; ++i)
					m_kDestroyEntities[i]->RealDestroy();
				m_kDestroyEntities.clear();
			}
		}
		break;
	}

	return 0;
}

bool FvEntityManager::LoadSpaceGeometry(int argc, char *argv[])
{
	if(!LoadSpaceInfo())
		return false;

	if(!LoadSpaceScheme(argc, argv))
		return false;

	FV_INFO_MSG("%s, Success\n", __FUNCTION__);
	return true;
}

class MemReader
{
public:
	MemReader(char* pkBuf, int iBufSize)
	:m_pkBuf(pkBuf),m_pkReadPt(pkBuf),m_pkEnd(pkBuf+iBufSize),m_iBufSize(iBufSize)
	{}

	void	SetReadPt(char* pkReadPt) { m_pkReadPt=pkReadPt; }
	char*	GetReadPt() { return m_pkReadPt; }

	template<typename T>
	void	Read(T& kVal)
	{
		char* pkHead = m_pkReadPt;
		int size = (int)sizeof(T);
		FV_ASSERT(pkHead + size <= m_pkEnd);
		memcpy(&kVal, pkHead, size);
		m_pkReadPt += size;
	}

	void	AddLen(int iLen)
	{
		char* pkHead = m_pkReadPt;
		FV_ASSERT(pkHead + iLen <= m_pkEnd);
		m_pkReadPt += iLen;
	}

	void	SnapTo4()
	{
		int iSize = m_pkReadPt - m_pkBuf;
		iSize = ((iSize +3) >> 2) << 2;
		m_pkReadPt = m_pkBuf + iSize;
		FV_ASSERT(m_pkReadPt <= m_pkEnd);
	}

	void ReadAoI(AoICfg& kAoI)
	{
		FvUInt32 uiFlg;
		Read(uiFlg);

#define AOI_READ(_FLG, _VAL)\
	if(uiFlg & AoICfg::_FLG)\
		{					\
			Read(kAoI._VAL);\
		}

		AOI_READ(GRIDSIZE, m_fGridSize);
		AOI_READ(REFPTLVL, m_uiRefPtLvl);
		AOI_READ(MINMOVESTEP, m_fMinMoveStep);
		AOI_READ(OBJINITSIZE, m_uiObjInitSize);
		AOI_READ(OBJINCRSIZE, m_uiObjIncrSize);
		AOI_READ(POSINITSIZE, m_uiPosInitSize);
		AOI_READ(POSINCRSIZE, m_uiPosIncrSize);
		AOI_READ(EVTINITSIZE, m_uiEvtInitSize);
		AOI_READ(EVTINCRSIZE, m_uiEvtIncrSize);
		AOI_READ(AOIEXTINITSIZE, m_uiAoIExtInitSize);
		AOI_READ(AOIEXTINCRSIZE, m_uiAoIExtIncrSize);
		AOI_READ(TRAPEXTINITSIZE, m_uiTrapExtInitSize);
		AOI_READ(TRAPEXTINCRSIZE, m_uiTrapExtIncrSize);

#undef AOI_READ
	}

protected:
	char*	m_pkBuf;
	char*	m_pkReadPt;
	char*	m_pkEnd;
	int		m_iBufSize;
};

bool FvEntityManager::LoadSpaceInfo()
{
	FvString kFileName = "ServerConfig/Spaces.cfg";
	Ogre::DataStreamPtr spDataStream = FvServerResource::Instance().OpenDataStream(kFileName);
	if(spDataStream.isNull())
	{
		FV_ERROR_MSG("%s, Open File:%s Failed\n", __FUNCTION__, kFileName.c_str());
		return false;
	}

	int iBufSize = (int)spDataStream->size();
	if(iBufSize <= 0)
	{
		FV_ERROR_MSG("%s, File:%s size is Zero\n", __FUNCTION__, kFileName.c_str());
		return false;
	}
	m_pkSpaceCfgBuf = new char[iBufSize];
	spDataStream->read(m_pkSpaceCfgBuf, iBufSize);

	//! TODO: 读文件代码太恶心了...写代码很低效

	MemReader kMR(m_pkSpaceCfgBuf, iBufSize);

	FvUInt32 uiSpaceCnt;
	kMR.Read(uiSpaceCnt);
	SpaceInfo* pkSpaceInfo(NULL);

	for(FvUInt16 uiSpaceType(0); uiSpaceType<uiSpaceCnt; ++uiSpaceType)
	{
		FvUInt32 uiSpaceSize;
		kMR.Read(uiSpaceSize);
		char* pkSpacePt = kMR.GetReadPt();
		FvUInt32 uiTmp;
		FvUInt16 uiTmp16;

		kMR.Read(uiTmp);//! bGlobalSpace

		if(IsGlobalMode() && uiTmp ||
			!IsGlobalMode() && !uiTmp)
		{
			pkSpaceInfo = new SpaceInfo();
			pkSpaceInfo->m_uiSpaceType = uiSpaceType;
			pkSpaceInfo->m_bGlobal = uiTmp ? true : false;

			FvVector4 kRect;
			kMR.Read(kRect.x);
			kMR.Read(kRect.y);
			kMR.Read(kRect.z);
			kMR.Read(kRect.w);
			pkSpaceInfo->m_kSpaceRect = kRect;

			kMR.Read(uiTmp);
			pkSpaceInfo->m_kSpacePath = kMR.GetReadPt();
			kMR.AddLen(uiTmp);

			kMR.Read(uiTmp);
			pkSpaceInfo->m_kKDTree.m_pkNodes = (SpaceInfo::SpaceKDTree::Node*)kMR.GetReadPt();
			kMR.AddLen(uiTmp);

			AoICfg kAoICfg;
			kMR.ReadAoI(kAoICfg);

			FvUInt32 uiCellCnt;
			kMR.Read(uiCellCnt);
			FV_ASSERT(uiCellCnt);
			pkSpaceInfo->m_uiCellCnt = FvUInt16(uiCellCnt);
			pkSpaceInfo->m_pkCellDatas = new CellData[uiCellCnt];
			for(FvUInt32 i=0; i<uiCellCnt; ++i)
			{
				FvUInt32 uiCellSize;
				kMR.Read(uiCellSize);
				char* pkCellPt = kMR.GetReadPt();

				FvVector4 kRect;
				kMR.Read(kRect.x);
				kMR.Read(kRect.y);
				kMR.Read(kRect.z);
				kMR.Read(kRect.w);
				pkSpaceInfo->m_pkCellDatas[i].m_kRect = kRect;

				kMR.Read(uiTmp16);
				pkSpaceInfo->m_pkCellDatas[i].m_bLoadEntity = uiTmp16>0 ? true : false;
				kMR.Read(uiTmp16);
				pkSpaceInfo->m_pkCellDatas[i].m_bLoadZone = uiTmp16>0 ? true : false;

				//! TODO: 需要把Ghost范围参数提出来
				const float fGhostSize(100.0f);
				pkSpaceInfo->m_pkCellDatas[i].m_kKDTree.m_kGhostRect.x = kRect.x + fGhostSize;
				pkSpaceInfo->m_pkCellDatas[i].m_kKDTree.m_kGhostRect.y = kRect.y - fGhostSize;
				pkSpaceInfo->m_pkCellDatas[i].m_kKDTree.m_kGhostRect.z = kRect.z + fGhostSize;
				pkSpaceInfo->m_pkCellDatas[i].m_kKDTree.m_kGhostRect.w = kRect.w - fGhostSize;

				pkSpaceInfo->m_pkCellDatas[i].m_kAoICfg = kAoICfg;
				kMR.ReadAoI(pkSpaceInfo->m_pkCellDatas[i].m_kAoICfg);

				for(int j=0; j<4; ++j)
				{
					kMR.Read(uiTmp);
					if(uiTmp)
						pkSpaceInfo->m_pkCellDatas[i].m_kKDTree.m_pkNodes[j] =
						(CellData::GhostKDTree::Node*)kMR.GetReadPt();
					else
						pkSpaceInfo->m_pkCellDatas[i].m_kKDTree.m_pkNodes[j] = NULL;
					kMR.AddLen(uiTmp);
				}

				kMR.Read(uiTmp);
				pkSpaceInfo->m_pkCellDatas[i].m_kKDTree.m_pkIDGroups =
					(CellData::GhostKDTree::IDGroupAddr*)kMR.GetReadPt();
				kMR.AddLen(uiTmp * sizeof(CellData::GhostKDTree::IDGroupAddr));
				FvUInt16* pkIDs = (FvUInt16*)kMR.GetReadPt();
				FvUInt32 uiIDCnt(0);
				for(FvUInt32 j=0; j<uiTmp; ++j)
				{
					//! TODO: 如果uiIDCnt为奇数,导致m_pkAddr不是4字节对齐,会不会读偏掉?
					pkSpaceInfo->m_pkCellDatas[i].m_kKDTree.m_pkIDGroups[j].m_pkAddr = pkIDs + uiIDCnt;
					uiIDCnt += pkSpaceInfo->m_pkCellDatas[i].m_kKDTree.m_pkIDGroups[j].m_uiSize;
				}
				kMR.AddLen(uiIDCnt * sizeof(FvUInt16));
				kMR.SnapTo4();

				FV_ASSERT(pkCellPt + uiCellSize == kMR.GetReadPt());
			}

			m_kSpaceInfoList.push_back(pkSpaceInfo);

			kMR.SnapTo4();
			FV_ASSERT(pkSpacePt + uiSpaceSize == kMR.GetReadPt());
		}
		else
		{
			m_kSpaceInfoList.push_back(NULL);
			kMR.SetReadPt(pkSpacePt + uiSpaceSize);
		}
	}

	return true;
}

bool FvEntityManager::LoadZoneEntities(int iSpaceType, int iCellIdx)
{
	if(!m_kSpaceInfoList[iSpaceType]->m_pkCellDatas[iCellIdx].m_bLoadEntity)
		return true;

	FvCellRect&	kCellRect = m_kSpaceInfoList[iSpaceType]->m_pkCellDatas[iCellIdx].m_kRect;

	//! 读取地图中摆好的实体信息
	int iMinMapX = int(kCellRect.LeftBorder()/FV_GRID_RESOLUTION);
	int iMaxMapX = int(kCellRect.RightBorder()/FV_GRID_RESOLUTION);
	int iMinMapY = int(kCellRect.BottomBorder()/FV_GRID_RESOLUTION);
	int iMaxMapY = int(kCellRect.TopBorder()/FV_GRID_RESOLUTION);
	FvString&kMapDir = m_kSpaceInfoList[iSpaceType]->m_kSpacePath;

	for(int iGridX=iMinMapX; iGridX<iMaxMapX; ++iGridX)
	{
		for(int iGridY=iMinMapY; iGridY<iMaxMapY; ++iGridY)
		{
			LoadZoneEntity(kCellRect, kMapDir, FvZoneSerializer::OutsideZoneIdentifier(iGridX, iGridY), iSpaceType, iGridX, iGridY);
		}
	}

	FV_INFO_MSG("ParseZoneEntities, SpaceType:%d, Cnt:%d\n", iSpaceType, m_kZoneEntityInfoSpaceMap[iSpaceType].size());
	return true;
}

static std::set<FvString> g_kInnerZoneNameSet;

void FvEntityManager::LoadZoneEntity(FvCellRect& kZoneRect, const FvString& kDir, const FvString& kFileName, int iSpaceType, int iGridX, int iGridY, bool bInnerZone)
{
	FvString kFullFileName = kDir + kFileName + ".zone";

	if(bInnerZone && !g_kInnerZoneNameSet.insert(kFullFileName).second)
		return;

	FvXMLSectionPtr spSection = FvServerResource::Instance().OpenSection(kFullFileName);
	if(!spSection)
		return;

	FvMatrix kInnerZoneMat;
	if(bInnerZone)
	{
		FvXMLSectionPtr spTransf = spSection->OpenSection("transform");
		if(!spTransf)
		{
			FV_ERROR_MSG("Can NOT find \"transform\" In InnerZone %s\n", kFullFileName.c_str());
			return;
		}
		kInnerZoneMat = spTransf->AsMatrix34();
	}

	std::vector<FvXMLSectionPtr> kSecItems;
	spSection->OpenSections("entity", kSecItems);
	if(!kSecItems.empty())
	{
		FvXMLSectionPtr spEntity;
		FvXMLSectionPtr spGID;
		FvXMLSectionPtr spType;
		FvXMLSectionPtr spTransf;
		FvXMLSectionPtr spProp;
		FvMatrix kMatrix;
		ZoneEntityInfo kZoneEntityInfo;

		std::vector<FvXMLSectionPtr>::iterator itrB = kSecItems.begin();
		std::vector<FvXMLSectionPtr>::iterator itrE = kSecItems.end();
		for(; itrB!=itrE; ++itrB)
		{
			spEntity = *itrB;

			spGID = spEntity->OpenSection("guid");
			spType = spEntity->OpenSection("type");
			spTransf = spEntity->OpenSection("transform");
			spProp = spEntity->OpenSection("properties");
			FV_ASSERT(spGID);
			FV_ASSERT(spType);
			FV_ASSERT(spTransf);
			FV_ASSERT(spProp);

			kMatrix = spTransf->AsMatrix34();

			FvUniqueID kID(spGID->AsString(""));
			kZoneEntityInfo.m_uiGID[0] = kID.GetA();
			kZoneEntityInfo.m_uiGID[1] = kID.GetB();
			kZoneEntityInfo.m_uiGID[2] = kID.GetC();
			kZoneEntityInfo.m_uiGID[3] = kID.GetD();

			kZoneEntityInfo.m_kName = spType->AsString("");

			if(bInnerZone)
			{
				kMatrix.PostMultiply(kInnerZoneMat);
				kZoneEntityInfo.m_kPos.Set(kMatrix._41, kMatrix._42, kMatrix._43);
				kZoneEntityInfo.m_kDir.SetYaw(kMatrix.Roll());
				kZoneEntityInfo.m_kDir.SetPitch(kMatrix.Pitch());
				kZoneEntityInfo.m_kDir.SetRoll(kMatrix.Yaw());
			}
			else
			{
				kZoneEntityInfo.m_kPos.Set(kMatrix._41 + iGridX*FV_GRID_RESOLUTION, kMatrix._42 + iGridY*FV_GRID_RESOLUTION, kMatrix._43);
				kZoneEntityInfo.m_kDir.SetYaw(kMatrix.Roll());
				kZoneEntityInfo.m_kDir.SetPitch(kMatrix.Pitch());
				kZoneEntityInfo.m_kDir.SetRoll(kMatrix.Yaw());
			}

			if(!kZoneRect.IsPointIn(kZoneEntityInfo.m_kPos))
				continue;

			kZoneEntityInfo.m_spPropSec = FvXMLSection::CreateSection(spProp->SectionName());
			kZoneEntityInfo.m_spPropSec->CopySections(spProp);

			m_kZoneEntityInfoSpaceMap[iSpaceType].push_back(kZoneEntityInfo);
		}
		kSecItems.clear();
	}

	spSection->OpenSections("overlapper", kSecItems);
	if(!kSecItems.empty())
	{
		std::vector<FvXMLSectionPtr>::iterator itrB = kSecItems.begin();
		std::vector<FvXMLSectionPtr>::iterator itrE = kSecItems.end();
		for(; itrB!=itrE; ++itrB)
		{
			FvString kZone = (*itrB)->AsString();
			if(kZone.empty())
				continue;
			LoadZoneEntity(kZoneRect, kDir, kZone, iSpaceType, iGridX, iGridY, true);
		}
		kSecItems.clear();
	}
}

bool FvEntityManager::LoadSpaceScheme(int argc, char *argv[])
{
	//! 读取本CellApp选择的方案idx
	m_uiLoadSpaceGeometryIdx = 0;
	bool bLoadZoneEntity = FvServerConfig::Get( "LoadZoneEntity", bool(false) );
	for (int i = 1; i < argc -1; ++i)
	{
		if (_stricmp( argv[i], "-LoadSpaceScheme" ) == 0)
		{
			m_uiLoadSpaceGeometryIdx = (FvUInt8)atoi( argv[ i + 1 ] );
		}
		else if (_stricmp( argv[i], "-LoadZoneEntity" ) == 0)
		{
			bLoadZoneEntity = _stricmp( argv[i + 1], "true" ) == 0 ? true : false;
		}
	}

	FvString kFileName = "ServerConfig/LoadSpaceScheme.xml";
	FvXMLSectionPtr spSection = FvServerResource::Instance().OpenSection(kFileName);
	if(!spSection)
	{
		FV_ERROR_MSG("%s, Open File:%s Failed\n", __FUNCTION__, kFileName.c_str());
		return false;
	}

	std::vector<FvXMLSectionPtr> kSchemes;
	spSection->OpenSections("Scheme", kSchemes);
	if(kSchemes.empty())
	{
		FV_ERROR_MSG("%s, File:%s has no Scheme section\n", __FUNCTION__, kFileName.c_str());
		return false;
	}

	if(m_uiLoadSpaceGeometryIdx >= kSchemes.size())
	{
		FV_ERROR_MSG("%s, SchemeIdx:%d is too big\n", __FUNCTION__, m_uiLoadSpaceGeometryIdx);
		return false;
	}

	if(!bLoadZoneEntity)
		return true;

	//! 载入地图里的entity
	FvXMLSectionPtr spScheme = kSchemes[m_uiLoadSpaceGeometryIdx];
	std::vector<FvXMLSectionPtr> kCells;
	spScheme->OpenSections("Cell", kCells);
	if(kCells.empty())
	{
		FV_ERROR_MSG("%s, Scheme:%d has No Cell\n", __FUNCTION__, m_uiLoadSpaceGeometryIdx);
		return false;
	}

	int iCellCnt = (int)kCells.size();
	for(int iIdx(0); iIdx<iCellCnt; ++iIdx)
	{
		FvString kS = kCells[iIdx]->AsString("");
		int iSpaceType(0), iCellIdx(0);
		if(sscanf(kS.c_str(), "%d%d", &iSpaceType, &iCellIdx) != 2 ||
			iSpaceType<0 || iCellIdx<0)
		{
			FV_ERROR_MSG("%s, Scheme:%d Cell:%d data Err\n", __FUNCTION__, m_uiLoadSpaceGeometryIdx, iIdx);
			return false;
		}

		if(!m_kSpaceInfoList[iSpaceType])
		{
			FV_ERROR_MSG("%s, Scheme:%d SpaceType:%d Err\n", __FUNCTION__, m_uiLoadSpaceGeometryIdx, iSpaceType);
			return false;
		}

		if(m_kSpaceInfoList[iSpaceType]->m_uiCellCnt <= iCellIdx)
		{
			FV_ERROR_MSG("%s, Scheme:%d CellIdx:%d too Big\n", __FUNCTION__, m_uiLoadSpaceGeometryIdx, iCellIdx);
			return false;
		}

		LoadZoneEntities(iSpaceType, iCellIdx);
	}
	kCells.clear();

	g_kInnerZoneNameSet.clear();
	return true;
}

bool FvEntityManager::CheckRect(FvCellRect& kRect, int* piGridPos)
{
	FV_ASSERT(piGridPos);
	int iGrid(0);
	iGrid = int(kRect.LeftBorder()/FV_GRID_RESOLUTION);
	piGridPos[0] = iGrid;
	if(iGrid*FV_GRID_RESOLUTION != kRect.LeftBorder())
	{
		FV_ERROR_MSG("%s, SpaceRect Border isn't on Grid\n", __FUNCTION__);
		return false;
	}
	iGrid = int(kRect.RightBorder()/FV_GRID_RESOLUTION);
	piGridPos[1] = iGrid;
	if(iGrid*FV_GRID_RESOLUTION != kRect.RightBorder())
	{
		FV_ERROR_MSG("%s, SpaceRect Border isn't on Grid\n", __FUNCTION__);
		return false;
	}
	iGrid = int(kRect.BottomBorder()/FV_GRID_RESOLUTION);
	piGridPos[2] = iGrid;
	if(iGrid*FV_GRID_RESOLUTION != kRect.BottomBorder())
	{
		FV_ERROR_MSG("%s, SpaceRect Border isn't on Grid\n", __FUNCTION__);
		return false;
	}
	iGrid = int(kRect.TopBorder()/FV_GRID_RESOLUTION);
	piGridPos[3] = iGrid;
	if(iGrid*FV_GRID_RESOLUTION != kRect.TopBorder())
	{
		FV_ERROR_MSG("%s, SpaceRect Border isn't on Grid\n", __FUNCTION__);
		return false;
	}
	if(piGridPos[0] >= piGridPos[1] ||
		piGridPos[2] >= piGridPos[3])
	{
		FV_ERROR_MSG("%s, SpaceRect is invalid\n", __FUNCTION__);
		return false;
	}

	return true;
}

void FvEntityManager::AddToCellAppMgr()
{
	if(!m_pkCellAppMgr)
		return;

	FvNetBundle& bundle = m_pkCellAppMgr->Bundle();
	bundle.StartRequest(CellAppMgrInterface::AddApp, new RegisterToCellAppMgrReplyHandler());
	bundle << m_uiLoadSpaceGeometryIdx;
	m_pkCellAppMgr->DelayedSend();
}

void FvEntityManager::InformOfLoadToCellAppMgr()
{
	if(!m_pkCellAppMgr)
		return;

	CellAppMgrInterface::InformOfLoadArgs& kArgs = CellAppMgrInterface::InformOfLoadArgs::start(m_pkCellAppMgr->Bundle());
	kArgs.load = 0.0f;
	kArgs.numEntities = 0;
	m_pkCellAppMgr->DelayedSend();
}

void FvEntityManager::InitEntityIDClient()
{
	FV_ASSERT(!IsGlobalMode());
	if(!GetDBMgr().Channel().IsEstablished())
		return;

	size_t uiCriticallyLowSize	= FvServerConfig::Get( "cellApp/ids/criticallyLowSize", size_t(100) );
	size_t uiLowSize			= FvServerConfig::Get( "cellApp/ids/lowSize", size_t(2000) );
	size_t uiDesiredSize		= FvServerConfig::Get( "cellApp/ids/desiredSize", size_t(2400) );
	size_t uiHighSize			= FvServerConfig::Get( "cellApp/ids/highSize", size_t(3000) );

	m_kEntityIDClient.Init(&GetDBMgr().Channel(), DBInterface::GetIDs, DBInterface::PutIDs, uiCriticallyLowSize, uiLowSize, uiDesiredSize, uiHighSize);
}

void FvEntityManager::InitGlobalEntityIDClientAndRegister(FvNetAddress& kAddr, bool bBlock)
{
	if(m_bGlobalInit)
		return;
	m_bGlobalInit = true;

	FvNetChannel* pkChannel = FindOrCreateChannel(kAddr);

	//! 只有Global模式需要获取id
	if(IsGlobalMode())
	{
		size_t uiCriticallyLowSize	= FvServerConfig::Get( "cellApp/GlobalIDs/criticallyLowSize", size_t(100) );
		size_t uiLowSize			= FvServerConfig::Get( "cellApp/GlobalIDs/lowSize", size_t(2000) );
		size_t uiDesiredSize		= FvServerConfig::Get( "cellApp/GlobalIDs/desiredSize", size_t(2400) );
		size_t uiHighSize			= FvServerConfig::Get( "cellApp/GlobalIDs/highSize", size_t(3000) );

		m_kGlobalEntityIDClient.Init(pkChannel, GlobalAppInterface::GetEntityIDs, GlobalAppInterface::PutEntityIDs,
			uiCriticallyLowSize, uiLowSize, uiDesiredSize, uiHighSize,
			bBlock);
	}

	//! 注册
	GlobalAppInterface::RegisterArgs& kArgs = GlobalAppInterface::RegisterArgs::start(pkChannel->Bundle());
	kArgs.bBaseApp = false;
	pkChannel->DelayedSend();
}


void FvEntityManager::OnStart()
{
	FV_INFO_MSG("CellApp is starting\n");
	FV_ASSERT(!m_bStarted);
	m_bStarted = true;

	FV_ASSERT(!m_pkTimeKeeper);
	FvNetTimerID gtid = m_kNub.RegisterTimer(1000000/m_iUpdateHertz, this, reinterpret_cast<void*>(TIMEOUT_GAME_TICK));
	m_pkTimeKeeper = new FvTimeKeeper(m_kNub, gtid, m_uiTime, m_iUpdateHertz, &m_pkCellAppMgr->addr(), &CellAppMgrInterface::GameTimeReading);

	m_uiDllTickLastTime = Timestamp();

	FvLogicDllManager::Instance().OnCellAppReady();
}

FvEntityID FvEntityManager::NewEntityID()
{
	FvEntityID iEntityID = IsGlobalMode() ? m_kGlobalEntityIDClient.GetID() : m_kEntityIDClient.GetID();
	if(iEntityID == FV_NULL_ENTITY)
	{
		FV_ERROR_MSG("%s, EntityID can't make\n", __FUNCTION__);
	}
	return iEntityID;
}

void FvEntityManager::InitGlobalID()
{
	if(m_spGlobalIDs)
		return;

	char tmp[64] = {0};
	sprintf_s(tmp, sizeof(tmp), "%d.GID", m_iCellAppID);

	FILE *pkSrcFile;
	int iError;
	iError = fopen_s(&pkSrcFile, tmp, "rb");
	if(iError != 0)
	{
		m_spGlobalIDs = FvXMLSection::CreateSection("root");
		return;
	}

	Ogre::DataStreamPtr spDataStream(OGRE_NEW Ogre::FileHandleDataStream(pkSrcFile));
	m_spGlobalIDs = FvXMLSection::OpenSection(spDataStream);
	fclose(pkSrcFile);
	if(!m_spGlobalIDs)
	{
		m_spGlobalIDs = FvXMLSection::CreateSection("root");
		return;
	}

	FvXMLSectionIterator itrB = m_spGlobalIDs->Begin();
	FvXMLSectionIterator itrE = m_spGlobalIDs->End();
	for(; itrB!=itrE; ++itrB)
	{
		FvString kIdx = (*itrB)->SectionName();
		int idx(0);
		sscanf_s(kIdx.c_str(), "Idx_%d", &idx);
		FvUInt64 uMaxID = (*itrB)->As<FvUInt64>();
		m_kGlobalIDs.push_back(std::make_pair(idx, GlobalID(uMaxID, uMaxID+100)));
	}
}

void FvEntityManager::SaveGlobalID()
{
	if(!m_spGlobalIDs)
		return;

	char tmp[64] = {0};
	FvXMLSectionPtr spChild;
	m_spGlobalIDs->DelChildren();
	int iCnt(m_kGlobalIDs.size());
	for(int i=0; i<iCnt; ++i)
	{
		sprintf_s(tmp, sizeof(tmp), "Idx_%d", m_kGlobalIDs[i].first);
		spChild = m_spGlobalIDs->NewSection(tmp);
		spChild->Set<FvUInt64>(m_kGlobalIDs[i].second.m_uiMaxID);
	}

	sprintf_s(tmp, sizeof(tmp), "%d.GID", m_iCellAppID);
	m_spGlobalIDs->Save(tmp);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class CLASS_TYPE, class ARGS_TYPE>
class MessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (CLASS_TYPE::*Handler)( const ARGS_TYPE & args );

	MessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		CLASS_TYPE* pObj = CLASS_TYPE::FindMessageHandler(srcAddr, header, data);
		if(pObj)
		{
			ARGS_TYPE args;
			data >> args;
			(pObj->*handler_)( args );
		}
	}

	Handler handler_;
};

template <class CLASS_TYPE>
class VarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (CLASS_TYPE::*Handler)( FvBinaryIStream & data );

	VarLenMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		CLASS_TYPE* pObj = CLASS_TYPE::FindMessageHandler(srcAddr, header, data);
		if(pObj)
		{
			(pObj->*handler_)( data );
		}
	}

	Handler handler_;
};

template <class CLASS_TYPE>
class RawMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (CLASS_TYPE::*Handler)(  const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data  );

	RawMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		CLASS_TYPE* pObj = CLASS_TYPE::FindMessageHandler(srcAddr, header, data);
		if(pObj)
		{
			(pObj->*handler_)( srcAddr, header, data );
		}
	}

	Handler handler_;
};


class CellAppListenerMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntityManager::*Handler)(const InterfaceListenerMsg& kMsg);

	CellAppListenerMessageHandler(Handler kHandler):m_kHandler(kHandler) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		InterfaceListenerMsg kMsg;
		data >> kMsg;
		FV_ASSERT(data.RemainingLength()==0 && !data.Error());
		(FvEntityManager::Instance().*m_kHandler)(kMsg);
	}

	Handler m_kHandler;
};


class SpaceVarLenByEntityMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvSpace::*Handler)(  FvBinaryIStream & data  );

	SpaceVarLenByEntityMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		//! FvEntityID iNearbyEntityID;
		//! FvVector3 kPos;
		//! FvVector3 kDir;
		//! FvNetAddress kBaseAddr;
		//! FvEntityTypeID uiEntityTypeID;
		//! FvEntityID iEntityID;
		//! FvDatabaseID iDBID;
		//! FvUInt8 uiInitFlg;
		//! Cell部分数据

		//! 根据nearby entity查找FvCell
		FvEntityID iEntityID;
		FvVector3 kPos;
		data >> iEntityID >> kPos;

		FvEntity* pkNearbyEntity = FvEntityManager::Instance().FindEntity(iEntityID);
		if(pkNearbyEntity)
		{
			FvSpace* pkSpace = pkNearbyEntity->GetSpace();
			FvSpace::CellInfo* pkCellInfo = pkSpace->GetCellAt(kPos.x, kPos.y);
			if(pkCellInfo)
			{
				if(pkCellInfo->m_uiIdx == pkSpace->GetCell()->CellIdx())
				{
					pkSpace->CreateEntity(kPos, data);
				}
				else
				{
					FvNetChannel* pkChannel = FvEntityManager::Instance().Nub().FindChannel(pkCellInfo->m_kAddr, true);
					FvNetBundle& kBundle = pkChannel->Bundle();
					kBundle.StartMessage(CellAppInterface::CreateEntity);
					kBundle << pkSpace->SpaceID() << kPos;
					kBundle.Transfer(data, data.RemainingLength());
					pkChannel->DelayedSend();
				}
			}
			else
			{
				ReplyFailed(data);
				data.Finish();
				FV_ERROR_MSG("Pos(%f,%f,%f) isn't in Space(%d) when CreateEntity, %s\n", kPos.x, kPos.y, kPos.z, pkSpace->SpaceID(), header.MsgName());
			}
		}
		else
		{
			ReplyFailed(data);
			data.Finish();
			FV_ERROR_MSG("Couldn't find Nearby Entity(%d) when CreateEntity, %s\n", iEntityID, header.MsgName());
		}
	}

	void	ReplyFailed(FvBinaryIStream& data)
	{
		FvDirection3 kDir;
		FvNetAddress kBaseAddr;
		FvEntityTypeID uiEntityTypeID;
		FvEntityID iEntityID;
		data >> kDir >> kBaseAddr >> uiEntityTypeID >> iEntityID;

		FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kBaseAddr);
		pkChannel->Bundle().StartMessage(BaseAppIntInterface::CurrentCell);
		pkChannel->Bundle() << iEntityID << FvSpaceID(0);//! 返回0表示失败
		pkChannel->DelayedSend();
	}

	Handler handler_;
};

template <class ARGS_TYPE>
class CellEntityMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( const ARGS_TYPE & args );
	typedef std::pair<Handler, EntityReality> Pair;

	CellEntityMessageHandler( Pair kPair ) : m_kPair( kPair ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntityID id;
		data >> id;
		FvEntity* pObj = FvEntityManager::Instance().FindEntity(id);
		if(pObj)
		{
			if(m_kPair.second == GHOST_ONLY)
			{
				FV_ASSERT(!pObj->IsReal());
				ARGS_TYPE args;
				data >> args;
				(pObj->*m_kPair.first)( args );
			}
			else if(m_kPair.second == REAL_ONLY)
			{
				ARGS_TYPE args;
				data >> args;
				(pObj->*m_kPair.first)( args );
			}
			else
			{
				FV_ASSERT(pObj->HasWitness());
				ARGS_TYPE args;
				data >> args;
				(pObj->*m_kPair.first)( args );
			}
		}
		else
		{
			data.Finish();
			FV_ERROR_MSG("Can't find Entity(%d) for %s:%s\n", id, __FUNCTION__, header.MsgName());
		}
	}

	Pair	m_kPair;
};


class EntityVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( FvBinaryIStream & data, int length );
	typedef std::pair<Handler, EntityReality> Pair;

	EntityVarLenMessageHandler( Pair kPair ) : m_kPair( kPair ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntityID id;
		data >> id;
		FvEntity* pObj = FvEntityManager::Instance().FindEntity(id);
		if(pObj)
		{
			if(m_kPair.second == GHOST_ONLY)
			{
				FV_ASSERT(!pObj->IsReal());
				(pObj->*m_kPair.first)( data, data.RemainingLength() );
			}
			else if(m_kPair.second == REAL_ONLY)
			{
				(pObj->*m_kPair.first)( data, data.RemainingLength() );
			}
			else
			{
				FV_ASSERT(pObj->HasWitness());
				(pObj->*m_kPair.first)( data, data.RemainingLength() );
			}
		}
		else
		{
			data.Finish();
			FV_ERROR_MSG("Can't find Entity(%d) for %s:%s\n", id, __FUNCTION__, header.MsgName());
		}
	}

	Pair	m_kPair;
};


class EntityVarLenMethodHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( FvBinaryIStream & data, int length );
	typedef std::pair<Handler, EntityReality> Pair;

	EntityVarLenMethodHandler( Pair kPair ) : m_kPair( kPair ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntityID id;
		data >> id;
		FvEntity* pObj = FvEntityManager::Instance().FindEntity(id);
		if(pObj)
		{
			if(m_kPair.second == GHOST_ONLY)
			{
				FV_ASSERT(!pObj->IsReal());
				(pObj->*m_kPair.first)( data, data.RemainingLength() );
			}
			else if(m_kPair.second == REAL_ONLY)
			{
				(pObj->*m_kPair.first)( data, data.RemainingLength() );
			}
			else
			{
				FV_ASSERT(pObj->HasWitness());
				(pObj->*m_kPair.first)( data, data.RemainingLength() );
			}
		}
		else
		{
			FvNetAddress* pkAddr = FvEntityManager::Instance().GetIdxChannelFinder().FindAddr(id);
			if(pkAddr)
			{
				FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(*pkAddr);
				FvNetBundle& kBundle = pkChannel->Bundle();
				static FvNetInterfaceElement kInterface = CellAppInterface::RunScriptMethod;
				kInterface.SetID(header.m_uiIdentifier);
				kBundle.StartMessage(kInterface);
				kBundle << id;
				kBundle.Transfer(data, data.RemainingLength());
				pkChannel->DelayedSend();
			}
			else
			{
				FV_ERROR_MSG("Can't find Entity(%d) for %s:%s\n", id, __FUNCTION__, header.MsgName());
				FvInt32 iMethodID;
				data >> iMethodID;
				CallRPCCallbackException(iMethodID, data);
				data.Finish();
			}
		}
	}

	Pair	m_kPair;
};


class RawEntityVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	typedef std::pair<Handler, EntityReality> Pair;

	RawEntityVarLenMessageHandler( Pair kPair ) : m_kPair( kPair ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntityID id;
		data >> id;
		FvEntity* pObj = FvEntityManager::Instance().FindEntity(id);
		if(pObj)
		{
			if(m_kPair.second == GHOST_ONLY)
			{
				FV_ASSERT(!pObj->IsReal());
				(pObj->*m_kPair.first)( srcAddr, header, data );
			}
			else if(m_kPair.second == REAL_ONLY)
			{
				(pObj->*m_kPair.first)( srcAddr, header, data );
			}
			else
			{
				FV_ASSERT(pObj->HasWitness());
				(pObj->*m_kPair.first)( srcAddr, header, data );
			}
		}
		else
		{
			data.Finish();
			FV_ERROR_MSG("Can't find Entity(%d) for %s:%s\n", id, __FUNCTION__, header.MsgName());
		}
	}

	Pair	m_kPair;
};

class EntityVarLenRequestHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( const FvNetAddress & srcAddr, FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	typedef std::pair<Handler, EntityReality> Pair;

	EntityVarLenRequestHandler( Pair kPair ) : m_kPair( kPair ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntityID id;
		data >> id;
		FvEntity* pObj = FvEntityManager::Instance().FindEntity(id);
		if(pObj)
		{
			if(m_kPair.second == GHOST_ONLY)
			{
				FV_ASSERT(!pObj->IsReal());
				(pObj->*m_kPair.first)( srcAddr, header, data );
			}
			else if(m_kPair.second == REAL_ONLY)
			{
				(pObj->*m_kPair.first)( srcAddr, header, data );
			}
			else
			{
				FV_ASSERT(pObj->HasWitness());
				(pObj->*m_kPair.first)( srcAddr, header, data );
			}
		}
		else
		{
			data.Finish();
			FV_ERROR_MSG("Can't find Entity(%d) for %s:%s, Auto Reply\n", id, __FUNCTION__, header.MsgName());
			FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(srcAddr);
			FV_ASSERT(pkChannel && header.m_iReplyID!=FV_NET_REPLY_ID_NONE);
			pkChannel->Bundle().StartReply(header.m_iReplyID);
			pkChannel->Bundle() << FvUInt8(0);//! 0表示失败
			pkChannel->DelayedSend();
		}
	}

	Pair	m_kPair;
};

class VehicleMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( FvBinaryIStream & data );

	VehicleMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntityID iEntityID;
		data >> iEntityID;
		FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(iEntityID);
		if(pkEntity)
		{
			(pkEntity->*handler_)( data );
		}
		else
		{
			FV_INFO_MSG("%s, Can't find Entity:%d\n", __FUNCTION__, iEntityID);
			data.Finish();
		}
	}

	Handler handler_;
};

class EntityCallBackHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( FvBinaryIStream & data );
	typedef std::pair<Handler, EntityReality> Pair;

	EntityCallBackHandler( Pair kPair ) : m_kPair( kPair ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntityID id;
		data >> id;
		FvEntity* pObj = FvEntityManager::Instance().FindEntity(id);
		if(pObj)
		{
			if(m_kPair.second == REAL_ONLY)
			{
				(pObj->*m_kPair.first)( data );
			}
			else
			{
				FV_ASSERT(0);
			}
		}
		else
		{
			FvNetAddress* pkAddr = FvEntityManager::Instance().GetIdxChannelFinder().FindAddr(id);
			if(pkAddr)
			{
				FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(*pkAddr);
				FvNetBundle& kBundle = pkChannel->Bundle();
				kBundle.StartMessage(CellAppInterface::RPCCallBack);
				kBundle << id;
				kBundle.Transfer(data, data.RemainingLength());
				pkChannel->DelayedSend();
			}
			else
			{
				FV_ERROR_MSG("Can't find Entity(%d) for %s:%s\n", id, __FUNCTION__, header.MsgName());
				data.Finish();
			}
		}
	}

	Pair	m_kPair;
};

template <class ARGS_TYPE>
class EntityCallBackExceptionHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( const ARGS_TYPE & args );
	typedef std::pair<Handler, EntityReality> Pair;

	EntityCallBackExceptionHandler( Pair kPair ) : m_kPair( kPair ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntityID id;
		data >> id;
		FvEntity* pObj = FvEntityManager::Instance().FindEntity(id);
		if(pObj)
		{
			if(m_kPair.second == REAL_ONLY)
			{
				ARGS_TYPE args;
				data >> args;
				(pObj->*m_kPair.first)( args );
			}
			else
			{
				FV_ASSERT(0);
			}
		}
		else
		{
			FvNetAddress* pkAddr = FvEntityManager::Instance().GetIdxChannelFinder().FindAddr(id);
			if(pkAddr)
			{
				FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(*pkAddr);
				FvNetBundle& kBundle = pkChannel->Bundle();
				kBundle.StartMessage(CellAppInterface::RPCCallBackException);
				kBundle << id;
				kBundle.Transfer(data, data.RemainingLength());
				pkChannel->DelayedSend();
			}
			else
			{
				FV_ERROR_MSG("Can't find Entity(%d) for %s:%s\n", id, __FUNCTION__, header.MsgName());
				data.Finish();
			}
		}
	}

	Pair	m_kPair;
};


#define DEFINE_SERVER_HERE
#include "FvCellAppInterface.h"

#define DEFINE_INTERFACE_HERE
#include <../FvCellAppManager/FvCellAppManagerInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvDBManager/FvDBInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvBase/FvBaseAppIntInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvGlobal/FvGlobalAppInterface.h>







