#ifdef _WIN32
#pragma warning (disable : 4355)
#endif

#include "FvCellAppManager.h"
#include "FvCellAppManagerInterface.h"
#include <../FvDBManager/FvDBInterface.h>
#include <../FvCell/FvCellAppInterface.h>
#include <../FvBase/FvBaseAppIntInterface.h>

#include <FvMemoryStream.h>
#include <FvTimestamp.h>
#include <FvNetWatcherGlue.h>
#include <FvNetPortMap.h>
#include <FvServerConfig.h>
#include <FvReviverSubject.h>
//#include <FvStreamHelper.h>
#include <FvTimeKeeper.h>
#include <FvDocWatcher.h>
#include <FvBaseZoneSpace.h>
#include <FvServerResource.h>

#include "FvSpace.h"

#include <signal.h>
#include <limits>



//#define __DEV_MODE__	//! 定义开发模式,可以只重启Base/Cell
//! 如果在服务器组Start之前关闭Cell,则通过超时判定剔除已关闭的Cell
//! 新建Space时,从后往前分配以避免分配到老的Cell
//! 如果在服务器组Start之后关闭Cell,则需在Add新的Cell时清空已有的Space
//! 已关闭的Cell的处理和Cell的分配同上


FV_DECLARE_DEBUG_COMPONENT( 0 )

FV_SINGLETON_STORAGE( FvCellAppMgr )

//! 求1的个数
inline FvUInt32	Ones32(FvUInt32 uiVal)
{
	uiVal -= ((uiVal >> 1) & 0x55555555);
	uiVal = (((uiVal >> 2) & 0x33333333) + (uiVal & 0x33333333));
	uiVal = (((uiVal >> 4) + uiVal) & 0x0F0F0F0F);
	uiVal += (uiVal >> 8);
	uiVal += (uiVal >> 16);
	return (uiVal & 0x0000003F);
}

void IntSignalHandler( int /*sigNum*/ )
{
	FvCellAppMgr * pMgr = FvCellAppMgr::pInstance();

	if (pMgr != NULL)
	{
		pMgr->ShutDown( false );
	}
}


FvCellAppMgr::FvCellAppMgr( FvNetNub & nub )
:m_kNub( nub )
,m_kBaseAppMgr(nub)
,m_bRun(false),m_bSpaceReady(false)
,m_iCellAppID(1)
,m_kBaseAppAddr(FvNetAddress::NONE)
,m_iSpaceID(nub.GetUserID()!=0 ? FV_MIN_SERVER_SPACE : FV_MIN_GLOBAL_SPACE)
,m_iMaxSpaceID(nub.GetUserID()!=0 ? FV_MAX_SERVER_SPACE : FV_MAX_GLOBAL_SPACE)
,m_uiTime(0)
,m_pkTimeKeeper(NULL)
,m_iUpdateHertz(DEFAULT_GAME_UPDATE_HERTZ)
,m_pkSpaceCfgBuf(NULL)
{
	FV_INFO_MSG( "\n---- Cell App Manager ----\n" );
	FV_INFO_MSG( "Address          = %s\n", m_kNub.Address().c_str() );

	FvServerConfig::Update( "gameUpdateHertz", m_iUpdateHertz );

	float fCellAppTimeoutPeriodInSeconds = FvServerConfig::Get( "cellAppMgr/cellAppTimeout", 3.f );
	m_iCellAppTimeoutTime = fCellAppTimeoutPeriodInSeconds;
	m_iCellAppTimeoutPeriod = int( floorf( fCellAppTimeoutPeriodInSeconds * m_iUpdateHertz + 0.5f ) );
	float fSystemManagePeriodInSeconds = FvServerConfig::Get( "cellAppMgr/systemManagePeriod", 15.f );
	m_iSystemManagePeriod = int( floorf( fSystemManagePeriodInSeconds * m_iUpdateHertz + 0.5f ) );

	FV_INFO_MSG( "SystemManage Period = %f\n", fSystemManagePeriodInSeconds);
	FV_INFO_MSG( "CellApp Timeout Period = %f\n", fCellAppTimeoutPeriodInSeconds);
}

FvCellAppMgr::~FvCellAppMgr()
{
	m_kNub.ProcessUntilChannelsEmpty();

	if(m_pkTimeKeeper)
	{
		delete m_pkTimeKeeper;
		m_pkTimeKeeper = NULL;
	}

	{
		CellAppMap::iterator itrB = m_kCellAppMap.begin();
		CellAppMap::iterator itrE = m_kCellAppMap.end();
		while(itrB != itrE)
		{
			delete itrB->second;
			++itrB;
		}
		m_kCellAppMap.clear();
	}

	{
		SpaceMap::iterator itrB = m_kSpaceMap.begin();
		SpaceMap::iterator itrE = m_kSpaceMap.end();
		while(itrB != itrE)
		{
			delete itrB->second;
			++itrB;
		}
		m_kSpaceMap.clear();
	}

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
	FV_SAFE_DELETE_ARRAY(m_pkSpaceCfgBuf);

	{
		DeadCellAppList::iterator itrB = m_kDeadApps.begin();
		DeadCellAppList::iterator itrE = m_kDeadApps.end();
		while(itrB != itrE)
		{
			FvCellApp* pkApp = *itrB;
			delete pkApp;
			++itrB;
		}
		m_kDeadApps.clear();
	}
}

bool FvCellAppMgr::Init(int argc, char * argv[])
{
	if(!LoadSpaceData())
	{
		FV_ERROR_MSG("%s, LoadSpaceData Failed\n", __FUNCTION__);
		return false;
	}

	if (m_kNub.Socket() == -1)
	{
		FV_ERROR_MSG( "Failed to create Nub on interface.\n" );
		return false;
	}

	FvReviverSubject::Instance().Init( &m_kNub, "cellAppMgr" );

	this->Nub().RegisterDeathListener( CellAppMgrInterface::HandleCellAppDeath,
		"CellAppInterface" );

	//! 非全局CellAppMgr需要关心DBMgr
	if(!IsGlobalMode())
	{
		if (!FV_INIT_ANONYMOUS_CHANNEL_CLIENT( m_kDBMgr, m_kNub,
			CellAppMgrInterface, DBInterface, 0 ))
		{
			FV_INFO_MSG( "CellAppMgr::init: Database not ready yet.\n" );
		}
	}

	CellAppMgrInterface::RegisterWithNub( m_kNub );

	FvNetReason reason = CellAppMgrInterface::RegisterWithMachined( m_kNub, 0 );
	if (reason != FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG("CellAppMgr::init: Unable to register with nub. "
				"Is machined running?\n");
		return false;
	}

	//! 非全局CellAppMgr需要关心BaseAppMgr
	if(!IsGlobalMode())
	{
		m_kNub.RegisterBirthListener( CellAppMgrInterface::HandleBaseAppMgrBirth, "BaseAppMgrInterface" );

		FvNetAddress baseAppMgrAddr;
		reason = m_kNub.FindInterface( "BaseAppMgrInterface", 0, baseAppMgrAddr );

		if (reason == FV_NET_REASON_SUCCESS)
		{
			m_kBaseAppMgr.Addr( baseAppMgrAddr );
			m_kBaseAppMgr.Channel().IsIrregular(true);
		}
		else if (reason == FV_NET_REASON_TIMER_EXPIRED)
		{
			FV_INFO_MSG( "CellAppMgr::init: BaseAppMgr not ready yet.\n" );
		}
		else
		{
			FV_ERROR_MSG( "CellAppMgr::init: "
				"Failed to find BaseAppMgr interface: %s\n",
				NetReasonToString( (FvNetReason &)reason ) );

			return false;
		}
	}

	m_kNub.RegisterBirthListener( CellAppMgrInterface::HandleCellAppMgrBirth, "CellAppMgrInterface" );


	signal( SIGINT, ::IntSignalHandler );
#ifndef _WIN32
	signal( SIGHUP, ::IntSignalHandler );
#endif //ndef _WIN32

	FV_INIT_WATCHER_DOC( "cellappmgr" );

	FV_REGISTER_WATCHER( 0, "cellappmgr",
		"Cell App Manager", "cellAppMgr", m_kNub );

	return true;
}

void FvCellAppMgr::ShutDown( bool shutDownOthers )
{
	FV_INFO_MSG( "BaseAppMgr::shutDown: shutDownOthers = %d\n",
		shutDownOthers );
	m_kNub.BreakProcessing();
}

FvCellApp* FvCellAppMgr::FindCellApp(const FvNetAddress& kAddr)
{
	CellAppMap::iterator itr = m_kCellAppMap.find(kAddr);
	if(itr != m_kCellAppMap.end())
		return itr->second;
	else
		return NULL;
}

FvSpace* FvCellAppMgr::FindSpace(FvSpaceID iSpaceID)
{
	SpaceMap::iterator itr = m_kSpaceMap.find(iSpaceID);
	if(itr != m_kSpaceMap.end())
		return itr->second;
	else
		return NULL;
}

FV_INLINE bool FvCellAppMgr::IsGlobalMode()
{
	return m_kNub.GetUserID() == 0 ? true : false;
}

FvSpace* FvCellAppMgr::CreateNewSpace(FvUInt16 uiSpaceType)
{
	FV_ASSERT(uiSpaceType < m_kSpaceInfoList.size());
	SpaceInfo* pkSpaceInfo = m_kSpaceInfoList[uiSpaceType];
	FV_ASSERT(pkSpaceInfo);
	FvSpace* pkSpace = new FvSpace(GetSpaceID(), pkSpaceInfo);
	bool bRet = m_kSpaceMap.insert(std::make_pair(pkSpace->SpaceID(), pkSpace)).second;
	FV_ASSERT(bRet);

	FvMemoryOStream os;
	os << pkSpace->SpaceID();

	FV_INFO_MSG("%s, spaceID:%d,CellCnt:%d\n", __FUNCTION__, pkSpace->SpaceID(), pkSpaceInfo->m_uiCellCnt);

	//! TODO:不同的CellApp选择策略,如:全部CellApp负载均衡策略,或节省CellApp的负载均衡策略

	for(FvUInt16 i=0; i<pkSpaceInfo->m_uiCellCnt; ++i)
	{
#ifndef __DEV_MODE__
		//! 找负载最低的CellApp
		FvCellApp* pkApp(NULL);
		std::vector<FvCellApp*>& kApps = pkSpaceInfo->m_pkCellInfo[i].m_kApps;
		int iAppCnt = kApps.size();
		int iMinLoad = 0xFFFFFFF;

		FV_INFO_MSG("%s, CellIdx:%d, AppCnt:%d\n", __FUNCTION__, i, iAppCnt);

		for(int j=0; j<iAppCnt; ++j)
		{
			if(kApps[j]->GetCellCnt() < iMinLoad)
			{
				iMinLoad = kApps[j]->GetCellCnt();
				pkApp = kApps[j];
			}
		}

		FvCell* pkCell = new FvCell(i, pkSpaceInfo->m_pkCellInfo[i].m_kRect);
		pkSpace->AddCell(pkCell);
		FV_ASSERT(pkApp);
		pkApp->AddCell(pkCell);

		//! FvSpaceID iSpaceID;
		//! FvUInt16 uiSpaceType;
		//! FvUInt16 uiCellIdx;
		FvNetBundle& kBundle = pkApp->Bundle();
		kBundle.StartMessage(CellAppInterface::AddCell);
		kBundle << pkSpace->SpaceID() << uiSpaceType << FvUInt16(i);
		pkApp->Channel().DelayedSend();

		os << pkApp->Channel().addr();

		FV_INFO_MSG("\t CellIdx:%d \t Addr:%s\n", i, pkApp->Channel().addr().c_str());
#else
		//! 找最后一个CellApp
		FvCellApp* pkApp = pkSpaceInfo->m_pkCellInfo[i].m_kApps.back();

		FvCell* pkCell = new FvCell(i, pkSpaceInfo->m_pkCellInfo[i].m_kRect);
		pkSpace->AddCell(pkCell);
		FV_ASSERT(pkApp);
		pkApp->AddCell(pkCell);

		//! FvSpaceID iSpaceID;
		//! FvUInt16 uiSpaceType;
		//! FvUInt16 uiCellIdx;
		FvNetBundle& kBundle = pkApp->Bundle();
		kBundle.StartMessage(CellAppInterface::AddCell);
		kBundle << pkSpace->SpaceID() << uiSpaceType << FvUInt16(i);
		pkApp->Channel().DelayedSend();

		os << pkApp->Channel().addr();

		FV_INFO_MSG("\t CellIdx:%d \t Addr:%s\n", i, pkApp->Channel().addr().c_str());
#endif
	}

	if(pkSpaceInfo->m_uiCellCnt > 1)
	{
		FvSpace::Cells& kCells = pkSpace->GetCells();
		FvSpace::Cells::iterator itrB = kCells.begin();
		FvSpace::Cells::iterator itrE = kCells.end();
		while(itrB != itrE)
		{
			FvNetBundle& kBundle = (*itrB)->GetCellApp()->Bundle();
			kBundle.StartMessage(CellAppInterface::UpdateGeometry);
			kBundle.AddBlob(os.Data(), os.RemainingLength());
			(*itrB)->GetCellApp()->Channel().DelayedSend();
			++itrB;
		}
	}

	return pkSpace;
}

void FvCellAppMgr::OnStartup()
{
	FV_ASSERT(!m_bRun);

	//! 转发给所有CellApp
	CellAppMap::iterator itrB = m_kCellAppMap.begin();
	CellAppMap::iterator itrE = m_kCellAppMap.end();
	while(itrB != itrE)
	{
		FvNetBundle& kBundle = itrB->second->Bundle();
		kBundle.StartMessage(CellAppInterface::Startup);
		kBundle << m_kBaseAppAddr;
		itrB->second->Channel().DelayedSend();
		++itrB;
	}

	FvNetTimerID gtid = m_kNub.RegisterTimer(1000000/m_iUpdateHertz, this, reinterpret_cast<void*>(TIMEOUT_GAME_TICK));
	m_pkTimeKeeper = new FvTimeKeeper(m_kNub, gtid, m_uiTime, m_iUpdateHertz);
	m_bRun = true;
}

void FvCellAppMgr::SendToCellApps(const FvNetInterfaceElement& ifElt, FvMemoryOStream& args)
{
	CellAppMap::iterator itrB = m_kCellAppMap.begin();
	CellAppMap::iterator itrE = m_kCellAppMap.end();
	while(itrB != itrE)
	{
		FvNetBundle& kBundle = itrB->second->Bundle();
		kBundle.StartMessage(ifElt);
		kBundle.AddBlob( args.Data(), args.Size() );
		itrB->second->Channel().DelayedSend();
		++itrB;
	}
}

bool FvCellAppMgr::IsValidSpaceID(FvSpaceID iSpaceID)
{
	if(IsGlobalMode() && FV_MIN_GLOBAL_SPACE < iSpaceID && iSpaceID < FV_MAX_GLOBAL_SPACE ||
		!IsGlobalMode() && FV_MIN_SERVER_SPACE < iSpaceID && iSpaceID < FV_MAX_SERVER_SPACE)
		return true;
	else
		return false;
}

int FvCellAppMgr::HandleTimeout( FvNetTimerID id, void * arg )
{
	switch(reinterpret_cast<FvUIntPtr>(arg))
	{
	case TIMEOUT_GAME_TICK:
		{
			++m_uiTime;

			if(m_uiTime % m_iCellAppTimeoutPeriod == 0)
			{
				CheckOverLoad();
				//CheckSpaceEmpty();	//! 好像不需要主动检测
			}

			if(m_uiTime % m_iSystemManagePeriod == 0)
			{
				CheckDeadCellApp();
			}

			//! TODO: 在关服流程没有完善的情况下,先提高写数据库时间的方式减少重开服务器的时间误差
			if(m_uiTime % 5 == 0)
			{
				//! 非全局CellAppMgr
				if(!IsGlobalMode())
				{
					//! 把服务器时间写入数据库
					FvNetBundle& kBundle = DBMgr().Bundle();
					kBundle.StartMessage(DBInterface::WriteGameTime);
					kBundle << m_uiTime;
					DBMgr().Channel().DelayedSend();
				}
			}

			//FV_INFO_MSG("ServerTime:%.1f\n", GameTimeInSeconds());
		}
		break;
	}

	return 0;
}

void FvCellAppMgr::CreateEntity( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	//! FvSpaceID iSpaceID;
	//! FvVector3 kPos;
	//! FvVector3 kDir;
	//! FvNetAddress kBaseAddr;
	//! FvEntityTypeID uiEntityTypeID;
	//! FvEntityID iEntityID;
	//! FvDatabaseID iDBID;
	//! FvUInt8 uiInitFlg;
	//! Cell部分数据

	FvSpaceID iSpaceID;
	FvVector3 kPos;
	data >> iSpaceID >> kPos;

	FV_INFO_MSG("%s, spaceID:%d, kPos(%f,%f,%f)\n", __FUNCTION__, iSpaceID, kPos.x, kPos.y, kPos.z);

	FvSpace* pkSpace = FindSpace(iSpaceID);
	if(!pkSpace)
	{
		FV_ERROR_MSG("%s, Can't Find Space:%d\n", __FUNCTION__, iSpaceID);
		goto CreateEntity_ERR;
	}

	FvCell* pkCell = pkSpace->FindCell(kPos);
	FV_ASSERT(pkCell);

	FvNetChannel& kChannel = pkCell->GetCellApp()->Channel();
	FvNetBundle& kBundle = kChannel.Bundle();
	kBundle.StartMessage(CellAppInterface::CreateEntity);
	kBundle << pkSpace->SpaceID() << kPos;
	kBundle.Transfer(data, data.RemainingLength());
	kChannel.DelayedSend();

	return;

CreateEntity_ERR:
	FvVector3 kDir;
	FvNetAddress kBaseAddr;
	FvEntityTypeID uiEntityTypeID;
	FvEntityID iEntityID;
	data >> kDir >> kBaseAddr >> uiEntityTypeID >> iEntityID;
	FvNetChannel* pkChannel = &m_kNub.FindOrCreateChannel(kBaseAddr);
	pkChannel->Bundle().StartMessage(BaseAppIntInterface::CurrentCell);
	pkChannel->Bundle() << iEntityID << FvSpaceID(0);//! 返回0表示失败
	pkChannel->DelayedSend();
	data.Finish();
}

void FvCellAppMgr::CreateEntityInNewSpace( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	//! FvUInt16 uiSpaceType;
	//! FvVector3 kPos;
	//! FvVector3 kDir;
	//! FvNetAddress kBaseAddr;
	//! FvEntityTypeID uiEntityTypeID;
	//! FvEntityID iEntityID;
	//! FvDatabaseID iDBID;
	//! FvUInt8 uiInitFlg;
	//! Cell部分数据

	FvUInt16 uiSpaceType;
	FvVector3 kPos;
	data >> uiSpaceType >> kPos;

	FV_INFO_MSG("%s, spaceType:%d, kPos(%f,%f,%f)\n", __FUNCTION__, uiSpaceType, kPos.x, kPos.y, kPos.z);

	if(uiSpaceType >= m_kSpaceInfoList.size() ||
		!m_kSpaceInfoList[uiSpaceType])
	{
		FV_ERROR_MSG("%s, SpaceType:%d too big or SpaceInfo is NULL\n", __FUNCTION__, uiSpaceType);
		goto CreateEntityInNewSpace_ERR;
	}


	FvSpace* pkSpace(NULL);

	if(m_kSpaceInfoList[uiSpaceType]->m_kSpaceRect.IsPointIn(kPos))
	{
		pkSpace = CreateNewSpace(uiSpaceType);
	}
	else
	{
		FV_ERROR_MSG("%s, Pos(%f,%f,%f) is not In Space(%d)\n", __FUNCTION__,
			kPos.x ,kPos.y, kPos.z,
			uiSpaceType);
		goto CreateEntityInNewSpace_ERR;
	}

	FvCell* pkCell = pkSpace->FindCell(kPos);
	FV_ASSERT(pkCell);

	FvNetChannel& kChannel = pkCell->GetCellApp()->Channel();
	FvNetBundle& kBundle = kChannel.Bundle();
	kBundle.StartMessage(CellAppInterface::CreateEntity);
	kBundle << pkSpace->SpaceID() << kPos;
	kBundle.Transfer(data, data.RemainingLength());
	kChannel.DelayedSend();

	return;

CreateEntityInNewSpace_ERR:
	FvVector3 kDir;
	FvNetAddress kBaseAddr;
	FvEntityTypeID uiEntityTypeID;
	FvEntityID iEntityID;
	data >> kDir >> kBaseAddr >> uiEntityTypeID >> iEntityID;
	FvNetChannel* pkChannel = &m_kNub.FindOrCreateChannel(kBaseAddr);
	pkChannel->Bundle().StartMessage(BaseAppIntInterface::CurrentCell);
	pkChannel->Bundle() << iEntityID << FvSpaceID(0);//! 返回0表示失败
	pkChannel->DelayedSend();
	data.Finish();
}

void FvCellAppMgr::PrepareForRestoreFromDB( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FV_INFO_MSG("%s\n", __FUNCTION__);

	FV_ASSERT(!IsGlobalMode());

	data >> m_uiTime;
	data.Finish();

	//! TODO:DB里需要增加SpaceType字段
/**
	int numSpaces;
	data >> time >> numSpaces;

	//! 恢复space
	for(int i=0; i<numSpaces; ++i)
	{
		FvSpaceID spaceID;
		int numData;
		data >> spaceID >> numData;
		for(int j=0; j<numData; ++j)
		{
			FvUInt64 spaceEntryID;
			FvUInt16 spaceDataKey;
			FvString spaceData;
			data >> spaceEntryID >> spaceDataKey >> spaceData;
		}
	}
**/

	//! 转发给所有CellApp
	CellAppMap::iterator itrB = m_kCellAppMap.begin();
	CellAppMap::iterator itrE = m_kCellAppMap.end();
	while(itrB != itrE)
	{
		FvNetBundle& kBundle = itrB->second->Bundle();
		kBundle.StartMessage(CellAppInterface::SetGameTime);
		kBundle << m_uiTime;
		itrB->second->Channel().DelayedSend();
		++itrB;
	}
}

void FvCellAppMgr::Startup( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FV_INFO_MSG( "CellAppMgr is starting\n" );

	FV_ASSERT(!IsGlobalMode());

	if(DBMgr().Channel().IsEstablished())
	{
		FvNetBundle& bundle = DBMgr().Channel().Bundle();
		bundle.StartMessage(DBInterface::CellAppOverloadStatus);
		bundle << false;
		DBMgr().Channel().DelayedSend();
	}

	OnStartup();
}

void FvCellAppMgr::ShutDown( const CellAppMgrInterface::ShutDownArgs & args )
{
	FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, sizeof(args));

	FV_ASSERT(!IsGlobalMode());
}

void FvCellAppMgr::ControlledShutDown( const CellAppMgrInterface::ControlledShutDownArgs & args )
{
	FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, sizeof(args));

	FV_ASSERT(!IsGlobalMode());
}

void FvCellAppMgr::ShouldOffload( const CellAppMgrInterface::ShouldOffloadArgs & args )
{
	FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, sizeof(args));

	FV_ASSERT(!IsGlobalMode());
}

void FvCellAppMgr::RunScript( FvBinaryIStream & data )
{
	FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, data.RemainingLength());
	data.Finish();
}

void FvCellAppMgr::AddApp( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
#ifdef __DEV_MODE__
	//! 如果Svr已经Start,需要清空已有Space
	if(m_bRun)
	{
		SpaceMap::iterator itrB = m_kSpaceMap.begin();
		SpaceMap::iterator itrE = m_kSpaceMap.end();
		while(itrB != itrE)
		{
			PutSpaceID(itrB->second->SpaceID());
			itrB->second->Destroy();
			delete itrB->second;
			++itrB;
		}
		m_kSpaceMap.clear();
	}
#endif

	FvUInt8 uiLoadSpaceGeometryIdx;
	data >> uiLoadSpaceGeometryIdx;

	FV_ASSERT(!FindCellApp(addr));
	FV_ASSERT(uiLoadSpaceGeometryIdx < m_kLoadSpaceGeometryList.size());

	FvCellApp* pkCellApp = new FvCellApp(m_kNub, addr, m_iCellAppID, uiLoadSpaceGeometryIdx);
	m_kCellAppMap.insert(std::make_pair(addr, pkCellApp));
	++m_iCellAppID;
	FV_ASSERT(m_iCellAppID > 0);

	LoadSpaceGeometrys& kLoadSpaceGeometrys = m_kLoadSpaceGeometryList[uiLoadSpaceGeometryIdx];
	for(int i=0; i<(int)kLoadSpaceGeometrys.size(); ++i)
	{
		FV_ASSERT(m_kSpaceInfoList[kLoadSpaceGeometrys[i].m_uiSpaceType]);
		m_kSpaceInfoList[kLoadSpaceGeometrys[i].m_uiSpaceType]->m_pkCellInfo[kLoadSpaceGeometrys[i].m_uiCellIdx].AddCellApp(pkCellApp);
	}
	CheckSpaceReady();

	FvCellAppInitData initData;
	initData.id = pkCellApp->CellAppID();
	initData.baseAppAddr = m_kBaseAppAddr;
	initData.isReady = m_bRun;
	initData.time = m_uiTime;

	FvNetChannel* pkChannel = m_kNub.FindChannel(addr);
	FV_ASSERT(pkChannel);
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartReply(header.m_iReplyID);
	kBundle << initData;
	pkChannel->DelayedSend();

	FV_DEBUG_MSG("CellAppMgr::add:\n"
		"\tAllocated id    = %u\n"
		"\tCellApps in use = %lu\n"
		"\tNub             = %s\n"
		"\tLoadIdx         = %d\n"
		"\tSpace is Ready  = %d\n",
		initData.id,
		m_kCellAppMap.size(),
		addr.c_str(),
		uiLoadSpaceGeometryIdx,
		m_bSpaceReady);


	if(IsGlobalMode() && m_bSpaceReady && !m_bRun)
	{
		FV_INFO_MSG( "CellAppMgr is starting\n" );
		OnStartup();
	}
}

void FvCellAppMgr::RecoverCellApp( FvBinaryIStream & data )
{
	FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, data.RemainingLength());
	data.Finish();

	FV_ASSERT(!IsGlobalMode());
}

void FvCellAppMgr::DelApp( const CellAppMgrInterface::DelAppArgs & args )
{
	FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, sizeof(args));

	FvCellApp* pkApp = FindCellApp(args.addr);
	FV_ASSERT(pkApp && pkApp->GetCellCnt()==0);

	LoadSpaceGeometrys& kLoadSpaceGeometrys = m_kLoadSpaceGeometryList[pkApp->LoadSpaceGeometryIdx()];
	for(int i=0; i<(int)kLoadSpaceGeometrys.size(); ++i)
	{
		m_kSpaceInfoList[kLoadSpaceGeometrys[i].m_uiSpaceType]->m_pkCellInfo[kLoadSpaceGeometrys[i].m_uiCellIdx].RemoveCellApp(pkApp);
	}
	m_bSpaceReady = false;
	CheckSpaceReady();

	m_kCellAppMap.erase(args.addr);
	delete pkApp;
}

void FvCellAppMgr::SetBaseApp( const CellAppMgrInterface::SetBaseAppArgs & args )
{
	m_kBaseAppAddr = args.addr;
	FV_INFO_MSG("%s: %s\n", __FUNCTION__, m_kBaseAppAddr.c_str());

	FV_ASSERT(!IsGlobalMode());

	if(!m_bRun)
		return;

	//! 转发给所有CellApp
	CellAppMap::iterator itrB = m_kCellAppMap.begin();
	CellAppMap::iterator itrE = m_kCellAppMap.end();
	while(itrB != itrE)
	{
		FvNetBundle& kBundle = itrB->second->Bundle();
		kBundle.StartMessage(CellAppInterface::SetBaseApp);
		kBundle << m_kBaseAppAddr;
		itrB->second->Channel().DelayedSend();
		++itrB;
	}
}

void FvCellAppMgr::HandleCellAppMgrBirth(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);
	FV_INFO_MSG("RECVPK>>%s\n", __FUNCTION__);
}

void FvCellAppMgr::HandleBaseAppMgrBirth(const InterfaceListenerMsg& kMsg)
{
	FV_ASSERT(!IsGlobalMode());
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);
	FV_INFO_MSG( "%s: %s\n", __FUNCTION__, kAddr.c_str());
	m_kBaseAppMgr.Addr( kAddr );
	m_kBaseAppMgr.Channel().IsIrregular(true);
}

void FvCellAppMgr::HandleCellAppDeath(const InterfaceListenerMsg& kMsg)
{
	FvNetAddress kAddr(kMsg.uiIP, kMsg.uiPort);
	FV_INFO_MSG("RECVPK>>%s\n", __FUNCTION__);
}

void FvCellAppMgr::HandleBaseAppDeath( FvBinaryIStream & data )
{
	FV_ASSERT(!IsGlobalMode());
	FvNetAddress kAddr;
	data >> kAddr;
	data.Finish();
	FV_INFO_MSG("%s, Addr:%s\n", __FUNCTION__, kAddr.c_str());

	FvNetChannel* pkChannel(NULL);
	if(pkChannel = m_kNub.FindChannel(kAddr))
	{
		pkChannel->Destroy();
	}
}

void FvCellAppMgr::AckCellAppDeath( const FvNetAddress & addr, const CellAppMgrInterface::AckCellAppDeathArgs & args )
{
	FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, sizeof(args));
}

void FvCellAppMgr::GameTimeReading( const FvNetAddress & srcAddr, FvNetReplyID replyID, const CellAppMgrInterface::GameTimeReadingArgs & args )
{
	//FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, sizeof(args));
	double time = m_pkTimeKeeper ? m_pkTimeKeeper->ReadingAtNextTick() : args.gameTimeReadingContribution;

	FvNetBundle bundle;
	bundle.StartReply(replyID);
	bundle << time;
	m_kNub.Send(srcAddr, bundle);
}

void FvCellAppMgr::UpdateSpaceData( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FvSpaceID iSpaceID;
	FvUInt16 uiKey;
	data >> iSpaceID >> uiKey;
	if(data.Error())
	{
		FV_ERROR_MSG("%s, stream Err\n", __FUNCTION__);
		data.Finish();
		return;
	}

	FvSpace* pkSpace = FindSpace(iSpaceID);
	if(!pkSpace)
	{
		FV_ERROR_MSG("%s, iSpaceID(%d) invalid\n", __FUNCTION__, iSpaceID);
		data.Finish();
		return;
	}

	FV_INFO_MSG("%s, SpaceID:%d, Key:%d\n", __FUNCTION__, iSpaceID, uiKey);

	FvMemoryOStream kOS;
	kOS << iSpaceID << ((const FvSpaceEntryID&)addr) << uiKey;
	kOS.Transfer(data, data.RemainingLength());

	FvSpace::Cells& kCells = pkSpace->GetCells();
	FvUInt32 uiSize(kCells.size());
	for(FvUInt32 i=0; i<uiSize; ++i)
	{
		FvNetBundle& kBundle = kCells[i]->GetCellApp()->Bundle();
		kBundle.StartMessage(CellAppInterface::SpaceData);
		kBundle.AddBlob(kOS.Data(), kOS.Size());
		kCells[i]->GetCellApp()->Channel().DelayedSend();
	}
}

void FvCellAppMgr::ShutDownSpace( const CellAppMgrInterface::ShutDownSpaceArgs & args )
{
	FV_INFO_MSG("%s, SpaceID:%d\n", __FUNCTION__, args.spaceID);

	SpaceMap::iterator itr = m_kSpaceMap.find(args.spaceID);
	if(itr == m_kSpaceMap.end())
	{
		FV_ERROR_MSG("%s, Can't find Space of id:%d\n", __FUNCTION__, args.spaceID);
		return;
	}

	itr->second->Destroy();
	delete itr->second;
	m_kSpaceMap.erase(itr);
}

void FvCellAppMgr::AckBaseAppsShutDown( const CellAppMgrInterface::AckBaseAppsShutDownArgs & args )
{
	FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, sizeof(args));
	FV_ASSERT(!IsGlobalMode());
}

void FvCellAppMgr::CheckStatus( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	//FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, data.RemainingLength());

	FV_ASSERT(!IsGlobalMode());

	FvNetChannel* pkChannel = m_kNub.FindChannel(addr);
	FV_ASSERT(pkChannel);
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartReply(header.m_iReplyID);
	FvUInt32 cnt = m_kCellAppMap.size();
	kBundle << m_bSpaceReady << cnt;
	pkChannel->DelayedSend();
}

void FvCellAppMgr::SetSharedData( FvBinaryIStream & data )
{
	FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, data.RemainingLength());
	data.Finish();
}

void FvCellAppMgr::DelSharedData( FvBinaryIStream & data )
{
	FV_INFO_MSG("RECVPK>>%s,len:%d\n", __FUNCTION__, data.RemainingLength());
	data.Finish();
}

void FvCellAppMgr::AddGlobalBase( FvBinaryIStream & data )
{
	FvString kLabel;
	FvEntityMailBoxRef kMB;
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

	FvMemoryOStream kOS;
	kOS << kLabel << kMB;
	SendToCellApps(CellAppInterface::AddGlobalBase, kOS);
}

void FvCellAppMgr::DelGlobalBase( FvBinaryIStream & data )
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

	FvMemoryOStream kOS;
	kOS << kLabel;
	SendToCellApps(CellAppInterface::DelGlobalBase, kOS);
}

void CellInfo::AddCellApp(FvCellApp* pkApp)
{
	m_kApps.push_back(pkApp);
}

void CellInfo::RemoveCellApp(FvCellApp* pkApp)
{
	std::vector<FvCellApp*>::iterator itrB = m_kApps.begin();
	std::vector<FvCellApp*>::iterator itrE = m_kApps.end();
	while(itrB != itrE)
	{
		if(*itrB == pkApp)
		{
			m_kApps.erase(itrB);
			break;
		}
		++itrB;
	}
}

SpaceInfo::~SpaceInfo()
{
	if(m_pkCellInfo)
	{
		delete [] m_pkCellInfo;
		m_pkCellInfo = NULL;
	}
}

FvUInt16 SpaceKDTree::FindCellIdx(float fX, float fY) const
{
	FV_ASSERT(m_pkNodes);
	SpaceKDTree::Node* pkNode = m_pkNodes;

	for(;;)
	{
		float fValue(fY);
		if(pkNode->m_bX)
			fValue = fX;

		if(fValue < pkNode->m_fKey)
		{
			if(pkNode->m_bLeftLeaf)
				return pkNode->GetLeftValue();
			pkNode = m_pkNodes + pkNode->GetLeftValue();
		}
		else
		{
			if(pkNode->m_bRightLeaf)
				return pkNode->GetRightValue();
			pkNode = m_pkNodes + pkNode->GetRightValue();
		}
	}
}

bool SpaceInfo::CheckSpaceReady()
{
	for(FvUInt16 i=0; i<m_uiCellCnt; ++i)
	{
		if(m_pkCellInfo[i].m_kApps.empty())
			return false;
	}

	return true;
}

bool FvCellAppMgr::LoadSpaceData()
{
	if(!LoadSpaceInfo())
		return false;

	if(!LoadSpaceScheme())
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

	void ReadAoI()
	{
		FvUInt32 uiFlg;
		Read(uiFlg);

		FvUInt32 uiBiCnts = Ones32(uiFlg);
		AddLen(uiBiCnts * sizeof(float));
	}

protected:
	char*	m_pkBuf;
	char*	m_pkReadPt;
	char*	m_pkEnd;
	int		m_iBufSize;
};

bool FvCellAppMgr::LoadSpaceInfo()
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
			pkSpaceInfo->m_kKDTree.m_pkNodes = (SpaceKDTree::Node*)kMR.GetReadPt();
			kMR.AddLen(uiTmp);

			kMR.ReadAoI();

			FvUInt32 uiCellCnt;
			kMR.Read(uiCellCnt);
			FV_ASSERT(uiCellCnt);
			pkSpaceInfo->m_uiCellCnt = FvUInt16(uiCellCnt);
			pkSpaceInfo->m_pkCellInfo = new CellInfo[uiCellCnt];
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
				pkSpaceInfo->m_pkCellInfo[i].m_kRect = kRect;

				kMR.Read(uiTmp16);
				pkSpaceInfo->m_pkCellInfo[i].m_bLoadEntity = uiTmp16>0 ? true : false;
				kMR.Read(uiTmp16);
				pkSpaceInfo->m_pkCellInfo[i].m_bLoadZone = uiTmp16>0 ? true : false;

				kMR.SetReadPt(pkCellPt + uiCellSize);
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

bool FvCellAppMgr::LoadSpaceScheme()
{
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

	int iSchemeIdx(0);
	std::vector<FvXMLSectionPtr> kCells;
	std::vector<FvXMLSectionPtr>::iterator itrB = kSchemes.begin();
	std::vector<FvXMLSectionPtr>::iterator itrE = kSchemes.end();
	while(itrB != itrE)
	{
		m_kLoadSpaceGeometryList.push_back(LoadSpaceGeometrys());

		(*itrB)->OpenSections("Cell", kCells);
		if(!kCells.empty())
		{
			FvUInt16 uiCellIdx(0);
			std::vector<FvXMLSectionPtr>::iterator itrB1 = kCells.begin();
			std::vector<FvXMLSectionPtr>::iterator itrE1 = kCells.end();
			while(itrB1 != itrE1)
			{
				FvString kS = (*itrB1)->AsString("");
				int i(0),j(0);
				if(sscanf(kS.c_str(), "%d%d", &i, &j) != 2 ||
					i<0 || j<0)
				{
					FV_ERROR_MSG("%s, Scheme:%d Cell:%d data Err\n", __FUNCTION__, iSchemeIdx, uiCellIdx);
					return false;
				}

				LoadSpaceGeometry kLoadSpaceGeometry;
				kLoadSpaceGeometry.m_uiSpaceType = (FvUInt16)i;
				kLoadSpaceGeometry.m_uiCellIdx = (FvUInt16)j;
				m_kLoadSpaceGeometryList[iSchemeIdx].push_back(kLoadSpaceGeometry);

				if(m_kSpaceInfoList[kLoadSpaceGeometry.m_uiSpaceType])
				{
					if(m_kSpaceInfoList[kLoadSpaceGeometry.m_uiSpaceType]->m_uiCellCnt <= kLoadSpaceGeometry.m_uiCellIdx)
					{
						FV_ERROR_MSG("%s, Scheme:%d Cell:%d CellIdx too Big\n", __FUNCTION__, iSchemeIdx, uiCellIdx);
						return false;
					}
				}

				++uiCellIdx;
				++itrB1;
			}

			kCells.clear();
		}
		else
		{
			FV_ERROR_MSG("%s, Scheme:%d has No Cell\n", __FUNCTION__, iSchemeIdx);
			return false;
		}

		++iSchemeIdx;
		++itrB;
	}

	return true;
}

bool FvCellAppMgr::CheckRect(FvCellRect& kRect, int* piGridPos)
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

FvSpaceID FvCellAppMgr::GetSpaceID()
{
	FvSpaceID iID(FV_NULL_SPACE);

	if(m_kFreeSpaceIDList.empty())
	{
		if(m_iSpaceID < m_iMaxSpaceID)
		{
			++m_iSpaceID;
			iID = m_iSpaceID;
		}
		else
		{
			FV_ERROR_MSG("%s, Can't get SpaceID\n", __FUNCTION__);
		}
	}
	else
	{
		iID = m_kFreeSpaceIDList.front();
		m_kFreeSpaceIDList.pop_front();
	}

	return iID;
}

void FvCellAppMgr::PutSpaceID(FvSpaceID iSpaceID)
{
	m_kFreeSpaceIDList.push_back(iSpaceID);
}

void FvCellAppMgr::CheckSpaceReady()
{
	if(m_bSpaceReady)
		return;

	m_bSpaceReady = true;

	SpaceInfoList::iterator itrB = m_kSpaceInfoList.begin();
	SpaceInfoList::iterator itrE = m_kSpaceInfoList.end();
	for(; itrB!=itrE; ++itrB)
	{
		if(!(*itrB))
			continue;

		if(!(*itrB)->CheckSpaceReady())
		{
			m_bSpaceReady = false;
			break;
		}
	}
}

void FvCellAppMgr::CheckOverLoad()
{
	if(!m_bRun)
		return;

	FvUInt64 uiTimeOut = ::StampsPerSecond() * m_iCellAppTimeoutTime;
	FvUInt64 uiNowTime = ::Timestamp();

	std::vector<FvCellApp*> kCellAppVec;
	{
		CellAppMap::iterator itrB = m_kCellAppMap.begin();
		CellAppMap::iterator itrE = m_kCellAppMap.end();
		while(itrB != itrE)
		{
			if(itrB->second->Channel().LastReceivedTime() + uiTimeOut < uiNowTime)
				kCellAppVec.push_back(itrB->second);
			++itrB;
		}
	}

	if(!kCellAppVec.empty())
	{
		std::vector<FvCellApp*>::iterator itrB = kCellAppVec.begin();
		std::vector<FvCellApp*>::iterator itrE = kCellAppVec.end();
		while(itrB != itrE)
		{
			FvCellApp* pkApp = *itrB;
			FV_INFO_MSG("%s, CellApp:%d, Addr:%s TimeOut\n", __FUNCTION__, pkApp->CellAppID(), pkApp->Addr().c_str());
			LoadSpaceGeometrys& kLoadSpaceGeometrys = m_kLoadSpaceGeometryList[pkApp->LoadSpaceGeometryIdx()];
			for(int i=0; i<(int)kLoadSpaceGeometrys.size(); ++i)
			{
				m_kSpaceInfoList[kLoadSpaceGeometrys[i].m_uiSpaceType]->m_pkCellInfo[kLoadSpaceGeometrys[i].m_uiCellIdx].RemoveCellApp(pkApp);
			}
			m_kCellAppMap.erase(pkApp->Addr());

			if(pkApp->GetCellCnt())
			{
				m_kDeadApps.push_back(pkApp);
			}
			else
			{
				delete pkApp;
			}

			++itrB;
		}

		m_bSpaceReady = false;
		CheckSpaceReady();
		if(!m_bSpaceReady)
		{
			FV_INFO_MSG("%s, Space isn't Ready\n", __FUNCTION__);
		}
	}
}

void FvCellAppMgr::CheckSpaceEmpty()
{
	SpaceMap::iterator itrB = m_kSpaceMap.begin();
	SpaceMap::iterator itrE = m_kSpaceMap.end();
	while(itrB != itrE)
	{
		if(itrB->second->IsEmpty())
		{
			itrB->second->Destroy();
			delete itrB->second;
			SpaceMap::iterator itr = itrB;
			++itrB;
			m_kSpaceMap.erase(itr);
		}
		else
		{
			++itrB;
		}
	}
}

void FvCellAppMgr::CheckDeadCellApp()
{
	DeadCellAppList::iterator itrB = m_kDeadApps.begin();
	while(itrB != m_kDeadApps.end())
	{
		FvCellApp* pkApp = *itrB;
		if(!pkApp->GetCellCnt())
		{
			delete pkApp;
			itrB = m_kDeadApps.erase(itrB);
		}
		else
		{
			++itrB;
		}
	}
}




template <class ARGS_TYPE>
class CellAppMgrMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvCellAppMgr::*Handler)( const ARGS_TYPE & args );

	CellAppMgrMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & /*srcAddr*/,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;
		(FvCellAppMgr::Instance().*handler_)( args );
	}

	Handler handler_;
};

template <class ARGS_TYPE>
class CellAppMgrMessageWithAddrHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvCellAppMgr::*Handler)( const FvNetAddress & addr, const ARGS_TYPE & args );

	CellAppMgrMessageWithAddrHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;
		(FvCellAppMgr::Instance().*handler_)( srcAddr, args );
	}

	Handler handler_;
};

template <class ARGS_TYPE>
class CellAppMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvCellApp::*Handler)( const ARGS_TYPE & args );

	CellAppMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress& srcAddr,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;
		FvCellApp* pkCellApp = FvCellAppMgr::Instance().FindCellApp(srcAddr);
		if(pkCellApp)
		{
			(pkCellApp->*handler_)( args );
		}
	}

	Handler handler_;
};

class CellAppVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvCellApp::*Handler)( FvBinaryIStream & data );

	CellAppVarLenMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FvCellApp* pkCellApp = FvCellAppMgr::Instance().FindCellApp(srcAddr);
		if(pkCellApp)
		{
			(pkCellApp->*handler_)( data );
		}
	}

	Handler handler_;
};

class CellAppMgrVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvCellAppMgr::*Handler)( FvBinaryIStream & data );

	CellAppMgrVarLenMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		(FvCellAppMgr::Instance().*handler_)( data );
	}

	Handler handler_;
};

class CellAppMgrRawMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvCellAppMgr::*Handler)(  const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data  );

	CellAppMgrRawMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		(FvCellAppMgr::Instance().*handler_)( srcAddr, header, data );
	}

	Handler handler_;
};


class CellAppMgrListenerMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvCellAppMgr::*Handler)(const InterfaceListenerMsg& kMsg);

	CellAppMgrListenerMessageHandler(Handler kHandler):m_kHandler(kHandler) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		InterfaceListenerMsg kMsg;
		data >> kMsg;
		FV_ASSERT(data.RemainingLength()==0 && !data.Error());
		(FvCellAppMgr::Instance().*m_kHandler)(kMsg);
	}

	Handler m_kHandler;
};



template <class RETURN_TYPE, class ARGS_TYPE>
class CellAppMgrReturnMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvCellAppMgr::*Handler)(
		const FvNetAddress & srcAddr,
		FvNetReplyID replyID,
		const ARGS_TYPE & args );

	CellAppMgrReturnMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		ARGS_TYPE args;
		data >> args;

		(FvCellAppMgr::Instance().*handler_)( srcAddr, header.m_iReplyID, args );
	}

	Handler handler_;
};







#define DEFINE_SERVER_HERE
#include "FvCellAppManagerInterface.h"

#define DEFINE_INTERFACE_HERE
#include <../FvDBManager/FvDBInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvCell/FvCellAppInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvBase/FvBaseAppIntInterface.h>