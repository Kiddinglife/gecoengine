#include "FvGlobalEntityManager.h"
#include <FvEntityExport.h>
#include <FvLogicDllManager.h>
#include <FvServerConfig.h>
#include <FvServerResource.h>
#include <../FvBase/FvBaseAppIntInterface.h>
#include <../FvCell/FvCellAppInterface.h>
#include <../FvCellAppManager/FvCellAppManagerInterface.h>
#include "FvGlobalRPCCallBack.h"


void IntSignalHandler( int /*sigNum*/ )
{
	FvEntityManager * pApp = FvEntityManager::pInstance();

	if (pApp != NULL)
	{
		pApp->ShutDown();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FV_SINGLETON_STORAGE(FvEntityManager)

FvEntityManager::FvEntityManager(FvNetNub& kNub)
:m_kNub(kNub)
,m_pkEntityExports(NULL)
,m_kGlobalEntityIDPool(FV_MIN_GLOBAL_ENTITY)	//! (0x3FFFFFFF, 0x7FFFFFFF)
,m_uiTime(0)
,m_iUpdateHertz(DEFAULT_GAME_UPDATE_HERTZ)
,m_iUpdateFlag(0)
{
	FvServerConfig::Update( "gameUpdateHertz", m_iUpdateHertz );

	float fSystemManagePeriodInSeconds = FvServerConfig::Get( "GlobalApp/systemManagePeriod", 3.f );
	m_iSystemManagePeriod = int( floorf( fSystemManagePeriodInSeconds * m_iUpdateHertz + 0.5f ) );

	FV_INFO_MSG( "SystemManage Period = %f\n", fSystemManagePeriodInSeconds);
}

FvEntityManager::~FvEntityManager()
{
	FV_ASSERT(m_kGlobalMap.empty());
	FV_ASSERT(m_kEntities.empty());
	CheckDelEntities();
	FV_ASSERT(m_kDelEntityList.empty());

	{
		for(int i=0; i<(int)m_kGlobalsListeners.size(); ++i)
		{
			delete m_kGlobalsListeners[i];
		}
		m_kGlobalsListeners.clear();
	}
}

bool FvEntityManager::Init(int iArgc, char* pcArgv[])
{
	if (m_kNub.Socket() == -1)
	{
		FV_ERROR_MSG( "Failed to create Nub on interface.\n" );
		return false;
	}

	FV_INFO_MSG( "Address = %s\n", m_kNub.Address().c_str() );

	GlobalAppInterface::RegisterWithNub(m_kNub);

	FvNetReason reason = GlobalAppInterface::RegisterWithMachined( m_kNub, 0 );
	if (reason != FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG("GlobalApp::Init: Unable to register with nub. "
			"Is machined running?\n");
		return false;
	}

	{
		FvNetNub::InterfaceInfoVec kCellAppMgrs;
		reason = m_kNub.FindInterfaces(0, "CellAppMgrInterface", 0, kCellAppMgrs);

		if(reason == FV_NET_REASON_SUCCESS)
		{
			if(kCellAppMgrs.size() > 1)
			{
				FV_ERROR_MSG("%s: "
					"Found too many GlobalCellAppMgr Interface: %d\n", __FUNCTION__, kCellAppMgrs.size());
				return false;
			}
			else
			{
				m_kGlobalCellAppMgr = kCellAppMgrs.front().kAddr;
			}
		}
		else if(reason == FV_NET_REASON_TIMER_EXPIRED)
		{
			FV_INFO_MSG("%s: GlobalCellAppMgr not ready yet.\n", __FUNCTION__);
		}
		else
		{
			FV_ERROR_MSG("%s: "
				"Failed to find GlobalCellAppMgr interface: %s\n",
				__FUNCTION__, NetReasonToString((FvNetReason&)reason));
			return false;
		}

		m_kNub.RegisterBirthListener(GlobalAppInterface::HandleCellAppMgrBirth, "CellAppMgrInterface", 0);
	}

	void* pkEntityExports(NULL);
	if(!FvLogicDllManager::Instance().DllExport(pkEntityExports))
	{
		FV_ERROR_MSG( "GlobalApp::Init: "
			"Get Dll Export Failed\n");
		return false;
	}
	m_pkEntityExports = (FvAllGlobalEntityExports*)pkEntityExports;

	FvString kEntityDefMD5Digest;
	if(!FvServerConfig::Update( "EntityDefMD5Digest", kEntityDefMD5Digest ))
	{
		FV_ERROR_MSG( "GlobalApp::Init: "
			"Get EntityDefMD5Digest from Server XML Failed\n");
		return false;
	}

	if(m_pkEntityExports->kMD5Digest.Quote() != kEntityDefMD5Digest)
	{
		FV_ERROR_MSG( "GlobalApp::Init: "
			"EntityDefMD5Digest in Dll isn't Equal to the one from Server XML\n");
		return false;
	}

	FvLogicDllManager::Instance().InitializeLogic();

	signal( SIGINT, ::IntSignalHandler );
#ifndef _WIN32
	signal( SIGHUP, ::IntSignalHandler );
#endif //ndef _WIN32


	m_kNub.RegisterTimer(1000000/m_iUpdateHertz, this, reinterpret_cast<void*>(TIMEOUT_GAME_TICK));
	FvLogicDllManager::Instance().OnGlobalAppReady();

	return true;
}

void FvEntityManager::ShutDown()
{
	m_kNub.BreakProcessing();
}

FvEntity* FvEntityManager::FindEntity(FvEntityID iID) const
{
	Entities::const_iterator itr = m_kEntities.find(iID);
	if(itr != m_kEntities.end())
		return itr->second.GetObject();
	else
		return NULL;
}

FvEntity* FvEntityManager::CreateEntity(const char* pcEntityTypeName, FvUInt8 uiInitFlg)
{
	FvGlobalEntityExport* pkEntityExport = m_pkEntityExports->FindByName(pcEntityTypeName);
	if(!pkEntityExport)
	{
		FV_ERROR_MSG("%s, EntityTypeName(%s) is't Exist\n", __FUNCTION__, pcEntityTypeName);
		return NULL;
	}

	return CreateEntity(pkEntityExport->uiTypeID, uiInitFlg);
}

FvEntity* FvEntityManager::CreateEntity(FvEntityTypeID uiEntityType, FvUInt8 uiInitFlg)
{
	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		FV_ERROR_MSG("%s, EntityType(%d) is invalid\n", __FUNCTION__, uiEntityType);
		return NULL;
	}

	FvEntityID iID = m_kGlobalEntityIDPool.GetID();
	if(iID == 0)
		return NULL;

	FvGlobalEntityExport* pkEntityExport = m_pkEntityExports->ppkEntityExport[uiEntityType];
	FvEntity* pkEntity = pkEntityExport->pFunCreateEntity();
	pkEntity->Init(iID, pkEntityExport);
	pkEntity->Initialize(uiInitFlg);

	return pkEntity;
}

void FvEntityManager::CreateEntityInNewGlobalSpace(const FvAllData& kAllData, FvUInt8 uiInitFlg)
{
	if(!kAllData.HasCell())
	{
		FV_ERROR_MSG("%s, EntityType:%d has NOT Cell\n", __FUNCTION__, kAllData.GetType());
		return;
	}

	if(m_kGlobalCellAppMgr.IsNone())
	{
		FV_ERROR_MSG("%s, GlobalCellAppMgr is NOT Ready\n", __FUNCTION__);
		return;
	}

	//! FvUInt16 uiSpaceType;
	//! FvVector3 kPos;
	//! FvVector3 kDir;
	//! FvNetAddress kBaseAddr;
	//! FvEntityTypeID uiEntityTypeID;
	//! FvEntityID iEntityID;
	//! FvDatabaseID iDBID;
	//! FvUInt8 uiInitFlg;
	//! Cell部分数据
	FvNetChannel& kChannel = m_kNub.FindOrCreateChannel(m_kGlobalCellAppMgr);
	FvNetBundle& kBundle = kChannel.Bundle();
	kBundle.StartMessage(CellAppMgrInterface::CreateEntityInNewSpace);
	kBundle << kAllData.m_uiSpaceType << kAllData.m_kPos << kAllData.m_kDir << FvNetAddress::NONE
		<< kAllData.GetType() << FV_NULL_ENTITY << FvDatabaseID(0) << uiInitFlg;
	kAllData.SerializeToStreamForCellData(kBundle);
	kChannel.DelayedSend();
}

FvMailBox* FvEntityManager::FindGlobalEntity(const char* pcLabel) const
{
	if(!pcLabel || !pcLabel[0])
		return NULL;

	GlobalMap::const_iterator itr = m_kGlobalMap.find(pcLabel);
	if(itr == m_kGlobalMap.end())
		return NULL;
	else
		return const_cast<FvMailBox*>(&(itr->second));
}

FvMailBox* FvEntityManager::FindGlobalEntity(const FvString& kLabel) const
{
	if(kLabel.empty())
		return NULL;

	GlobalMap::const_iterator itr = m_kGlobalMap.find(kLabel);
	if(itr == m_kGlobalMap.end())
		return NULL;
	else
		return const_cast<FvMailBox*>(&(itr->second));
}

void FvEntityManager::HandleCellAppMgrBirth(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);

	FV_INFO_MSG("%s: %s, UserID:%d\n", __FUNCTION__, kAddr.c_str(), kMsg.uiUserID);

	if(kMsg.uiUserID == 0)
	{
		m_kGlobalCellAppMgr = kAddr;
	}
}

void FvEntityManager::Register(const FvNetAddress& kAddr, const GlobalAppInterface::RegisterArgs& kArgs)
{
	FV_INFO_MSG("Register, bBaseApp:%d, Addr:%s\n", kArgs.bBaseApp, kAddr.c_str());
	m_kGlobalsListeners.push_back(new GlobalsListener(kArgs.bBaseApp, kAddr));

	if(m_kGlobalMap.empty())
		return;

	//! 发送已有的GlobalMailBoxes
	FvNetChannel* pkChannel = FindOrCreateChannel(kAddr);
	FvNetBundle& kBundle = pkChannel->Bundle();

	if(kArgs.bBaseApp)
	{
		GlobalMap::iterator itrB = m_kGlobalMap.begin();
		GlobalMap::iterator itrE = m_kGlobalMap.end();
		while(itrB != itrE)
		{
			kBundle.StartMessage(BaseAppIntInterface::AddGlobalMailBox);
			kBundle << itrB->first << itrB->second;
			++itrB;
		}
	}
	else
	{
		GlobalMap::iterator itrB = m_kGlobalMap.begin();
		GlobalMap::iterator itrE = m_kGlobalMap.end();
		while(itrB != itrE)
		{
			kBundle.StartMessage(CellAppInterface::AddGlobalMailBox);
			kBundle << itrB->first << itrB->second;
			++itrB;
		}
	}

	pkChannel->DelayedSend();
}

void FvEntityManager::Deregister(const FvNetAddress& kAddr, const GlobalAppInterface::DeregisterArgs& kArgs)
{
	bool bFind(false);
	for(int i=0; i<(int)m_kGlobalsListeners.size(); ++i)
	{
		if(m_kGlobalsListeners[i]->Address() == kAddr)
		{
			FV_INFO_MSG("DeRegister, bBaseApp:%d, Addr:%s\n", m_kGlobalsListeners[i]->IsBaseApp(), kAddr.c_str());
			bFind = true;
			break;
		}
	}

	if(!bFind)
	{
		FV_ERROR_MSG("DeRegister, Can't find listener Addr:%s\n", kAddr.c_str());
	}
}

void FvEntityManager::PutEntityIDs(const FvNetAddress& kAddr, const FvNetUnpackedMessageHeader& kHeader, FvBinaryIStream& kData)
{
	FV_INFO_MSG("PutEntityIDs(%d) from Addr:%s\n", kData.RemainingLength()/sizeof(FvEntityID), kAddr.c_str());
	m_kGlobalEntityIDPool.PutIDsFromStream(kData);
}

void FvEntityManager::GetEntityIDs(const FvNetAddress& kAddr, const FvNetUnpackedMessageHeader& kHeader, FvBinaryIStream& kData)
{
	int iNum(0);
	kData >> iNum;

	FV_INFO_MSG("GetEntityIDs(%d) from Addr:%s\n", iNum, kAddr.c_str());
	FV_ASSERT(kHeader.m_iReplyID != FvNetUnpackedMessageHeader::NO_REPLY);

	FvNetChannel* pkChannel = FindOrCreateChannel(kAddr);
	pkChannel->Bundle().StartReply(kHeader.m_iReplyID);
	m_kGlobalEntityIDPool.GetIDsToStream(iNum, pkChannel->Bundle());
	pkChannel->DelayedSend();
}

const FvNetAddress& FvEntityManager::Address() const
{
	return m_kNub.Address();
}

void FvEntityManager::AddEntity(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity);

	if(!m_iUpdateFlag)
	{
		bool bRet = m_kEntities.insert(std::make_pair(pkEntity->GetEntityID(),pkEntity)).second;
		FV_ASSERT(bRet);
	}
	else
	{
		m_kAddDelEntityList.push_back(AddDelEntity(pkEntity,1));
	}
}

void FvEntityManager::DelEntity(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity);

	if(!m_iUpdateFlag)
	{
		FV_ASSERT(pkEntity);
		pkEntity->IncRef();	//! 增加引用计数,防止被析构
		int s = (int)m_kEntities.erase(pkEntity->GetEntityID());
		FV_ASSERT(s == 1);
		m_kDelEntityList.push_back(pkEntity);

		FV_INFO_MSG("Entities:%d\n", m_kEntities.size());
	}
	else
	{
		m_kAddDelEntityList.push_back(AddDelEntity(pkEntity,0));
	}
}

bool FvEntityManager::AddGlobalMailBox(const char* pcName, FvMailBox& kMailBox)
{
	if(!pcName || !pcName[0])
		return false;

	GlobalMap::iterator itr = m_kGlobalMap.find(pcName);
	if(itr != m_kGlobalMap.end())
		return false;

	m_kGlobalMap.insert(std::make_pair(pcName, kMailBox));

	BroadCastGlobalMailBox(true, pcName, &kMailBox);
	return true;
}

bool FvEntityManager::DelGlobalMailBox(const char* pcName, FvMailBox& kMailBox)
{
	if(!pcName || !pcName[0])
		return false;

	GlobalMap::iterator itr = m_kGlobalMap.find(pcName);
	if(itr != m_kGlobalMap.end())
	{
		if(itr->second != kMailBox)
			return false;

		m_kGlobalMap.erase(itr);
	}
	else
	{
		return false;
	}

	BroadCastGlobalMailBox(false, pcName, NULL);
	return true;
}

void FvEntityManager::OnEntityDestroyed(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity);

	std::vector<FvString> kLabels;

	GlobalMap::iterator itrB = m_kGlobalMap.begin();
	GlobalMap::iterator itrE = m_kGlobalMap.end();
	while(itrB != itrE)
	{
		if(itrB->second == pkEntity->GlobalMBInEntity())
		{
			kLabels.push_back(itrB->first);
			itrB = m_kGlobalMap.erase(itrB);
		}
		else
		{
			++itrB;
		}
	}

	if(!kLabels.empty())
	{
		for(int i=0; i<(int)kLabels.size(); ++i)
		{
			BroadCastGlobalMailBox(false, kLabels[i].c_str(), NULL);
		}
	}
}

int FvEntityManager::HandleTimeout( FvNetTimerID id, void * arg )
{
	switch(reinterpret_cast<FvUIntPtr>(arg))
	{
	case TIMEOUT_GAME_TICK:
		{
			++m_uiTime;

			if(m_uiTime % m_iSystemManagePeriod == 0)
			{
				CheckDelEntities();
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

void FvEntityManager::ClearAddDelEntityList()
{
	if(m_iUpdateFlag && m_kAddDelEntityList.empty())
		return;

	for(int i=0; i<(int)m_kAddDelEntityList.size(); ++i)
	{
		if(m_kAddDelEntityList[i].m_iFlag == 0)
		{
			DelEntity(m_kAddDelEntityList[i].m_pkEntity);
		}
		else if(m_kAddDelEntityList[i].m_iFlag == 1)
		{
			AddEntity(m_kAddDelEntityList[i].m_pkEntity);
		}
	}
	m_kAddDelEntityList.clear();
}

void FvEntityManager::CheckDelEntities()
{
	for(int i=0; i<(int)m_kDelEntityList.size(); )
	{
		FvEntity* pkEntity = m_kDelEntityList[i];
		if(pkEntity->RefCount() == 1)
		{
			m_kGlobalEntityIDPool.PutID(pkEntity->GetEntityID());//! TODO: 短期内id可能会重复
			delete pkEntity;
			m_kDelEntityList[i] = m_kDelEntityList[m_kDelEntityList.size()-1];
			m_kDelEntityList.pop_back();
		}
		else
		{
			++i;
		}
	}
}

void FvEntityManager::BroadCastGlobalMailBox(bool bAdd, const char* pcName, FvMailBox* pkMailBox)
{
	FV_ASSERT(pcName && pcName[0]);

	if(bAdd)
	{
		FV_ASSERT(pkMailBox);
		for(int i=0; i<(int)m_kGlobalsListeners.size(); ++i)
		{
			m_kGlobalsListeners[i]->AddGlobal(pcName, *pkMailBox);
		}
	}
	else
	{
		for(int i=0; i<(int)m_kGlobalsListeners.size(); ++i)
		{
			m_kGlobalsListeners[i]->RemoveGlobal(pcName);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FvEntityManager::GlobalsListener::GlobalsListener(bool bBaseApp, const FvNetAddress& kAddr)
:m_bBaseApp(bBaseApp)
{
	m_pkChannel = FvEntityManager::Instance().FindOrCreateChannel(kAddr);
}

FvEntityManager::GlobalsListener::~GlobalsListener()
{

}

void FvEntityManager::GlobalsListener::AddGlobal(const char* pcName, FvMailBox& kMailBox)
{
	FV_ASSERT(pcName && pcName[0]);

	FV_ASSERT(m_pkChannel);
	FvNetBundle& kBundle = m_pkChannel->Bundle();

	if(m_bBaseApp)
	{
		kBundle.StartMessage(BaseAppIntInterface::AddGlobalMailBox);
	}
	else
	{
		kBundle.StartMessage(CellAppInterface::AddGlobalMailBox);
	}

	kBundle << pcName << kMailBox;
	m_pkChannel->DelayedSend();
}

void FvEntityManager::GlobalsListener::RemoveGlobal(const char* pcName)
{
	FV_ASSERT(pcName && pcName[0]);

	FV_ASSERT(m_pkChannel);
	FvNetBundle& kBundle = m_pkChannel->Bundle();

	if(m_bBaseApp)
	{
		kBundle.StartMessage(BaseAppIntInterface::DelGlobalMailBox);
	}
	else
	{
		kBundle.StartMessage(CellAppInterface::DelGlobalMailBox);
	}

	kBundle << pcName;
	m_pkChannel->DelayedSend();
}

const FvNetAddress& FvEntityManager::GlobalsListener::Address() const
{
	FV_ASSERT(m_pkChannel);
	return m_pkChannel->addr();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GlobalAppListenerMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntityManager::*Handler)(const InterfaceListenerMsg& kMsg);

	GlobalAppListenerMessageHandler(Handler kHandler):m_kHandler(kHandler) {}

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


template <class ARGS_TYPE>
class GlobalAppMessageWithAddrHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntityManager::*Handler)(const FvNetAddress& kAddr, const ARGS_TYPE& kArgs);

	GlobalAppMessageWithAddrHandler(Handler pkHandler):m_pkHandler(pkHandler) {}

private:
	virtual void HandleMessage(const FvNetAddress& kSrcAddr,
		FvNetUnpackedMessageHeader& /*kHeader*/,
		FvBinaryIStream& kData)
	{
		ARGS_TYPE kArgs;
		kData >> kArgs;
		(FvEntityManager::Instance().*m_pkHandler)(kSrcAddr, kArgs);
	}

	Handler m_pkHandler;
};

class GlobalAppRawMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntityManager::*Handler)(const FvNetAddress& kAddr, const FvNetUnpackedMessageHeader& kHeader, FvBinaryIStream& kData);

	GlobalAppRawMessageHandler(Handler pkHandler):m_pkHandler(pkHandler) {}

private:
	virtual void HandleMessage(const FvNetAddress& kSrcAddr,
		FvNetUnpackedMessageHeader& kHeader,
		FvBinaryIStream& kData)
	{
		(FvEntityManager::Instance().*m_pkHandler)(kSrcAddr, kHeader, kData);
	}

	Handler m_pkHandler;
};

class GlobalEntityMethodHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)(FvBinaryIStream& kData);

	GlobalEntityMethodHandler(Handler pkHandler):m_pkHandler(pkHandler) {}

private:
	virtual void HandleMessage(const FvNetAddress& kSrcAddr,
		FvNetUnpackedMessageHeader& kHeader,
		FvBinaryIStream& kData)
	{
		FvEntityID iEntityID;
		kData >> iEntityID;
		FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(iEntityID);
		if(pkEntity)
		{
			(pkEntity->*m_pkHandler)(kData);
		}
		else
		{
			FV_INFO_MSG("%s, Can't find Entity:%d\n", __FUNCTION__, iEntityID);
			FvInt32 iMethodIdx;
			kData >> iMethodIdx;
			CallRPCCallbackException(iMethodIdx, kData);
			kData.Finish();
		}
	}

	Handler m_pkHandler;
};

class GlobalEntityVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)(FvBinaryIStream& kData);

	GlobalEntityVarLenMessageHandler(Handler pkHandler):m_pkHandler(pkHandler) {}

private:
	virtual void HandleMessage(const FvNetAddress& kSrcAddr,
		FvNetUnpackedMessageHeader& kHeader,
		FvBinaryIStream& kData)
	{
		FvEntityID iEntityID;
		kData >> iEntityID;
		FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(iEntityID);
		if(pkEntity)
		{
			(pkEntity->*m_pkHandler)(kData);
		}
		else
		{
			FV_INFO_MSG("%s, Can't find Entity:%d\n", __FUNCTION__, iEntityID);
			kData.Finish();
		}
	}

	Handler m_pkHandler;
};

template <class ARGS_TYPE>
class GlobalEntityCallBackExceptionMessage : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)(const ARGS_TYPE& kArgs);

	GlobalEntityCallBackExceptionMessage(Handler pkHandler):m_pkHandler(pkHandler) {}

private:
	virtual void HandleMessage(const FvNetAddress& kSrcAddr,
		FvNetUnpackedMessageHeader& /*kHeader*/,
		FvBinaryIStream& kData)
	{
		FvEntityID iEntityID;
		ARGS_TYPE kArgs;
		kData >> iEntityID >> kArgs;
		FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(iEntityID);
		if(pkEntity)
		{
			(pkEntity->*m_pkHandler)(kArgs);
		}
		else
		{
			FV_INFO_MSG("%s, Can't find Entity:%d\n", __FUNCTION__, iEntityID);
			kData.Finish();
		}
	}

	Handler m_pkHandler;
};



#define DEFINE_SERVER_HERE
#include "FvGlobalAppInterface.h"

#define DEFINE_INTERFACE_HERE
#include <../FvBase/FvBaseAppIntInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvCell/FvCellAppInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvCellAppManager/FvCellAppManagerInterface.h>
