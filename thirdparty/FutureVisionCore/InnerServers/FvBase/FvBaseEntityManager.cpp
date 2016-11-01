#include "FvBaseEntityManager.h"
#include "FvClientInterface.h"
#include <../FvDBManager/FvDBInterface.h>
#include <../FvCell/FvCellAppInterface.h>
#include <../FvDBManager/FvIDatabase.h>
#include <../FvCellAppManager/FvCellAppManagerInterface.h>
#include <../FvBaseAppManager/FvBaseAppManagerInterface.h>
#include <../FvGlobal/FvGlobalAppInterface.h>
#include <FvWriteDB.h>
#include <FvServerResource.h>
#include <FvEntityDefConstants.h>
#include <FvLogicDllManager.h>
#include <FvServerConfig.h>
#include <FvTimeKeeper.h>
#include "FvProxy.h"
#include "FvBaseMailBox.h"
#include "FvGlobalBases.h"
#include <FvEntityData.h>
#include <FvEntityExport.h>
#include "FvBaseRPCCallBack.h"


//#define __DEV_MODE__	//! 定义开发模式,可以只重启Base/Cell


void IntSignalHandler( int /*sigNum*/ )
{
	FvEntityManager * pApp = FvEntityManager::pInstance();

	if (pApp != NULL)
	{
		//pApp->GetIdxChannelFinder().Dump();
		pApp->ShutDown();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RegisterToBaseAppMgrReplyHandler : public FvNetReplyMessageHandler
{
public:
	RegisterToBaseAppMgrReplyHandler()
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		FV_ASSERT(data.RemainingLength() >= sizeof(FvBaseAppInitData));
		FvBaseAppInitData initData;
		data >> initData;
		FvEntityManager::Instance().FinishInit(initData);

		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("Registe to BaseAppMgr Exception\n");
		delete this;
	}
};

class CreateBaseReplyHandler : public FvNetReplyMessageHandler
{
public:
	CreateBaseReplyHandler(CreateBaseCallBack* pkCallBack)
	:m_pkCallBack(pkCallBack)
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		if(m_pkCallBack)
		{
			FvMailBox kMB;
			data >> kMB;

			if(!kMB.GetAddr().IsNone())
				m_pkCallBack->HandleCallBack(&kMB);
			else
				m_pkCallBack->HandleCallBack(NULL);
		}
		data.Finish();
		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("CreateBaseReplyHandler Exception\n");
		if(m_pkCallBack)
			m_pkCallBack->HandleCallBack(NULL);
		delete this;
	}

	CreateBaseCallBack*	m_pkCallBack;
};

class CreateBaseFromDBReplyHandler : public FvNetReplyMessageHandler
{
public:
	CreateBaseFromDBReplyHandler(CreateBaseFromDBCallBack* pkCallBack)
	:m_pkCallBack(pkCallBack)
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		if(m_pkCallBack)
		{
			FvInt8 iState;
			data >> iState;

			if(iState == 0)//! DB中无此实体
			{
				m_pkCallBack->HandleCallBack(NULL, 0, false);
			}
			else if(iState == 1)//! DB中有此实体,第一次载入
			{
				FvDatabaseID iDBID;
				FvMailBox kMB;
				data >> iDBID >> kMB;
				m_pkCallBack->HandleCallBack(&kMB, iDBID, false);
			}
			else if(iState == 2)//! DB中有此实体,已载入
			{
				FvDatabaseID iDBID;
				FvMailBox kMB;
				data >> iDBID >> kMB;
				m_pkCallBack->HandleCallBack(&kMB, iDBID, true);
			}
			else
			{
				FV_ASSERT(false);
			}
		}
		data.Finish();
		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("CreateBaseFromDBReplyHandler Exception\n");
		if(m_pkCallBack)
			m_pkCallBack->HandleCallBack(NULL, 0, false);
		delete this;
	}

	CreateBaseFromDBCallBack*	m_pkCallBack;
};

class CreateBaseLocallyFromDBReplyHandler : public FvNetReplyMessageHandler
{
public:
	CreateBaseLocallyFromDBReplyHandler(FvEntityTypeID uiTypeID, FvEntityID iEntityID, FvUInt8 uiInitFlg, CreateBaseFromDBCallBack* pkCallBack)
	:m_uiTypeID(uiTypeID)
	,m_iEntityID(iEntityID)
	,m_uiInitFlg(uiInitFlg)
	,m_pkCallBack(pkCallBack)
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		FvUInt8 status;
		data >> status;

		//! 第一次load
		if(status == FvDatabaseLoginStatus::LOGGED_ON)
		{
			FvDatabaseID iDBID;
			data >> iDBID;
			FvEntity* pkEntity = FvEntityManager::Instance().CreateEntityCommon(m_uiTypeID, m_iEntityID, iDBID, data, true, m_uiInitFlg);
			FV_ASSERT(pkEntity);
			FvMailBox kMB;
			kMB.Set(m_iEntityID, m_uiTypeID, FvEntityManager::Instance().IntNub().Address(), FvEntityMailBoxRef::BASE);
			if(m_pkCallBack)
				m_pkCallBack->HandleCallBack(&kMB, iDBID, false);
		}
		else if(status == FvDatabaseLoginStatus::LOGIN_REJECTED_ALREADY_LOGGED_IN)//! 已被load
		{
			FvDatabaseID iDBID;
			FvMailBox kMB;
			data >> iDBID >> kMB;
			if(m_pkCallBack)
				m_pkCallBack->HandleCallBack(&kMB, iDBID, true);
		}
		else if(status == FvDatabaseLoginStatus::LOGIN_REJECTED_NO_SUCH_USER)//! 实体不存在
		{
			FvString err;
			data >> err;
			if(m_pkCallBack)
				m_pkCallBack->HandleCallBack(NULL, 0, false);
		}
		else
		{
			FV_ASSERT(false);
		}

		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("CreateBaseFromDBReplyHandler Exception\n");
		if(m_pkCallBack)
			m_pkCallBack->HandleCallBack(NULL, 0, false);
		delete this;
	}

	FvEntityTypeID				m_uiTypeID;
	FvEntityID					m_iEntityID;
	FvUInt8						m_uiInitFlg;
	CreateBaseFromDBCallBack*	m_pkCallBack;
};

class LookUpBaseReplyHandler : public FvNetReplyMessageHandler
{
public:
	LookUpBaseReplyHandler(LookUpBaseCallBack* pkCallBack)
	:m_pkCallBack(pkCallBack)
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		int iLen = data.RemainingLength();

		//! 实体未load
		if(iLen == 0)
		{
			m_pkCallBack->HandleCallBack(NULL, true);
		}
		else if(iLen == sizeof(FvInt32))//! 失败
		{
			data.Finish();
			m_pkCallBack->HandleCallBack(NULL, false);
		}
		else if(iLen == sizeof(FvEntityMailBoxRef))//! 实体已load
		{
			FvMailBox kMB;
			data >> kMB;
			m_pkCallBack->HandleCallBack(&kMB, true);
		}
		else
		{
			FV_ASSERT(false);
		}

		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("LookUpBaseReplyHandler Exception\n");
		m_pkCallBack->HandleCallBack(NULL, false);
		delete this;
	}

	LookUpBaseCallBack*	m_pkCallBack;
};

class LookUpDBIDByNameReplyHandler : public FvNetReplyMessageHandler
{
public:
	LookUpDBIDByNameReplyHandler(LookUpDBIDCallBack* pkCallBack)
	:m_pkCallBack(pkCallBack)
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		FvDatabaseID iDBID;
		data >> iDBID;
		m_pkCallBack->HandleCallBack(iDBID);

		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("LookUpDBIDByNameReplyHandler Exception\n");
		m_pkCallBack->HandleCallBack(0);
		delete this;
	}

	LookUpDBIDCallBack*	m_pkCallBack;
};

class DeleteBaseByDBIDReplyHandler : public FvNetReplyMessageHandler
{
public:
	DeleteBaseByDBIDReplyHandler(DeleteBaseCallBack* pkCallBack)
	:m_pkCallBack(pkCallBack)
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		int iLen = data.RemainingLength();

		//! 成功
		if(iLen == 0)
		{
			m_pkCallBack->HandleCallBack(true, NULL);
		}
		else if(iLen == sizeof(FvInt32))//! 失败
		{
			data.Finish();
			m_pkCallBack->HandleCallBack(false, NULL);
		}
		else if(iLen == sizeof(FvEntityMailBoxRef))//! 实体已load,不能删除
		{
			FvMailBox kMB;
			data >> kMB;
			m_pkCallBack->HandleCallBack(false, &kMB);
		}
		else
		{
			FV_ASSERT(false);
		}

		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("DeleteBaseByDBIDReplyHandler Exception\n");
		m_pkCallBack->HandleCallBack(false, NULL);
		delete this;
	}

	DeleteBaseCallBack*	m_pkCallBack;
};

class CreateBaseLocallyFromDBWithReplyReplyHandler : public FvNetReplyMessageHandler
{
public:
	CreateBaseLocallyFromDBWithReplyReplyHandler(FvEntityTypeID uiTypeID, FvEntityID iEntityID, FvUInt8 uiInitFlg, const FvNetAddress& kAddr, FvNetReplyID iReplyID)
	:m_uiTypeID(uiTypeID)
	,m_iEntityID(iEntityID)
	,m_uiInitFlg(uiInitFlg)
	,m_kAddr(kAddr)
	,m_iReplyID(iReplyID)
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kAddr);
		FvNetBundle& kBundle = pkChannel->Bundle();
		kBundle.StartReply(m_iReplyID);

		FvUInt8 status;
		data >> status;

		//! 第一次load
		if(status == FvDatabaseLoginStatus::LOGGED_ON)
		{
			FvDatabaseID iDBID;
			data >> iDBID;
			FvEntity* pkEntity = FvEntityManager::Instance().CreateEntityCommon(m_uiTypeID, m_iEntityID, iDBID, data, true, m_uiInitFlg);
			if(pkEntity)
			{
				FvEntityMailBoxRef ref;
				ref.Init(m_iEntityID, FvEntityManager::Instance().IntNub().Address(), FvEntityMailBoxRef::BASE, m_uiTypeID);

				kBundle << FvInt8(1) << iDBID << ref;
			}
			else
			{
				kBundle << FvInt8(0);//! 创建失败
			}
		}
		else if(status == FvDatabaseLoginStatus::LOGIN_REJECTED_ALREADY_LOGGED_IN)//! 已被load
		{
			kBundle << FvInt8(2);
			kBundle.Transfer(data, data.RemainingLength());
		}
		else if(status == FvDatabaseLoginStatus::LOGIN_REJECTED_NO_SUCH_USER)//! 实体不存在
		{
			FvString err;
			data >> err;

			kBundle << FvInt8(0);
		}
		else
		{
			FV_ASSERT(false);
		}

		pkChannel->DelayedSend();
		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("CreateBaseLocallyFromDBWithReplyReplyHandler Exception\n");
		delete this;
	}

	FvEntityTypeID		m_uiTypeID;
	FvEntityID			m_iEntityID;
	FvUInt8				m_uiInitFlg;
	const FvNetAddress	m_kAddr;
	FvNetReplyID		m_iReplyID;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FV_SINGLETON_STORAGE(FvEntityManager)

FvEntityManager::FvEntityManager(FvNetNub& kIntNub, FvUInt16 uiExternalPort)
:m_iBaseAppID(FvBaseAppID(-1))
,m_kIntNub(kIntNub)
,m_kExtNub(kIntNub.GetMachinePort(),
			kIntNub.GetUserID(),
			htons(uiExternalPort),
			FvServerConfig::Get( "baseApp/externalInterface",
				FvServerConfig::Get( "externalInterface" ) ).c_str())
,m_kBaseAppMgr(kIntNub)
,m_kCellAppMgr(FvNetAddress::NONE)
,m_kGlobalCellAppMgr(FvNetAddress::NONE)
,m_pkEntityExports(NULL)
,m_pkEntityForCall(NULL)
,m_pkBaseAppData(NULL)
,m_pkGlobalData(NULL)
,m_pkGlobalBases(NULL)
,m_uiTime(0)
,m_pkTimeKeeper(NULL)
,m_iUpdateHertz(DEFAULT_GAME_UPDATE_HERTZ)
,m_bIsBootstrap(false)
,m_fLoad(0.0f)
,m_kCreateBaseAnywhereAddr(FvNetAddress::NONE)
,m_kIdxChannelFinder(kIntNub)
,m_iUpdateFlag(0)
,m_bLocalMailBoxAsRemote(false)
,m_uiDllTickLastTime(0)
{
	m_kExtNub.IsExternal(true);

	m_pkGlobalBases = new FvGlobalBases();

	FvServerConfig::Update( "gameUpdateHertz", m_iUpdateHertz );

	float fSystemManagePeriodInSeconds = FvServerConfig::Get( "baseApp/systemManagePeriod", 3.f );
	m_iSystemManagePeriod = int( floorf( fSystemManagePeriodInSeconds * m_iUpdateHertz + 0.5f ) );
	float fProxyTimeoutPeriodInSeconds = FvServerConfig::Get( "baseApp/proxyTimeoutPeriod", 30.f );
	m_iProxyTimeoutPeriod = int( floorf( fProxyTimeoutPeriodInSeconds * m_iUpdateHertz + 0.5f ) );
	float fTimeSyncPeriodInSeconds = FvServerConfig::Get( "baseApp/timeSyncPeriod", 60.f );
	m_iSyncTimePeriod = int( floorf( fTimeSyncPeriodInSeconds * m_iUpdateHertz + 0.5f ) );
	m_bLocalMailBoxAsRemote = FvServerConfig::Get( "baseApp/LocalMailBoxAsRemote", false );

	FV_INFO_MSG( "SystemManage Period = %f\n", fSystemManagePeriodInSeconds);
	FV_INFO_MSG( "Proxy Timeout Period = %f\n", fProxyTimeoutPeriodInSeconds);
	FV_INFO_MSG( "Time Sync Period = %f\n", fTimeSyncPeriodInSeconds);
	FV_INFO_MSG( "LocalMailBoxAsRemote = %d\n", m_bLocalMailBoxAsRemote);

	m_kExtMappingAddr.m_uiIP = m_kExtMappingAddr.m_uiPort = m_kExtMappingAddr.m_uiSalt = 0;
	FvString kMappingIP = FvServerConfig::Get( "baseApp/externalMappingIP" );
	if(!kMappingIP.empty())
	{
		int a1, a2, a3, a4;
		if(sscanf(kMappingIP.c_str(), "%d.%d.%d.%d", &a1, &a2, &a3, &a4) == 4)
		{
			FvUInt32 uiIP = (a1 << 24)|(a2 << 16)|(a3 << 8)|a4;
			m_kExtMappingAddr.m_uiIP = ntohl(uiIP);
			m_kExtMappingAddr.m_uiPort = m_kExtNub.Address().m_uiPort;
		}
	}
	if(m_kExtMappingAddr.IsNone())
		m_kExtMappingAddr = m_kExtNub.Address();
}

FvEntityManager::~FvEntityManager()
{
	if(m_pkTimeKeeper)
	{
		delete m_pkTimeKeeper;
		m_pkTimeKeeper = NULL;
	}

	if(m_pkGlobalBases)
	{
		delete m_pkGlobalBases;
		m_pkGlobalBases = NULL;
	}

#ifndef __DEV_MODE__
	FV_ASSERT(m_kEntities.empty());
	FV_ASSERT(m_kProxies.empty());
	FV_ASSERT(m_kPendingLogins.empty());

	CheckDelEntities();
	FV_ASSERT(m_kDelEntityList.empty());
#endif

	//FvSharedData*	m_pkBaseAppData;
	//FvSharedData*	m_pkGlobalData;
	//FvGlobalBases*	m_pkGlobalBases;
}

class IntEntityMessageHandler : public FvNetInputMessageHandler
{
protected:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvProxy* pkProxy = FvEntityManager::Instance().GetAndCheckProxyForCall(header, srcAddr);
		if(pkProxy)
		{
			pkProxy->PipeIntMessage(header.m_uiIdentifier, data, header.m_iLength);
		}
		else
		{
			//FV_ERROR_MSG("%s, Can't find Proxy, MsgID:%d\n", __FUNCTION__, header.m_uiIdentifier);
			data.Finish();
		}
	}
};
IntEntityMessageHandler g_IntEntityMessageHandler;

class ExtEntityMessageHandler : public FvNetInputMessageHandler
{
protected:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvProxy* pkProxy = FvEntityManager::Instance().GetAndCheckProxyForCall(header, srcAddr);
		if(pkProxy)
		{
			//! 调用Base的方法
			if((header.m_uiIdentifier & 0xC0) == 0xC0)
				pkProxy->HandleExtMessage(header.m_uiIdentifier & ~0xC0, data, header.m_iLength);
			else//! 调用Cell的方法
				pkProxy->PipeExtMessage(header.m_uiIdentifier, data, header.m_iLength);
		}
		else
		{
			//FV_ERROR_MSG("%s, Can't find Proxy, MsgID:%d\n", __FUNCTION__, header.m_uiIdentifier);
			//! 不返回回调异常
			data.Finish();
		}
	}
};
ExtEntityMessageHandler g_ExtEntityMessageHandler;

bool FvEntityManager::Init(int iArgc, char* pcArgv[], FvUInt16 uiExternalPort)
{
	if (m_kExtNub.Socket() == -1)
	{
		FV_ERROR_MSG( "BaseApp::Init: "
			"Unable to bind to external interface on specified port:%d.\n", uiExternalPort);
		return false;
	}

	if (m_kIntNub.Socket() == -1)
	{
		FV_ERROR_MSG( "Failed to create Nub on internal interface.\n" );
		return false;
	}

	FV_INFO_MSG( "External Mapping address = %s\n", m_kExtMappingAddr.c_str() );
	FV_INFO_MSG( "External address = %s\n", m_kExtNub.Address().c_str() );
	FV_INFO_MSG( "Internal address = %s\n", m_kIntNub.Address().c_str() );

	if (!FV_INIT_ANONYMOUS_CHANNEL_CLIENT( m_kDBMgr, m_kIntNub,
		BaseAppIntInterface, DBInterface, 0 ))
	{
		FV_INFO_MSG( "BaseApp::init: Database not ready yet.\n" );
	}
	else
	{
		InitEntityIDClient();
	}

	BaseAppIntInterface::RegisterWithNub( m_kIntNub );
	BaseAppExtInterface::RegisterWithNub( m_kExtNub );

	m_kIntNub.pExtensionData(this);
	for(FvNetMessageID id = 128; id != 255; ++id)
	{
		m_kIntNub.ServeInterfaceElement( BaseAppIntInterface::EntityMessage,
			id, &g_IntEntityMessageHandler );
	}

	m_kExtNub.pExtensionData(this);
	for(FvNetMessageID id = 64; id != 255; ++id)
	{
		m_kExtNub.ServeInterfaceElement( BaseAppExtInterface::EntityMessage,
			id, &g_ExtEntityMessageHandler );
	}

	m_kIntNub.RegisterChildNub(&m_kExtNub);
	m_kIntNub.RegisterChannelFinder(&m_kIdxChannelFinder);

	FvNetReason reason = BaseAppIntInterface::RegisterWithMachined( m_kIntNub, 0 );

	if (reason != FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG("BaseApp::init: Unable to register with nub. "
			"Is machined running?\n");
		return false;
	}

	{
		FvNetNub::InterfaceInfoVec kGlobalApps;
		reason = m_kIntNub.FindInterfaces(0, "GlobalAppInterface", 0, kGlobalApps);

		if (reason == FV_NET_REASON_SUCCESS)
		{
			if(kGlobalApps.size() > 1)
			{
				FV_ERROR_MSG( "BaseApp::init: "
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
			FV_INFO_MSG( "BaseApp::init: GlobalApp not ready yet.\n" );
		}
		else
		{
			FV_ERROR_MSG( "BaseApp::init: "
				"Failed to find GlobalApp interface: %s\n",
				NetReasonToString( (FvNetReason &)reason ) );

			return false;
		}

		m_kIntNub.RegisterBirthListener( BaseAppIntInterface::HandleGlobalAppBirth, "GlobalAppInterface", 0 );
	}

	{
		FvNetNub::InterfaceInfoVec kCellAppMgrs;
		reason = m_kIntNub.FindInterfaces(0xFFFF, "CellAppMgrInterface", 0, kCellAppMgrs);//! 获取所有的CellAppMgr

		if (reason == FV_NET_REASON_SUCCESS)
		{
			for(int i=0; i<(int)kCellAppMgrs.size(); ++i)
			{
				if(kCellAppMgrs[i].uiUserID == m_kIntNub.GetUserID())
					m_kCellAppMgr = kCellAppMgrs[i].kAddr;
				else if(kCellAppMgrs[i].uiUserID == 0)
					m_kGlobalCellAppMgr = kCellAppMgrs[i].kAddr;
			}
		}
		else if (reason == FV_NET_REASON_TIMER_EXPIRED)
		{
			FV_INFO_MSG( "BaseApp::init: CellAppMgr not ready yet.\n" );
		}
		else
		{
			FV_ERROR_MSG( "BaseApp::init: "
				"Failed to find CellAppMgr interface: %s\n",
				NetReasonToString( (FvNetReason &)reason ) );

			return false;
		}

		m_kIntNub.RegisterBirthListener( BaseAppIntInterface::HandleCellAppMgrBirth, "CellAppMgrInterface", 0xFFFF );//! 监听所有的CellAppMgr
	}

	{
		FvNetAddress baseAppMgrAddr;
		reason = m_kIntNub.FindInterface( "BaseAppMgrInterface", 0, baseAppMgrAddr );

		if (reason == FV_NET_REASON_SUCCESS)
		{
			m_kBaseAppMgr.Addr( baseAppMgrAddr );
			m_kBaseAppMgr.Channel().IsIrregular(true);
		}
		else if (reason == FV_NET_REASON_TIMER_EXPIRED)
		{
			FV_INFO_MSG( "BaseApp::init: BaseAppMgr not ready yet.\n" );
		}
		else
		{
			FV_ERROR_MSG( "BaseApp::init: "
				"Failed to find BaseAppMgr interface: %s\n",
				NetReasonToString( (FvNetReason &)reason ) );

			return false;
		}

		m_kIntNub.RegisterBirthListener( BaseAppIntInterface::HandleBaseAppMgrBirth,
			"BaseAppMgrInterface" );
	}

	void* pkEntityExports(NULL);
	if(!FvLogicDllManager::Instance().DllExport(pkEntityExports))
	{
		FV_ERROR_MSG( "BaseApp::Init: "
			"Get Dll Export Failed\n");
		return false;
	}
	m_pkEntityExports = (FvAllBaseEntityExports*)pkEntityExports;
/**
	FvString kEntityDefMD5Digest;
	if(!FvServerConfig::Update( "EntityDefMD5Digest", kEntityDefMD5Digest ))
	{
		FV_ERROR_MSG( "BaseApp::Init: "
			"Get EntityDefMD5Digest from Server XML Failed\n");
		return false;
	}

	if(m_pkEntityExports->kMD5Digest.Quote() != kEntityDefMD5Digest)
	{
		FV_ERROR_MSG( "BaseApp::Init: "
			"EntityDefMD5Digest in Dll isn't Equal to the one from Server XML\n");
		return false;
	}
**/

	if(!FvLogicDllManager::Instance().InitializeLogic())
		return false;

	signal( SIGINT, ::IntSignalHandler );
#ifndef _WIN32
	signal( SIGHUP, ::IntSignalHandler );
#endif //ndef _WIN32

	//FV_INIT_WATCHER_DOC( "baseapp" );

	//FV_REGISTER_WATCHER( 0, "baseapp",
	//	"Base App", "baseApp", m_kIntNub );

	//! BaseAppMgr和CellAppMgr都ok后,向BaseAppMgr注册
	if(m_kBaseAppMgr.Channel().IsEstablished() && !m_kCellAppMgr.IsNone())
		AddToBaseAppMgr();

#ifdef __DEV_MODE__
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
	if(DBMgr().Channel().IsEstablished())
	{
		FV_INFO_MSG("Clear DB\n");

		char sqlCmd[256] = {0};
		sprintf(sqlCmd, "TRUNCATE TABLE futurevisionlogons");
		int len = strlen(sqlCmd);
		FvNetBundle& bundle = DBMgr().Bundle();
		bundle.StartRequest(DBInterface::ExecuteRawCommand, new ClearDBReplyHandler());
		memcpy(bundle.Reserve(len), sqlCmd, len);
		DBMgr().Send();
	}
#endif

	return true;
}

bool FvEntityManager::FinishInit(const FvBaseAppInitData& kInitData)
{
	FV_INFO_MSG("Add to Mgr Reply, id:%d, bReady:%d, time:%d\n", kInitData.id, kInitData.isReady, kInitData.time);

	m_iBaseAppID = kInitData.id;
	m_uiTime = kInitData.time;
	//kInitData.isReady;

	if(kInitData.isReady)
	{
		FV_INFO_MSG("BaseApp is starting, bootStrap:%d\n", 0);

		//! 在服务器组启动后注册
		OnStartup();
	}

	return true;
}

void FvEntityManager::ShutDown()
{
	m_kIntNub.BreakProcessing();
}

FvEntity* FvEntityManager::FindEntity(FvEntityID iID) const
{
	Entities::const_iterator itr = m_kEntities.find(iID);
	if(itr != m_kEntities.end())
		return itr->second.GetObject();
	else
		return NULL;
}

FvProxy* FvEntityManager::FindProxy(const FvNetAddress& addr) const
{
	Proxies::const_iterator itr = m_kProxies.find(addr);
	if(itr != m_kProxies.end())
		return itr->second;
	else
		return NULL;
}

FvNetChannel* FvEntityManager::CellAppMgr()
{
	FV_ASSERT(!m_kCellAppMgr.IsNone());
	return m_kIntNub.FindChannel(m_kCellAppMgr, true);
}

FvNetChannel* FvEntityManager::GlobalCellAppMgr()
{
	FV_ASSERT(!m_kGlobalCellAppMgr.IsNone());
	return m_kIntNub.FindChannel(m_kGlobalCellAppMgr, true);
}

FvMailBox* FvEntityManager::FindGlobalBase(const char* pcLabel) const
{
	if(!pcLabel || !pcLabel[0])
		return NULL;

	FV_ASSERT(m_pkGlobalBases);
	return m_pkGlobalBases->Find(pcLabel);
}

FvMailBox* FvEntityManager::FindGlobalBase(const FvString& kLabel) const
{
	FV_ASSERT(m_pkGlobalBases);
	return m_pkGlobalBases->Find(kLabel);
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

void FvEntityManager::CreateBaseAnywhere(FvEntityTypeID uiEntityType, const FvAllData& kAllData, bool bGlobal, CreateBaseCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL);
		return;
	}

	const FvAllData* pkAllData = &kAllData;
	if(pkAllData->GetType() != uiEntityType)
	{
		FV_ERROR_MSG("%s, EntityType(%d) != AllDataType(%d)\n", __FUNCTION__, uiEntityType, pkAllData->GetType());
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL);
		return;
	}

	//! 都选择remote方式创建
	//FV_ASSERT(!m_kCreateBaseAnywhereAddr.IsNone());
	if(!m_kCreateBaseAnywhereAddr.IsNone() && m_kCreateBaseAnywhereAddr != m_kIntNub.Address())
	{
		FvNetChannel* pkChannel = m_kIntNub.FindChannel(m_kCreateBaseAnywhereAddr, true);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();

		//! 记录回调
		CreateBaseReplyHandler* pReplyHandler = new CreateBaseReplyHandler(pkCallBack);

		kBundle.StartRequest(BaseAppIntInterface::CreateBaseWithCellData, pReplyHandler);
		kBundle << uiEntityType << bGlobal << uiInitFlg;
		pkAllData->SerializeToStream(kBundle);

		pkChannel->DelayedSend();
	}
	else
	{
		FvEntity* pkEntity = CreateBaseLocally(uiEntityType, kAllData, bGlobal, uiInitFlg);
		if(!pkCallBack)
			return;

		if(pkEntity)
		{
			FvMailBox kMB;
			kMB.Set(pkEntity->GetEntityID(), pkEntity->GetEntityTypeID(), m_kCreateBaseAnywhereAddr, FvEntityMailBoxRef::BASE);
			pkCallBack->HandleCallBack(&kMB);
		}
		else
		{
			pkCallBack->HandleCallBack(NULL);
		}
	}
}

void FvEntityManager::CreateBaseAnywhere(const char* pcEntityTypeName, const FvAllData& kAllData, bool bGlobal, CreateBaseCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL);
		return;
	}

	CreateBaseAnywhere(typeID, kAllData, bGlobal, pkCallBack, uiInitFlg);
}

void FvEntityManager::CreateBaseAnywhereFromDB(FvEntityTypeID uiEntityType, const char* pcName, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	//! 都选择remote方式创建
	//FV_ASSERT(!m_kCreateBaseAnywhereAddr.IsNone());
	if(!m_kCreateBaseAnywhereAddr.IsNone() && m_kCreateBaseAnywhereAddr != m_kIntNub.Address())
	{
		FvNetChannel* pkChannel = m_kIntNub.FindChannel(m_kCreateBaseAnywhereAddr, true);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();

		//! 记录回调
		CreateBaseFromDBReplyHandler* pReplyHandler = new CreateBaseFromDBReplyHandler(pkCallBack);

		kBundle.StartRequest(BaseAppIntInterface::CreateBaseFromDB, pReplyHandler);
		kBundle << uiEntityType << true << pcName << uiInitFlg;

		pkChannel->DelayedSend();
	}
	else
	{
		CreateBaseLocallyFromDB(uiEntityType, pcName, pkCallBack, uiInitFlg);
	}
}

void FvEntityManager::CreateBaseAnywhereFromDB(const char* pcEntityTypeName, const char* pcName, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	CreateBaseAnywhereFromDB(typeID, pcName, pkCallBack, uiInitFlg);
}

void FvEntityManager::CreateBaseAnywhereFromDBID(FvEntityTypeID uiEntityType, FvDatabaseID iDBID, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	//! 都选择remote方式创建
	//FV_ASSERT(!m_kCreateBaseAnywhereAddr.IsNone());
	if(!m_kCreateBaseAnywhereAddr.IsNone() && m_kCreateBaseAnywhereAddr != m_kIntNub.Address())
	{
		FvNetChannel* pkChannel = m_kIntNub.FindChannel(m_kCreateBaseAnywhereAddr, true);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();

		//! 记录回调
		CreateBaseFromDBReplyHandler* pReplyHandler = new CreateBaseFromDBReplyHandler(pkCallBack);

		kBundle.StartRequest(BaseAppIntInterface::CreateBaseFromDB, pReplyHandler);
		kBundle << uiEntityType << false << iDBID << uiInitFlg;

		pkChannel->DelayedSend();
	}
	else
	{
		CreateBaseLocallyFromDBID(uiEntityType, iDBID, pkCallBack, uiInitFlg);
	}
}

void FvEntityManager::CreateBaseAnywhereFromDBID(const char* pcEntityTypeName, FvDatabaseID iDBID, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	CreateBaseAnywhereFromDBID(typeID, iDBID, pkCallBack, uiInitFlg);
}

FvEntity* FvEntityManager::CreateBaseLocally(FvEntityTypeID uiEntityType, const FvAllData& kAllData, bool bGlobal, FvUInt8 uiInitFlg)
{
	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		return NULL;
	}

	const FvAllData* pkAllData = &kAllData;
	if(pkAllData->GetType() != uiEntityType)
	{
		FV_ERROR_MSG("%s, EntityType(%d) != AllDataType(%d)\n", __FUNCTION__, uiEntityType, pkAllData->GetType());
		return NULL;
	}

	//! 创建Base
	FvEntityID iEntityID = bGlobal ? GetNextGlobalEntityID() : GetNextEntityID();
	return CreateEntityCommon(uiEntityType, iEntityID, 0, pkAllData, uiInitFlg);
}

FvEntity* FvEntityManager::CreateBaseLocally(const char* pcEntityTypeName, const FvAllData& kAllData, bool bGlobal, FvUInt8 uiInitFlg)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		return NULL;
	}

	return CreateBaseLocally(typeID, kAllData, bGlobal, uiInitFlg);
}

void FvEntityManager::CreateBaseLocallyFromDB(FvEntityTypeID uiEntityType, const char* pcName, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	FvNetBundle& kBundle = DBMgr().Bundle();
	FvEntityID iEntityID = GetNextEntityID();

	//! 记录回调
	CreateBaseLocallyFromDBReplyHandler* pReplyHandler = new CreateBaseLocallyFromDBReplyHandler(uiEntityType, iEntityID, uiInitFlg, pkCallBack);

	kBundle.StartRequest(DBInterface::LoadEntity, pReplyHandler);
	kBundle << uiEntityType << iEntityID << true << pcName;

	DBMgr().Channel().DelayedSend();
}

void FvEntityManager::CreateBaseLocallyFromDB(const char* pcEntityTypeName, const char* pcName, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	CreateBaseLocallyFromDB(typeID, pcName, pkCallBack, uiInitFlg);
}

void FvEntityManager::CreateBaseLocallyFromDBID(FvEntityTypeID uiEntityType, FvDatabaseID iDBID, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	FvNetBundle& kBundle = DBMgr().Bundle();
	FvEntityID iEntityID = GetNextEntityID();

	//! 记录回调
	CreateBaseLocallyFromDBReplyHandler* pReplyHandler = new CreateBaseLocallyFromDBReplyHandler(uiEntityType, iEntityID, uiInitFlg, pkCallBack);

	kBundle.StartRequest(DBInterface::LoadEntity, pReplyHandler);
	kBundle << uiEntityType << iEntityID << false << iDBID;

	DBMgr().Channel().DelayedSend();
}

void FvEntityManager::CreateBaseLocallyFromDBID(const char* pcEntityTypeName, FvDatabaseID iDBID, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	CreateBaseLocallyFromDBID(typeID, iDBID, pkCallBack, uiInitFlg);
}

void FvEntityManager::CreateBaseRemotely(FvEntityTypeID uiEntityType, const FvAllData& kAllData, FvMailBox& kBaseMB, bool bGlobal, CreateBaseCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL);
		return;
	}

	const FvAllData* pkAllData = &kAllData;

	//! 判断MB类型
	if(kBaseMB.GetComponent() != FvEntityMailBoxRef::BASE)
	{
		FV_ERROR_MSG("%s, MailBox Type(%d) is't BaseMB\n", __FUNCTION__, kBaseMB.GetComponent());
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL);
		return;
	}

	//! 判断地址
	if(kBaseMB.GetAddr().IsNone())
	{
		FV_ERROR_MSG("%s, MailBox Addr is NULL\n", __FUNCTION__);
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL);
		return;
	}
	else if(kBaseMB.GetAddr() != m_kIntNub.Address())
	{
		if(pkAllData->GetType() != uiEntityType)
		{
			FV_ERROR_MSG("%s, EntityType(%d) != AllDataType(%d)\n", __FUNCTION__, uiEntityType, pkAllData->GetType());
			if(pkCallBack)
				pkCallBack->HandleCallBack(NULL);
			return;
		}

		FvNetChannel* pkChannel = m_kIntNub.FindChannel(kBaseMB.GetAddr(), true);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();

		//! 记录回调
		CreateBaseReplyHandler* pReplyHandler = new CreateBaseReplyHandler(pkCallBack);

		kBundle.StartRequest(BaseAppIntInterface::CreateBaseWithCellData, pReplyHandler);
		kBundle << uiEntityType << bGlobal << uiInitFlg;
		pkAllData->SerializeToStream(kBundle);

		pkChannel->DelayedSend();
	}
	else
	{
		FvEntity* pkEntity = CreateBaseLocally(uiEntityType, kAllData, bGlobal, uiInitFlg);
		if(!pkCallBack)
			return;

		if(pkEntity)
		{
			FvMailBox kMB;
			kMB.Set(pkEntity->GetEntityID(), pkEntity->GetEntityTypeID(), kBaseMB.GetAddr(), FvEntityMailBoxRef::BASE);
			pkCallBack->HandleCallBack(&kMB);
		}
		else
		{
			pkCallBack->HandleCallBack(NULL);
		}
	}
}

void FvEntityManager::CreateBaseRemotely(const char* pcEntityTypeName, const FvAllData& kAllData, FvMailBox& kBaseMB, bool bGlobal, CreateBaseCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL);
		return;
	}

	CreateBaseRemotely(typeID, kAllData, kBaseMB, bGlobal, pkCallBack, uiInitFlg);
}

void FvEntityManager::CreateBaseRemotelyFromDB(FvEntityTypeID uiEntityType, const char* pcName, FvMailBox& kBaseMB, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	//! 判断MB类型
	if(kBaseMB.GetComponent() != FvEntityMailBoxRef::BASE)
	{
		FV_ERROR_MSG("%s, MailBox Type(%d) is't BaseMB\n", __FUNCTION__, kBaseMB.GetComponent());
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	//! 判断地址
	if(kBaseMB.GetAddr().IsNone())
	{
		FV_ERROR_MSG("%s, MailBox Addr is NULL\n", __FUNCTION__);
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}
	else if(kBaseMB.GetAddr() != m_kIntNub.Address())
	{
		FvNetChannel* pkChannel = m_kIntNub.FindChannel(kBaseMB.GetAddr(), true);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();

		//! 记录回调
		CreateBaseFromDBReplyHandler* pReplyHandler = new CreateBaseFromDBReplyHandler(pkCallBack);

		kBundle.StartRequest(BaseAppIntInterface::CreateBaseFromDB, pReplyHandler);
		kBundle << uiEntityType << true << pcName << uiInitFlg;

		pkChannel->DelayedSend();
	}
	else
	{
		CreateBaseLocallyFromDB(uiEntityType, pcName, pkCallBack, uiInitFlg);
	}
}

void FvEntityManager::CreateBaseRemotelyFromDB(const char* pcEntityTypeName, const char* pcName, FvMailBox& kBaseMB, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	CreateBaseRemotelyFromDB(typeID, pcName, kBaseMB, pkCallBack, uiInitFlg);
}

void FvEntityManager::CreateBaseRemotelyFromDBID(FvEntityTypeID uiEntityType, FvDatabaseID iDBID, FvMailBox& kBaseMB, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	//! 判断MB类型
	if(kBaseMB.GetComponent() != FvEntityMailBoxRef::BASE)
	{
		FV_ERROR_MSG("%s, MailBox Type(%d) is't BaseMB\n", __FUNCTION__, kBaseMB.GetComponent());
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	//! 判断地址
	if(kBaseMB.GetAddr().IsNone())
	{
		FV_ERROR_MSG("%s, MailBox Addr is NULL\n", __FUNCTION__);
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}
	else if(kBaseMB.GetAddr() != m_kIntNub.Address())
	{
		FvNetChannel* pkChannel = m_kIntNub.FindChannel(kBaseMB.GetAddr(), true);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();

		//! 记录回调
		CreateBaseFromDBReplyHandler* pReplyHandler = new CreateBaseFromDBReplyHandler(pkCallBack);

		kBundle.StartRequest(BaseAppIntInterface::CreateBaseFromDB, pReplyHandler);
		kBundle << uiEntityType << false << iDBID << uiInitFlg;

		pkChannel->DelayedSend();
	}
	else
	{
		CreateBaseLocallyFromDBID(uiEntityType, iDBID, pkCallBack, uiInitFlg);
	}
}

void FvEntityManager::CreateBaseRemotelyFromDBID(const char* pcEntityTypeName, FvDatabaseID iDBID, FvMailBox& kBaseMB, CreateBaseFromDBCallBack* pkCallBack, FvUInt8 uiInitFlg)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		if(pkCallBack)
			pkCallBack->HandleCallBack(NULL, 0, false);
		return;
	}

	CreateBaseRemotelyFromDBID(typeID, iDBID, kBaseMB, pkCallBack, uiInitFlg);
}

void FvEntityManager::LookUpBaseByDBID(FvEntityTypeID uiEntityType, FvDatabaseID iDBID, LookUpBaseCallBack* pkCallBack)
{
	FV_ASSERT(pkCallBack);

	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		pkCallBack->HandleCallBack(NULL, false);
		return;
	}

	FvNetBundle& kBundle = DBMgr().Bundle();

	//! 记录回调
	LookUpBaseReplyHandler* pReplyHandler = new LookUpBaseReplyHandler(pkCallBack);

	DBInterface::LookupEntityArgs& kArgs = DBInterface::LookupEntityArgs::StartRequest(kBundle, pReplyHandler);
	kArgs.entityTypeID = uiEntityType;
	kArgs.dbid = iDBID;
	kArgs.offChannel = false;

	DBMgr().Channel().DelayedSend();
}

void FvEntityManager::LookUpBaseByDBID(const char* pcEntityTypeName, FvDatabaseID iDBID, LookUpBaseCallBack* pkCallBack)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		pkCallBack->HandleCallBack(NULL, false);
		return;
	}

	LookUpBaseByDBID(typeID, iDBID, pkCallBack);
}

void FvEntityManager::LookUpBaseByName(FvEntityTypeID uiEntityType, const char* pcName, LookUpBaseCallBack* pkCallBack)
{
	FV_ASSERT(pkCallBack);

	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		pkCallBack->HandleCallBack(NULL, false);
		return;
	}

	FvNetBundle& kBundle = DBMgr().Bundle();

	//! 记录回调
	LookUpBaseReplyHandler* pReplyHandler = new LookUpBaseReplyHandler(pkCallBack);

	kBundle.StartRequest(DBInterface::LookupEntityByName, pReplyHandler);
	kBundle << uiEntityType << pcName << false;

	DBMgr().Channel().DelayedSend();
}

void FvEntityManager::LookUpBaseByName(const char* pcEntityTypeName, const char* pcName, LookUpBaseCallBack* pkCallBack)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		pkCallBack->HandleCallBack(NULL, false);
		return;
	}

	LookUpBaseByName(typeID, pcName, pkCallBack);
}

void FvEntityManager::LookUpDBIDByName(FvEntityTypeID uiEntityType, const char* pcName, LookUpDBIDCallBack* pkCallBack)
{
	FV_ASSERT(pkCallBack);

	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		pkCallBack->HandleCallBack(0);
		return;
	}

	FvNetBundle& kBundle = DBMgr().Bundle();

	//! 记录回调
	LookUpDBIDByNameReplyHandler* pReplyHandler = new LookUpDBIDByNameReplyHandler(pkCallBack);

	kBundle.StartRequest(DBInterface::LookupDBIDByName, pReplyHandler);
	kBundle << uiEntityType << pcName;

	DBMgr().Channel().DelayedSend();
}

void FvEntityManager::LookUpDBIDByName(const char* pcEntityTypeName, const char* pcName, LookUpDBIDCallBack* pkCallBack)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		pkCallBack->HandleCallBack(0);
		return;
	}

	LookUpDBIDByName(typeID, pcName, pkCallBack);
}

void FvEntityManager::DeleteBaseByDBID(FvEntityTypeID uiEntityType, FvDatabaseID iDBID, DeleteBaseCallBack* pkCallBack)
{
	FV_ASSERT(pkCallBack);

	if(m_pkEntityExports->uiEntityCnt <= uiEntityType)
	{
		pkCallBack->HandleCallBack(false, NULL);
		return;
	}

	FvNetBundle& kBundle = DBMgr().Bundle();

	//! 记录回调
	DeleteBaseByDBIDReplyHandler* pReplyHandler = new DeleteBaseByDBIDReplyHandler(pkCallBack);

	DBInterface::DeleteEntityArgs& kArgs = DBInterface::DeleteEntityArgs::StartRequest(kBundle, pReplyHandler);
	kArgs.entityTypeID = uiEntityType;
	kArgs.dbid = iDBID;

	DBMgr().Channel().DelayedSend();
}

void FvEntityManager::DeleteBaseByDBID(const char* pcEntityTypeName, FvDatabaseID iDBID, DeleteBaseCallBack* pkCallBack)
{
	//! 转换类型名
	FvEntityTypeID typeID = EntityNameToIndex(pcEntityTypeName);
	if(typeID == FvEntityTypeID(-1))
	{
		pkCallBack->HandleCallBack(false, NULL);
		return;
	}

	DeleteBaseByDBID(typeID, iDBID, pkCallBack);
}

void FvEntityManager::ExecuteRawDatabaseCommand(const FvString kCommand, ExecuteRawDatabaseCommandCallBack* pkCallBack)
{
	class ExecuteRawDatabaseCommandReplyHandler : public FvNetReplyMessageHandler
	{
	public:
		ExecuteRawDatabaseCommandReplyHandler(ExecuteRawDatabaseCommandCallBack* pkCallBack)
		:m_pkCallBack(pkCallBack)
		{}

	private:
		virtual void HandleMessage( const FvNetAddress & source,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data,
			void * arg )
		{
			FV_ASSERT(m_pkCallBack);
			DBResult kResult;
			FvUInt64 uiAffectedRows(0);
			FvString kErr;
			data >> kErr;

			if(kErr.size() == 0)//! 成功
			{
				FvUInt32 uiCols;
				data >> uiCols;

				if(uiCols)//! 有返回结果
				{
					FvUInt32 uiRows;
					data >> uiRows;

					for(FvUInt32 i=0; i<uiRows; ++i)
					{
						kResult.push_back(DBResultLine());
						DBResultLine& kLine = kResult.back();

						for(FvUInt32 j=0; j<uiCols; ++j)
						{
							int	iLen = data.ReadPackedInt();

							if(iLen)
							{
								kLine.push_back(DBResultCell(static_cast<const char*>(data.Retrieve(iLen)), iLen));
							}
							else
							{
								FvUInt8 bEmpty;
								data >> bEmpty;

								if(bEmpty)
								{
									kLine.push_back(DBResultCell(static_cast<const char*>(data.Retrieve(0)), 0));
								}
								else
								{
									kLine.push_back(DBResultCell());
								}
							}
						}
					}
				}
				else//! 无返回结果
				{
					data >> uiAffectedRows;
				}
			}
			else
			{
				FV_ERROR_MSG("%s\n", kErr.c_str());
			}

			m_pkCallBack->HandleCallBack(kResult, uiAffectedRows, kErr);
			delete this;
		}

		virtual void HandleException( const FvNetNubException & ne,
			void * arg )
		{
			FV_ASSERT(m_pkCallBack);
			DBResult kResult;
			FvUInt64 uiAffectedRows(0);
			FvString kErr("Exception");

			m_pkCallBack->HandleCallBack(kResult, uiAffectedRows, kErr);
			delete this;
		}

		ExecuteRawDatabaseCommandCallBack*	m_pkCallBack;
	};

	FV_ASSERT(pkCallBack);
	int len = (int)kCommand.size();
	FvNetBundle& bundle = DBMgr().Bundle();
	bundle.StartRequest(DBInterface::ExecuteRawCommand, new ExecuteRawDatabaseCommandReplyHandler(pkCallBack));
	memcpy(bundle.Reserve(len), kCommand.data(), len);
	DBMgr().Send();
}

/**
void FvEntityManager::SetCellAppData(const char* pcKey, value)
{

}

void FvEntityManager::DelCellAppData(const char* pcKey)
{

}
**/

const FvNetAddress& FvEntityManager::Address() const
{
	return m_kIntNub.Address();
}

void FvEntityManager::BaseAppLogin( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data )
{
	FvUInt32 session;
	FvInt32 attempt;
	data >> session >> attempt;

	FV_INFO_MSG("%s:Session:%d,attempt:%d,Addr:%s\n", __FUNCTION__, session, attempt, srcAddr.c_str());

	PendingLogins::iterator itr = m_kPendingLogins.find(session);
	if(itr == m_kPendingLogins.end())
		return;

	FvProxy* pkProxy = itr->second.first;
	m_kPendingLogins.erase(itr);

	FV_ASSERT(!pkProxy->HasClient());
	pkProxy->CreateClientChannel(srcAddr);
	bool bRet = m_kProxies.insert(std::make_pair(srcAddr, pkProxy)).second;
	FV_ASSERT(bRet);

	FvNetBundle bundle;
	bundle.StartReply(header.m_iReplyID);
	bundle << session;

	//! 返回client时间和更新频率
	FV_ASSERT(m_iUpdateHertz>0 && m_iUpdateHertz<=0xFF);
	bundle.StartMessage(ClientInterface::UpdateFrequencyNotification);
	bundle << FvUInt8(m_iUpdateHertz);
	bundle.StartMessage(ClientInterface::SetGameTime);
	bundle << m_uiTime;

	m_kExtNub.Send(srcAddr, bundle);
}

void FvEntityManager::Authenticate( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, const BaseAppExtInterface::AuthenticateArgs & args )
{
	//FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::CreateBaseWithCellData( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data )
{
	//! 判断是否是从(DB->)BaseAppMgr发出的,是否是login请求
	//! TODO: 检查BaseAppMgrInterface::CreateBaseEntity,该接口也会调用本接口,但不是login
	bool bLogin(false);
	if(m_kBaseAppMgr.Addr() == srcAddr)
		bLogin = true;

	//! 从DB
	//FvEntityID id;
	//FvEntityTypeID type;
	//FvDatabaseID dbID;
	//FvNetAddress addr;
	//FvString encryptionKey;
	//bool bTrue;
	//(CELL|BASE|ONLY_PERSISTENT)的数据
	//如果EntityDef::HasCellScript()为真,则有
	//	FvVector3(pos)
	//	FvVector3(dir)
	//	FvInt32(spaceID)
	//	FvUInt16(spaceType)

	//! 从其他BaseApp/CellApp
	//FvEntityTypeID type;
	//bool bGlobal
	//FvUInt8 uiInitFlg
	//(CELL|BASE)的数据
	//如果EntityDef::HasCellScript()为真,则有
	//	FvVector3(pos)
	//	FvVector3(dir)
	//	FvInt32(spaceID)
	//	FvUInt16(spaceType)

	FvEntityID id;
	FvEntityTypeID type;
	FvDatabaseID dbID(0);
	bool bGlobal(false);
	FvUInt8 uiInitFlg(FV_ENTITY_DEF_INIT_FLG);

	if(bLogin)
	{
		FvNetAddress addr;
		FvString encryptionKey;
		bool bTrue;
		data >> id >> type >> dbID >> addr >> encryptionKey >> bTrue;
	}
	else
	{
		data >> type >> bGlobal >> uiInitFlg;
	}

	id = bGlobal ? GetNextGlobalEntityID() : GetNextEntityID();

	FV_INFO_MSG("%s:bLogin:%d,bGlobal:%d,addr:%s,id:%d,type:%d,dbID:%I64d,InitFlg:%d\n", __FUNCTION__, bLogin, bGlobal, srcAddr.c_str(), id, type, dbID, uiInitFlg);

	//! 创建Base
	FvEntity* pkEntity = CreateEntityCommon(type, id, dbID, data, bLogin, uiInitFlg);

	//! 返回结果,CellApp不需要返回
	if(header.m_iReplyID != FvNetUnpackedMessageHeader::NO_REPLY)
	{
		FvNetChannel* pkChannel = m_kIntNub.FindChannel(srcAddr);
		FV_ASSERT(pkChannel);
		FvNetBundle& bundle = pkChannel->Bundle();
		bundle.StartReply(header.m_iReplyID);
		FvEntityMailBoxRef ref;
		if(pkEntity)
			ref.Init(id, m_kIntNub.Address(), FvEntityMailBoxRef::BASE, type);
		else
			ref.Init(0, FvNetAddress::NONE, FvEntityMailBoxRef::BASE, 0);
		bundle << ref;
		if(bLogin && pkEntity)
		{
			FvUInt32 uiSession(0);
			uiSession = id;

			//! 返回session
			bundle << uiSession;

			//! 记录proxy
			FV_ASSERT(pkEntity->IsProxy());
			m_kPendingLogins.insert(std::make_pair(uiSession,std::make_pair((FvProxy*)pkEntity, ::Timestamp())));
		}
		pkChannel->DelayedSend();
	}
}

void FvEntityManager::CreateBaseFromDB( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data )
{
	FvEntityTypeID uiTypeID;
	bool bName;
	FvString kName;
	FvDatabaseID iDBID(0);
	FvUInt8 uiInitFlg;
	data >> uiTypeID >> bName;
	if(bName)
		data >> kName;
	else
		data >> iDBID;
	data >> uiInitFlg;
	FV_INFO_MSG("%s, Type:%d, bName:%d, Name:%s, DBID:%I64d, InitFlg:%d\n", __FUNCTION__, uiTypeID, bName, kName.c_str(), iDBID, uiInitFlg);

	FvNetBundle& kBundle = DBMgr().Bundle();
	FvEntityID iEntityID = GetNextEntityID();

	//! 记录回调
	CreateBaseLocallyFromDBWithReplyReplyHandler* pReplyHandler = new CreateBaseLocallyFromDBWithReplyReplyHandler(uiTypeID, iEntityID, uiInitFlg, srcAddr, header.m_iReplyID);

	kBundle.StartRequest(DBInterface::LoadEntity, pReplyHandler);
	kBundle << uiTypeID << iEntityID << bName;
	if(bName)
		kBundle << kName;
	else
		kBundle << iDBID;

	DBMgr().Channel().DelayedSend();
}

void FvEntityManager::LogOnAttempt( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::AddGlobalBase( FvBinaryIStream & data )
{
	FvString kLabel;
	FvMailBox kMB;
	data >> kLabel >> kMB;

	FV_ASSERT(m_pkGlobalBases);
	m_pkGlobalBases->Add(kLabel, kMB);
}

void FvEntityManager::DelGlobalBase( FvBinaryIStream & data )
{
	FvString kLabel;
	data >> kLabel;

	FV_ASSERT(m_pkGlobalBases);
	m_pkGlobalBases->Remove(kLabel);
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

void FvEntityManager::HandleCellAppMgrBirth(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);

	FV_INFO_MSG( "BaseApp::handleCellAppMgrBirth: %s, UserID:%d\n", kAddr.c_str(), kMsg.uiUserID );

	if(kMsg.uiUserID == m_kIntNub.GetUserID())
	{
		if(m_kCellAppMgr.IsNone() && !kAddr.IsNone())
		{
			FV_INFO_MSG( "BaseApp::handleCellAppMgrBirth: "
				"CellAppMgr is now ready.\n" );

			m_kCellAppMgr = kAddr;

			//! BaseAppMgr和CellAppMgr都ok后,向BaseAppMgr注册
			if(m_kBaseAppMgr.Channel().IsEstablished())
				AddToBaseAppMgr();
		}
	}
	else if(kMsg.uiUserID == 0)
	{
		m_kGlobalCellAppMgr = kAddr;
	}
}

void FvEntityManager::HandleBaseAppMgrBirth(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);

	FV_INFO_MSG( "BaseApp::handleBaseAppMgrBirth: %s\n", kAddr.c_str() );

	if(!m_kBaseAppMgr.Channel().IsEstablished() && !kAddr.IsNone())
	{
		FV_INFO_MSG( "BaseApp::handleBaseAppMgrBirth: "
			"BaseAppMgr is now ready.\n" );

		m_kBaseAppMgr.Addr( kAddr );
		m_kBaseAppMgr.Channel().IsIrregular(true);

		//! BaseAppMgr和CellAppMgr都ok后,向BaseAppMgr注册
		if(!m_kCellAppMgr.IsNone())
			AddToBaseAppMgr();
	}
}

void FvEntityManager::HandleGlobalAppBirth(const InterfaceListenerMsg& kMsg)
{
	FV_INFO_MSG("%s\n", __FUNCTION__);

	if(!m_kGlobalEntityIDClient.IsInited())
	{
		FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);
		InitGlobalEntityIDClientAndRegister(kAddr, false);	//! TODO: 异步获取id,需要等一段时间才能使用
	}
}

void FvEntityManager::HandleCellAppDeath( FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
	data.Finish();
}

void FvEntityManager::Startup( const BaseAppIntInterface::StartupArgs & args )
{
	FV_INFO_MSG("BaseApp is starting, bootStrap:%d\n", args.bootstrap);
	m_bIsBootstrap = args.bootstrap;

	OnStartup();
}

void FvEntityManager::ShutDown( const BaseAppIntInterface::ShutDownArgs & args )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::ControlledShutDown( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::SetCreateBaseInfo( FvBinaryIStream & data )
{
	data >> m_kCreateBaseAnywhereAddr;
	//FV_INFO_MSG("%s, %s\n", __FUNCTION__, m_kCreateBaseAnywhereAddr.c_str());
}

void FvEntityManager::SetSharedData( FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::DelSharedData( FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::SetClient( const BaseAppIntInterface::SetClientArgs & args )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
	FV_ASSERT(0);
}

void FvEntityManager::RunScript( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
	data.Finish();
}

void FvEntityManager::OldSetBackupBaseApp( const BaseAppIntInterface::OldSetBackupBaseAppArgs & args )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::OldStartBaseAppBackup( const BaseAppIntInterface::OldStartBaseAppBackupArgs & args )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::OldStopBaseAppBackup( const BaseAppIntInterface::OldStopBaseAppBackupArgs & args )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::OldBackupBaseEntities( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
	data.Finish();
}

void FvEntityManager::OldBackupHeartbeat( const BaseAppIntInterface::OldBackupHeartbeatArgs & args )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::OldRestoreBaseApp( const BaseAppIntInterface::OldRestoreBaseAppArgs & args )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::StartBaseEntitiesBackup( const BaseAppIntInterface::StartBaseEntitiesBackupArgs & args )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::StopBaseEntitiesBackup( const BaseAppIntInterface::StopBaseEntitiesBackupArgs & args )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::BackupBaseEntity( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
	data.Finish();
}

void FvEntityManager::StopBaseEntityBackup( const FvNetAddress & srcAddr, const BaseAppIntInterface::StopBaseEntityBackupArgs & args )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
}

void FvEntityManager::HandleBaseAppDeath( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
	data.Finish();
}

void FvEntityManager::SetBackupBaseApps( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
	data.Finish();
}

void FvEntityManager::EmergencySetCurrentCell( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
	FV_ASSERT(0);
}

void FvEntityManager::CallWatcher( const FvNetAddress& srcAddr, const FvNetUnpackedMessageHeader& header, FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);
	data.Finish();
}

void FvEntityManager::AddToBaseAppMgr()
{
	FV_ASSERT(m_kBaseAppMgr.Channel().IsEstablished());
	FvNetBundle& bundle = m_kBaseAppMgr.Bundle();
	bundle.StartRequest(BaseAppMgrInterface::Add, new RegisterToBaseAppMgrReplyHandler());
	bundle << m_kIntNub.Address();
	bundle << m_kExtMappingAddr;
	m_kBaseAppMgr.Send();
}

FvProxy* FvEntityManager::GetAndCheckProxyForCall(FvNetUnpackedMessageHeader& kHeader, const FvNetAddress& kSrcAddr)
{
	FvProxy* pkProxy(NULL);
	if(kHeader.m_pkNub == &m_kIntNub)
	{
		FV_ASSERT(kHeader.m_pkChannel && kHeader.m_pkChannel->IsIndexed());
		pkProxy = (FvProxy*)FindEntity(kHeader.m_pkChannel->id());
	}
	else
	{
		FV_ASSERT(kHeader.m_pkNub == &m_kExtNub);
		pkProxy = FindProxy(kSrcAddr);
	}

	return pkProxy;
}

FvEntity* FvEntityManager::GetBaseForCall(FvNetUnpackedMessageHeader& kHeader)
{
	FV_ASSERT(kHeader.m_pkNub==&m_kIntNub && kHeader.m_pkChannel && kHeader.m_pkChannel->IsIndexed());
	return FindEntity(kHeader.m_pkChannel->id());
}

void FvEntityManager::AddressDead(const FvNetAddress& kAddress, FvNetReason eReason)
{

}

FvEntityID FvEntityManager::GetNextEntityID()
{
	return m_kEntityIDClient.GetID();
}

FvEntityID FvEntityManager::GetNextGlobalEntityID()
{
	return m_kGlobalEntityIDClient.GetID();
}

FvEntity* FvEntityManager::CreateEntityCommon(FvEntityTypeID uiTypeID, FvEntityID iEntityID, FvDatabaseID iDBID, FvBinaryIStream& stream, bool bFromDB, FvUInt8 uiInitFlg)
{
	if(uiTypeID >= m_pkEntityExports->uiEntityCnt)
		return NULL;

	//! From DB
	//(CELL|BASE|ONLY_PERSISTENT)的数据
	//如果EntityDef::HasCellScript()为真,则有
	//	FvVector3(pos)
	//	FvVector3(dir)
	//	FvInt32(spaceID)
	//	FvUInt16(spaceType)

	//! 不是From DB
	//(CELL|BASE)的数据
	//如果EntityDef::HasCellScript()为真,则有
	//	FvVector3(pos)
	//	FvVector3(dir)
	//	FvInt32(spaceID)
	//	FvUInt16(spaceType)

	//! 创建Base
	FvBaseEntityExport* pkExport = m_pkEntityExports->ppkEntityExport[uiTypeID];
	FvEntity* pkEntity = pkExport->pFunCreateEntity();
	pkEntity->Init(iEntityID, iDBID, pkExport);
	if(bFromDB)
	{
		pkEntity->GetAttrib().DeserializeFromStreamForDBData(stream);
		pkEntity->InitAttribNoDBData();
		if(pkEntity->GetCellData())
			pkEntity->GetCellData()->InitDefaultValForNoDBData();
	}
	else
	{
		pkEntity->GetAttrib().DeserializeFromStreamForAllData(stream);
	}

	pkEntity->Initialize(uiInitFlg);
	pkEntity->AfterInitialize();
	return pkEntity;
}

FvEntity* FvEntityManager::CreateEntityCommon(FvEntityTypeID uiTypeID, FvEntityID iEntityID, FvDatabaseID iDBID, const FvAllData* pkAllData, FvUInt8 uiInitFlg)
{
	if(uiTypeID >= m_pkEntityExports->uiEntityCnt)
		return NULL;

	//! 不是From DB
	//(CELL|BASE)的数据
	//如果EntityDef::HasCellScript()为真,则有
	//	FvVector3(pos)
	//	FvVector3(dir)
	//	FvInt32(spaceID)
	//	FvUInt16(spaceType)

	//! 创建Base
	FvBaseEntityExport* pkExport = m_pkEntityExports->ppkEntityExport[uiTypeID];
	FvEntity* pkEntity = pkExport->pFunCreateEntity();
	pkEntity->Init(iEntityID, iDBID, pkExport);
	pkEntity->InitAttrib(pkAllData, pkEntity->GetAttrib().GetCellData());

	pkEntity->Initialize(uiInitFlg);
	pkEntity->AfterInitialize();
	return pkEntity;
}

void FvEntityManager::ChangeProxy(const FvNetAddress& kAddr, FvProxy* pkProxy)
{
	FV_ASSERT(FindProxy(kAddr) && pkProxy);
	m_kProxies[kAddr] = pkProxy;
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

		FV_INFO_MSG("Entities:%d, Proxies:%d\n", m_kEntities.size(), m_kProxies.size());
	}
	else
	{
		m_kAddDelEntityList.push_back(AddDelEntity(pkEntity,0));
	}
}

void FvEntityManager::EraseProxy(const FvNetAddress& kAddr)
{
	int s = (int)m_kProxies.erase(kAddr);
	FV_INFO_MSG("%s, erase Proxy:%s, Cnt:%d, Remain: Entities:%d, Proxies:%d\n", __FUNCTION__, kAddr.c_str(), s, m_kEntities.size(), m_kProxies.size());
}

int FvEntityManager::HandleTimeout( FvNetTimerID id, void * arg )
{
	switch(reinterpret_cast<FvUIntPtr>(arg))
	{
	case TIMEOUT_GAME_TICK:
		{
			++m_uiTime;

			//if(m_uiTime % m_iProxyTimeoutPeriod == 0)
			{
				CheckProxyTimeOut();
			}

			if(m_uiTime % m_iSyncTimePeriod == 0)
			{
				m_pkTimeKeeper->SynchroniseWithMaster();
			}

			if(m_uiTime % m_iSystemManagePeriod == 0)
			{
				if(0)
				{
					static int s_Stats[10] = {0};
					int* pLoopStats = FvEntityManager::IntNub().LoopStats();
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

				if(m_kBaseAppMgr.Channel().IsEstablished())
				{
					FvNetBundle& bundle = m_kBaseAppMgr.Bundle();
					bundle.StartMessage(BaseAppMgrInterface::InformOfLoad);
					FvInt32 numProxies = m_kProxies.size();
					FvInt32 numBases = m_kEntities.size() - numProxies;
					float load = m_kEntities.size()/100.0f;
					bundle << load << numBases << numProxies;
					m_kBaseAppMgr.Send();
				}

				CheckDelEntities();
			}

			if(m_uiTime % 3 == 0)
			{
				FvUInt64 nowTime = Timestamp();
				float dTime = float(nowTime-m_uiDllTickLastTime) / StampsPerSecond();
				m_uiDllTickLastTime = nowTime;

				FvLogicDllManager::Instance().TickDll(dTime);
			}

			//! 定时发包给Client
			{
				Proxies::iterator itrB = m_kProxies.begin();
				Proxies::iterator itrE = m_kProxies.end();
				while(itrB != itrE)
				{
					itrB->second->SendToClient();
					++itrB;
				}
			}

			{//! TODO: 测试用
				StartUpdate();
				Entities::iterator itrB = m_kEntities.begin();
				Entities::iterator itrE = m_kEntities.end();
				while(itrB != itrE)
				{
					itrB->second->OnTimer(0, NULL);
					++itrB;
				}
				StopUpdate();
				ClearAddDelEntityList();
			}

			//FV_INFO_MSG("ServerTime:%.1f\n", GameTimeInSeconds());
			//FV_INFO_MSG("-----------------------------------\n");
			//FvGlobalBases::Maps& kMaps = m_pkGlobalBases->GetGlobalBaseMap();
			//FvGlobalBases::Maps::iterator itrB = kMaps.begin();
			//FvGlobalBases::Maps::iterator itrE = kMaps.end();
			//while(itrB != itrE)
			//{
			//	FV_INFO_MSG("%s : %s\n", itrB->first.c_str(), itrB->second.c_str());
			//	++itrB;
			//}

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

FvEntityTypeID FvEntityManager::EntityNameToIndex(const char* pcEntityTypeName) const
{
	FvBaseEntityExport* pkExport = m_pkEntityExports->FindByName(pcEntityTypeName);
	if(!pkExport)
	{
		FV_ERROR_MSG("%s, EntityTypeName(%s) is't Exist\n", __FUNCTION__, pcEntityTypeName);
		return FV_INVALID_ENTITY_TYPE_ID;
	}
	return pkExport->uiTypeID;
}

void FvEntityManager::InitEntityIDClient()
{
	if(!DBMgr().Channel().IsEstablished())
		return;

	size_t uiCriticallyLowSize	= FvServerConfig::Get( "baseApp/ids/criticallyLowSize", size_t(100) );
	size_t uiLowSize			= FvServerConfig::Get( "baseApp/ids/lowSize", size_t(2000) );
	size_t uiDesiredSize		= FvServerConfig::Get( "baseApp/ids/desiredSize", size_t(2400) );
	size_t uiHighSize			= FvServerConfig::Get( "baseApp/ids/highSize", size_t(3000) );

	m_kEntityIDClient.Init(&DBMgr().Channel(), DBInterface::GetIDs, DBInterface::PutIDs, uiCriticallyLowSize, uiLowSize, uiDesiredSize, uiHighSize);
}

void FvEntityManager::InitGlobalEntityIDClientAndRegister(FvNetAddress& kAddr, bool bBlock)
{
	FvNetChannel* pkChannel = FindOrCreateChannel(kAddr);

	size_t uiCriticallyLowSize	= FvServerConfig::Get( "baseApp/GlobalIDs/criticallyLowSize", size_t(100) );
	size_t uiLowSize			= FvServerConfig::Get( "baseApp/GlobalIDs/lowSize", size_t(2000) );
	size_t uiDesiredSize		= FvServerConfig::Get( "baseApp/GlobalIDs/desiredSize", size_t(2400) );
	size_t uiHighSize			= FvServerConfig::Get( "baseApp/GlobalIDs/highSize", size_t(3000) );

	m_kGlobalEntityIDClient.Init(pkChannel, GlobalAppInterface::GetEntityIDs, GlobalAppInterface::PutEntityIDs,
		uiCriticallyLowSize, uiLowSize, uiDesiredSize, uiHighSize,
		bBlock);

	//! 注册
	GlobalAppInterface::RegisterArgs& kArgs = GlobalAppInterface::RegisterArgs::start(pkChannel->Bundle());
	kArgs.bBaseApp = true;
	pkChannel->DelayedSend();
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

void FvEntityManager::CheckProxyTimeOut()
{
	FvUInt64 uiNowTime = ::Timestamp();
	FvUInt64 uiTimeOutVal = ::StampsPerSecond() * 30;
	FvUInt64 uiTimeOutVal1 = ::StampsPerSecond() * 90;
	//FvUInt64 uiTimeOutVal = ::StampsPerSecond() * 200;
	std::vector<FvProxy*> kTimeOutList;

	if(true)
	{
		Proxies::iterator itrB = m_kProxies.begin();
		Proxies::iterator itrE = m_kProxies.end();
		while(itrB != itrE)
		{
			if(!itrB->second->IsDestroy() &&
				//itrB->second->EntitiesEnabled() &&	//! TODO: 这里有隐患
				itrB->second->HasClient() &&
				itrB->second->ClientChannel().LastReceivedTime() + uiTimeOutVal < uiNowTime)
			{
				kTimeOutList.push_back(itrB->second);
			}

			++itrB;
		}
	}

	if(true)
	{
		PendingLogins::iterator itrB = m_kPendingLogins.begin();
		PendingLogins::iterator itrE = m_kPendingLogins.end();
		while(itrB != itrE)
		{
			if(itrB->second.second + uiTimeOutVal1 < uiNowTime)
			{
				kTimeOutList.push_back(itrB->second.first);
				itrB = m_kPendingLogins.erase(itrB);
			}
			else
			{
				++itrB;
			}
		}
	}

	if(!kTimeOutList.empty())
	{
		std::vector<FvProxy*>::iterator itrB = kTimeOutList.begin();
		std::vector<FvProxy*>::iterator itrE = kTimeOutList.end();
		while(itrB != itrE)
		{
			FV_INFO_MSG("%s, ProxyTimeOut(%d)\n", __FUNCTION__, (*itrB)->GetEntityID());
			(*itrB)->OnClientDeath();
			++itrB;
		}

		FV_INFO_MSG("%s, Proxy Remain(%d)\n", __FUNCTION__, m_kProxies.size());
	}
}

void FvEntityManager::OnStartup()
{
	FV_ASSERT(!m_pkTimeKeeper);
	FvNetTimerID gtid = m_kIntNub.RegisterTimer(1000000/m_iUpdateHertz, this, reinterpret_cast<void*>(TIMEOUT_GAME_TICK));
	m_pkTimeKeeper = new FvTimeKeeper(m_kIntNub, gtid, m_uiTime, m_iUpdateHertz, &m_kCellAppMgr, &CellAppMgrInterface::GameTimeReading);

	m_uiDllTickLastTime = Timestamp();
	FvLogicDllManager::Instance().OnBaseAppReady(m_bIsBootstrap);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityChannelFinder::EntityChannelFinder(FvNetNub& kNub)
:m_kNub(kNub)
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
		return itr->second;
	else
		return NULL;
}

FvNetChannel* EntityChannelFinder::MakeChannel(FvNetChannelID idx, const FvNetAddress& kCellAddr)
{
	FV_ASSERT(!Find(idx));
	FV_ASSERT(!kCellAddr.IsNone());

	FvNetChannel* pChannel = new FvNetChannel( m_kNub, kCellAddr, FvNetChannel::INTERNAL, 1.0f, NULL, idx);
	pChannel->IsIrregular(true);
	pChannel->ShouldAutoSwitchToSrcAddr(true);
	m_kChannelMap.insert(std::make_pair(idx, pChannel));
	return pChannel;
}

FvNetChannel* EntityChannelFinder::Find(FvNetChannelID id)
{
	IdxChannelMap::iterator itr = m_kChannelMap.find(id);
	if(itr != m_kChannelMap.end())
		return itr->second;
	else
		return NULL;
}

void EntityChannelFinder::DestroyChannel(FvNetChannelID idx)
{
	IdxChannelMap::iterator itr = m_kChannelMap.find(idx);
	if(itr != m_kChannelMap.end())
	{
		itr->second->Condemn();
		m_kChannelMap.erase(itr);
	}
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
	sprintf(timeStr, "BaseApp_%04d%02d%02d_%02d%02d.dump", nowTime.wYear, nowTime.wMonth, nowTime.wDay, nowTime.wHour, nowTime.wMinute);
	spSection->Save(timeStr);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class ARGS_TYPE>
class BaseAppMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntityManager::*Handler)( const ARGS_TYPE & args );

	BaseAppMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & /*srcAddr*/,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;
		(FvEntityManager::Instance().*handler_)( args );
	}

	Handler handler_;
};

template <class ARGS_TYPE>
class BaseAppMessageWithAddrHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntityManager::*Handler)( const FvNetAddress & addr, const ARGS_TYPE & args );

	BaseAppMessageWithAddrHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;
		(FvEntityManager::Instance().*handler_)( srcAddr, args );
	}

	Handler handler_;
};

template <class ARGS_TYPE>
class NoBlockProxyMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvProxy::*Handler)( const ARGS_TYPE & args );

	NoBlockProxyMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;
		FvProxy* pkProxy = FvEntityManager::Instance().GetAndCheckProxyForCall(header, srcAddr);
		if(pkProxy)
		{
			(pkProxy->*handler_)(args);
		}
	}

	Handler handler_;
};


template <class ARGS_TYPE>
class BaseMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( const FvNetAddress & srcAddr, const ARGS_TYPE & args );

	BaseMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data )
	{
		FV_ASSERT(0);//! 这个接口用不到
	}

	Handler handler_;
};


template <class ARGS_TYPE>
class BaseMessageWithAddrHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( const ARGS_TYPE & args, const FvNetAddress & srcAddr );

	BaseMessageWithAddrHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntity* pObj = FvEntityManager::Instance().GetBaseForCall(header);
		FV_ASSERT(pObj);
		ARGS_TYPE args;
		data >> args;
		(pObj->*handler_)( args, srcAddr );
	}

	Handler handler_;
};


class BaseAppVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntityManager::*Handler)( FvBinaryIStream & data );

	BaseAppVarLenMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data )
	{
		(FvEntityManager::Instance().*handler_)( data );
	}

	Handler handler_;
};


class BaseAppRawMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntityManager::*Handler)(  const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data  );

	BaseAppRawMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		(FvEntityManager::Instance().*handler_)( srcAddr, header, data );
	}

	Handler handler_;
};


class BaseVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( FvBinaryIStream & data );

	BaseVarLenMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntity* pkEntity = FvEntityManager::Instance().GetBaseForCall(header);
		FV_ASSERT(pkEntity);
		(pkEntity->*handler_)( data );
	}

	Handler handler_;
};


class BaseAppListenerMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntityManager::*Handler)(const InterfaceListenerMsg& kMsg);

	BaseAppListenerMessageHandler(Handler kHandler):m_kHandler(kHandler) {}

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


class RawBaseMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)(  const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data  );

	RawBaseMessageHandler( Handler handler ) : handler_( handler ) {}

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
			(pkEntity->*handler_)( srcAddr, header, data );
		}
		else
		{
			FV_INFO_MSG("%s, Can't find Entity:%d\n", __FUNCTION__, iEntityID);
			data.Finish();
		}
	}

	Handler handler_;
};

class MethodHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)(  const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data  );

	MethodHandler( Handler handler ) : handler_( handler ) {}

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
			(pkEntity->*handler_)( srcAddr, header, data );
		}
		else
		{
			FV_INFO_MSG("%s, Can't find Entity:%d\n", __FUNCTION__, iEntityID);
			FvInt32 iMethodID;
			data >> iMethodID;
			CallRPCCallbackException(iMethodID, data);
			data.Finish();
		}
	}

	Handler handler_;
};

template <bool BTRUE>
class ProxyVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvProxy::*Handler)( FvBinaryIStream & data );

	ProxyVarLenMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvProxy* pkProxy = FvEntityManager::Instance().GetAndCheckProxyForCall(header, srcAddr);
		if(pkProxy)
		{
			(pkProxy->*handler_)(data);
		}
	}

	Handler handler_;
};

class RawProxyMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvProxy::*Handler)(  const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data  );

	RawProxyMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntityID iEntityID;
		data >> iEntityID;
		FvProxy* pObj = (FvProxy*)FvEntityManager::Instance().FindEntity(iEntityID);
		if(pObj)
		{
			(pObj->*handler_)( srcAddr, header, data );
		}
		else
		{
			FV_INFO_MSG("%s, Can't find Proxy:%d\n", __FUNCTION__, iEntityID);
			data.Finish();
		}
	}

	Handler handler_;
};

template <class ARGS_TYPE>
class BaseAppMessageWithAddrAndHeaderHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntityManager::*Handler)(  const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, const ARGS_TYPE & args  );

	BaseAppMessageWithAddrAndHeaderHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;
		(FvEntityManager::Instance().*handler_)( srcAddr, header, args );
	}

	Handler handler_;
};


template <class ARGS_TYPE>
class ProxyMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvProxy::*Handler)( const ARGS_TYPE & args );

	ProxyMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;
		FvProxy* pkProxy = FvEntityManager::Instance().GetAndCheckProxyForCall(header, srcAddr);
		if(pkProxy)
		{
			(pkProxy->*handler_)(args);
		}
	}

	Handler handler_;
};


template <class ARGS_TYPE>
class BaseEntityCallBackExceptionHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( const ARGS_TYPE & args );

	BaseEntityCallBackExceptionHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvEntityID iEntityID;
		ARGS_TYPE args;
		data >> iEntityID >> args;
		FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(iEntityID);
		if(pkEntity)
		{
			(pkEntity->*handler_)(args);
		}
		else
		{
			FV_ERROR_MSG("Can't find Entity(%d)\n", iEntityID);
			data.Finish();
		}
	}

	Handler handler_;
};


class BaseEntityVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvEntity::*Handler)( FvBinaryIStream & data );

	BaseEntityVarLenMessageHandler( Handler handler ) : handler_( handler ) {}

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
			FV_ERROR_MSG("Can't find Entity(%d)\n", iEntityID);
			data.Finish();
		}
	}

	Handler handler_;
};



#define DEFINE_SERVER_HERE
#include "FvBaseAppIntInterface.h"

#define DEFINE_SERVER_HERE
#include "FvBaseAppExtInterface.h"

#define DEFINE_INTERFACE_HERE
#include <../FvBaseAppManager/FvBaseAppManagerInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvDBManager/FvDBInterface.h>

#define DEFINE_INTERFACE_HERE
#include "FvClientInterface.h"

#define DEFINE_INTERFACE_HERE
#include <../FvCell/FvCellAppInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvCellAppManager/FvCellAppManagerInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvGlobal/FvGlobalAppInterface.h>















