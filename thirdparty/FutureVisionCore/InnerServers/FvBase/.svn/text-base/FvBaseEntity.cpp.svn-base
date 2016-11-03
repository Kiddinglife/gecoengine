#include "FvBaseEntity.h"
#include "FvBaseEntityManager.h"
#include "FvClientInterface.h"
#include <../FvCell/FvCellAppInterface.h>
#include <../FvCellAppManager/FvCellAppManagerInterface.h>
#include <../FvDBManager/FvDBInterface.h>
#include <../FvServerCommon/FvWriteDB.h>
#include "FvProxy.h"
#include "FvGlobalBases.h"
#include <FvEntityData.h>
#include <FvEntityExport.h>
#include "FvBaseRPCCallBack.h"

FvMailBox FvEntity::ms_kCellMB;


FvEntity::FvEntity()
:m_iEntityID(0)
,m_iDBID(0)
,m_bIsProxy(false)
,m_pkCellMB(NULL)
,m_pkCellChannel(NULL)
,m_bDeleteFromDB(false)
,m_bWriteToDB(false)
,m_bIsCreateCellPending(false)
,m_bIsDestroyCellPending(false)
#pragma warning (push)
#pragma warning (disable: 4355)
,m_kAttribEventCallBack(this)
#pragma warning (pop)
,m_uiRPCCallBackID(0)
{

}

FvEntity::~FvEntity()
{
	m_kAttrib.DestroyCellData();

#ifndef __DEV_MODE__
	FV_ASSERT(!m_pkCellChannel);
#endif
}

bool FvEntity::Init(FvEntityID id, FvDatabaseID dbID, FvBaseEntityExport* pkExport)
{
	m_iEntityID = id;
	m_iDBID = dbID;
	m_kAttrib.Init(m_iEntityID, this, pkExport);
	m_kAttrib.CreateCellData();
	m_kBaseMB.Set(m_iEntityID, pkExport->uiTypeID, FvEntityManager::Instance().IntNub().Address(), FvEntityMailBoxRef::BASE);
	FvEntityManager::Instance().AddEntity(this);
	return true;
}

FvEntityTypeID FvEntity::GetEntityTypeID() const
{
	return m_kAttrib.GetExport()->uiTypeID;
}

void FvEntity::CreateCellEntity(const FvMailBox* pkCellMB, FvUInt8 uiInitFlg)
{
	if(m_pkCellChannel || IsDestroy())
	{
		goto CreateCellEntity_ERR;
	}

	if(!m_kAttrib.GetExport()->kForm.bHasCell)
	{
		FV_ERROR_MSG("%s, Entity has no Cell\n", __FUNCTION__);
		goto CreateCellEntity_ERR;
	}

	FvCellData* pkCellData = m_kAttrib.GetCellData();

	if(pkCellMB)
	{
		if(pkCellMB->GetComponent() != FvEntityMailBoxRef::CELL)
		{
			FV_ERROR_MSG("%s, MailBox Type(%d) Err\n", __FUNCTION__, pkCellMB->GetComponent());
			goto CreateCellEntity_ERR;
		}
		else if(pkCellMB->GetAddr().IsNone())
		{
			FV_ERROR_MSG("%s, MailBox Addr is NULL\n", __FUNCTION__);
			goto CreateCellEntity_ERR;
		}
		else if(pkCellMB->GetEntityID() == 0)
		{
			FV_ERROR_MSG("%s, MailBox EntityID is 0\n", __FUNCTION__);
			goto CreateCellEntity_ERR;
		}
		else
		{
			if(GetEntityID()<FV_MAX_SERVER_ENTITY && pkCellMB->GetEntityID()>FV_MIN_CLIENT_ENTITY ||
				GetEntityID()>FV_MIN_GLOBAL_ENTITY && pkCellMB->GetEntityID()<FV_MAX_CLIENT_ENTITY)
			{
				FV_ERROR_MSG("%s, MailBox EntityID:%d and MyEntityID:%d is Not in Same range\n", __FUNCTION__, pkCellMB->GetEntityID(), GetEntityID());
				goto CreateCellEntity_ERR;
			}
		}
	}
	else
	{
		if(GetEntityID()<FV_MAX_SERVER_ENTITY && pkCellData->m_iSpaceID>FV_MIN_GLOBAL_SPACE ||
			GetEntityID()>FV_MIN_GLOBAL_ENTITY && pkCellData->m_iSpaceID<FV_MAX_SERVER_SPACE)
		{
			FV_ERROR_MSG("%s, SpaceID:%d and MyEntityID:%d is Not in Same range\n", __FUNCTION__, pkCellData->m_iSpaceID, GetEntityID());
			goto CreateCellEntity_ERR;
		}
	}

	FV_ASSERT(!m_bIsCreateCellPending);

	//! MB为CellMB
	if(pkCellMB)
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
		FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(pkCellMB->GetAddr());
		FvNetBundle& kBundle = pkChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::CreateEntityNearEntity);
		kBundle << pkCellMB->GetEntityID() << pkCellData->m_kPos << pkCellData->m_kDir << FvEntityManager::Instance().IntNub().Address()
			<< GetEntityTypeID() << m_iEntityID << m_iDBID << uiInitFlg;
		m_kAttrib.SerializeToStreamForCellData(kBundle);
		pkChannel->DelayedSend();
	}
	else
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
		FvNetChannel* pkChannel = pkCellData->m_iSpaceID < FV_MAX_SERVER_SPACE ?
			FvEntityManager::Instance().CellAppMgr() :
			FvEntityManager::Instance().GlobalCellAppMgr();
		FvNetBundle& kBundle = pkChannel->Bundle();
		kBundle.StartMessage(CellAppMgrInterface::CreateEntity);
		kBundle << pkCellData->m_iSpaceID << pkCellData->m_kPos << pkCellData->m_kDir << FvEntityManager::Instance().IntNub().Address()
			<< GetEntityTypeID() << m_iEntityID << m_iDBID << uiInitFlg;
		m_kAttrib.SerializeToStreamForCellData(kBundle);
		pkChannel->DelayedSend();
	}

	//! 延后到收到CurrentCell消息后
	//m_pkCellChannel = FvEntityManager::Instance().GetIdxChannelFinder().MakeChannel(m_iEntityID);
	m_bIsCreateCellPending = true;

	return;

CreateCellEntity_ERR:
	OnCreateCellFailure();
}

void FvEntity::CreateInNewSpace(FvUInt8 uiInitFlg)
{
	if(m_pkCellChannel || IsDestroy())
	{
		goto CreateInNewSpace_ERR;
	}

	if(!m_kAttrib.GetExport()->kForm.bHasCell)
	{
		FV_ERROR_MSG("%s, Entity has no Cell\n", __FUNCTION__);
		goto CreateInNewSpace_ERR;
	}

	FV_ASSERT(!m_bIsCreateCellPending);

	FvCellData* pkCellData = m_kAttrib.GetCellData();

	//! TODO: 这里增加SpaceType判断,避免发送错误的类型

	//! FvUInt16 uiSpaceType;
	//! FvVector3 kPos;
	//! FvVector3 kDir;
	//! FvNetAddress kBaseAddr;
	//! FvEntityTypeID uiEntityTypeID;
	//! FvEntityID iEntityID;
	//! FvDatabaseID iDBID;
	//! FvUInt8 uiInitFlg;
	//! Cell部分数据
	FvNetChannel* pkChannel = GetEntityID() < FV_MAX_SERVER_ENTITY ?
		FvEntityManager::Instance().CellAppMgr() :
		FvEntityManager::Instance().GlobalCellAppMgr();
	FvNetBundle& kBundle = pkChannel->Bundle();
	kBundle.StartMessage(CellAppMgrInterface::CreateEntityInNewSpace);
	kBundle << pkCellData->m_uiSpaceType << pkCellData->m_kPos << pkCellData->m_kDir << FvEntityManager::Instance().IntNub().Address()
		<< GetEntityTypeID() << m_iEntityID << m_iDBID << uiInitFlg;
	m_kAttrib.SerializeToStreamForCellData(kBundle);
	pkChannel->DelayedSend();

	//! 延后到收到CurrentCell消息后
	//m_pkCellChannel = FvEntityManager::Instance().GetIdxChannelFinder().MakeChannel(m_iEntityID);
	m_bIsCreateCellPending = true;

	return;

CreateInNewSpace_ERR:
	OnCreateCellFailure();
}

void FvEntity::Destroy(bool bDeleteFromDB, bool bWriteToDB)
{
	if(IsDestroy())
		return;

	//! TODO: 只能return重复调Destroy的情况,m_bIsDestroyCellPending为T时仍需要可以执行,否则需要返回false
	if(m_bIsDestroyCellPending)
		return;

	if(GetEntityID() > FV_MIN_GLOBAL_ENTITY)
	{
		if(bDeleteFromDB || bWriteToDB)
		{
			FV_WARNING_MSG("%s, Entity:%d is Global, Can't write to DB\n", __FUNCTION__, GetEntityID());
			bDeleteFromDB = false;
			bWriteToDB = false;
		}
	}
	else
	{
		m_bDeleteFromDB = bDeleteFromDB;
		m_bWriteToDB = bWriteToDB;
	}

	m_kRefCnt.SetDestroy();
	OnDestroy();
	if(!m_kRefCnt.CanDestroy())
		return;

	//! TODO: 这里有隐患,当Destroy()的调用在m_kDestroyEntities列表的处理中被触发时,会被clear掉
	//! 如:m_kDestroyEntities列表的处理里调用B的UnInitialize(),里面调用A的Destroy()
	FvEntityManager::Instance().AddDestroyEntity(this);
}

void FvEntity::DestroyCellEntity()
{
	//! TODO: 允许在cell未创建好前调用
	if(!(!m_bIsDestroyCellPending && m_pkCellChannel && m_pkCellChannel->IsEstablished()))
		return;

	//! 放到CellLost回来后处理
	//if(IsProxy())
	//{
	//	((FvProxy*)this)->CellChannelClose();
	//}

	m_pkCellChannel->Bundle().StartMessage(CellAppInterface::DestroyEntity);
	m_pkCellChannel->Bundle() << m_iEntityID << FvInt32(0);
	m_pkCellChannel->DelayedSend();

	m_bIsDestroyCellPending = true;
}

class WriteToDBReplyHandler : public FvNetReplyMessageHandler
{
public:
	WriteToDBReplyHandler(FvEntity* pkEntity, EntityWriteToDBCallBack* pkCallBack):m_spEntity(pkEntity),m_pkCallBack(pkCallBack)
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		bool bSuccess;
		FvDatabaseID iDBID;
		data >> bSuccess >> iDBID;

		FV_ASSERT(m_spEntity);
		m_spEntity->SetDBID(iDBID);

		if(m_pkCallBack)
			m_pkCallBack->HanldeCallBack(bSuccess);

		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("Registe to BaseAppMgr Exception\n");
		if(m_pkCallBack)
			m_pkCallBack->HanldeCallBack(false);
		delete this;
	}

	FvEntityPtr					m_spEntity;
	EntityWriteToDBCallBack*	m_pkCallBack;
};

class WriteToDBWaitForCellReplyHandler : public FvNetReplyMessageHandler
{
public:
	WriteToDBWaitForCellReplyHandler(FvEntity* pkEntity, EntityWriteToDBCallBack* pkCallBack):m_spEntity(pkEntity),m_pkCallBack(pkCallBack)
	{}

private:
	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		FvUInt8 flg;
		data >> flg;
		if(flg == 0)//! 失败
		{
			delete this;
			return;
		}

		FV_ASSERT(m_spEntity);
		//m_spEntity->ReadStreamToEntity(data, FvEntityDescription::FV_CELL_DATA|FvEntityDescription::FV_ONLY_PERSISTENT_DATA);
		//m_spEntity->ReadStreamToPosDir(data);

		FvNetBundle& bundle = FvEntityManager::Instance().DBMgr().Bundle();
		bundle.StartRequest(DBInterface::WriteEntity, new WriteToDBReplyHandler(m_spEntity.GetObject(), m_pkCallBack));

		FV_ASSERT(m_spEntity->GetAttrib().GetExport()->kForm.bHasCell);
		bundle << FvInt8(WRITE_ALL_DATA) << m_spEntity->GetEntityTypeID() << m_spEntity->GetDBID() << m_spEntity->GetEntityID();
		m_spEntity->GetAttrib().SerializeToStreamForBaseDBData(bundle);
		bundle.AddBlob(data.Retrieve(0), data.RemainingLength());

		FvEntityManager::Instance().DBMgr().Channel().DelayedSend();

		delete this;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * arg )
	{
		FV_ERROR_MSG("Registe to BaseAppMgr Exception\n");
		if(m_pkCallBack)
			m_pkCallBack->HanldeCallBack(false);
		delete this;
	}

	FvEntityPtr					m_spEntity;
	EntityWriteToDBCallBack*	m_pkCallBack;
};

void FvEntity::WriteToDB(EntityWriteToDBCallBack* pkCallBack)
{
	//! TODO: 调用Destroy后还可以调用WriteToDB来修正之前Destroy的是否存档值吗?pkCallBack怎么办?
	if(IsDestroy())
	{
		if(pkCallBack)
			pkCallBack->HanldeCallBack(false);
		return;
	}

	if(GetEntityID() > FV_MIN_GLOBAL_ENTITY)
	{
		FV_INFO_MSG("%s, Entity:%d is Global, Can't write to DB\n", __FUNCTION__, GetEntityID());
		if(pkCallBack)
			pkCallBack->HanldeCallBack(false);
		return;
	}

	if(m_kAttrib.GetExport()->kForm.bHasCell && m_pkCellMB)
	{
		m_pkCellChannel->Bundle().StartRequest(CellAppInterface::WriteToDBRequest, new WriteToDBWaitForCellReplyHandler(this, pkCallBack));
		m_pkCellChannel->Bundle() << GetEntityID();
		m_pkCellChannel->DelayedSend();
	}
	else
	{
		FvNetBundle& bundle = FvEntityManager::Instance().DBMgr().Bundle();
		bundle.StartRequest(DBInterface::WriteEntity, new WriteToDBReplyHandler(this, pkCallBack));

		bundle << FvInt8(WRITE_ALL_DATA) << GetEntityTypeID() << m_iDBID << m_iEntityID;
		m_kAttrib.SerializeToStreamForDBData(bundle);

		FvEntityManager::Instance().DBMgr().Channel().DelayedSend();
	}
}

int  FvEntity::AddTimer(float fInitialOffset, float fRepeatOffset, void* pUserArg)
{
	return 0;
}

void FvEntity::DelTimer(int iID)
{

}

void FvEntity::RegisterGlobally(const char* pcName, RegisterGlobalBaseCallBack* pkCallBack)
{
	FvGlobalBases* pkGlobalBases = FvEntityManager::Instance().GetGlobalBases();
	FV_ASSERT(pkGlobalBases);
	pkGlobalBases->RegisterRequest(pcName, this, pkCallBack);
}

void FvEntity::DeregisterGlobally(const char* pcName)
{
	FvGlobalBases* pkGlobalBases = FvEntityManager::Instance().GetGlobalBases();
	FV_ASSERT(pkGlobalBases);
	pkGlobalBases->Deregister(pcName, this);
}

bool FvEntity::CopyAllData(FvAllData* pkAllData)
{
	if(!pkAllData)
	{
		FV_ERROR_MSG("%s, AllData is NULL\n", __FUNCTION__);
		return false;
	}
	else if(pkAllData->GetType() != GetEntityTypeID())
	{
		FV_ERROR_MSG("%s, AllData Type(%d) Err, EntityType is %d\n", __FUNCTION__, pkAllData->GetType(), GetEntityTypeID());
		return false;
	}

	FvCellData* pkCellData = m_kAttrib.GetCellData();
	if(m_kAttrib.GetExport()->kForm.bHasCell && !pkCellData)
	{
		FV_ERROR_MSG("%s, CellData is NULL\n", __FUNCTION__);
		return false;
	}

	CopyAttrib(pkAllData, pkCellData);
	return true;
}

void FvEntity::CurrentCell( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	//FV_ASSERT(m_pkCellChannel);
	//FV_ASSERT(args.newCellAddr==srcAddr && srcAddr==m_pkCellChannel->addr());
	//FV_ASSERT(m_kAttrib.GetCellData());

	FvSpaceID		iNewSpaceID;
	FvNetAddress	kNewCellAddr(FvNetAddress::NONE);
	data >> iNewSpaceID;

	//! spaceID为0表示创建Cell实体失败
	if(iNewSpaceID == 0)
	{
		FV_ASSERT(m_bIsCreateCellPending);
		m_bIsCreateCellPending = false;
		//FvEntityManager::Instance().GetIdxChannelFinder().DestroyChannel(m_iEntityID);
		m_pkCellChannel = NULL;
		OnCreateCellFailure();
		return;
	}
	else
	{
		data >> kNewCellAddr;
	}

	//! 创建/更新CellMB
	//m_pkDic->SetSpaceID(iNewSpaceID);
	if(!m_pkCellMB)
	{
		FV_ASSERT(m_kAttrib.GetCellData());
		m_kAttrib.DestroyCellData();

		FV_ASSERT(!m_pkCellChannel);
		m_pkCellChannel = FvEntityManager::Instance().GetIdxChannelFinder().MakeChannel(m_iEntityID, kNewCellAddr);

		m_pkCellMB = new FvMailBox();
		m_pkCellMB->Set(m_iEntityID, GetEntityTypeID(), kNewCellAddr, FvEntityMailBoxRef::CELL);
		OnGetCell();
	}
	else
	{
		m_pkCellMB->SetAddr(kNewCellAddr);
	}
	FV_INFO_MSG("%s, spaceID:%d, Addr:%s, ID:%d\n", __FUNCTION__, iNewSpaceID, kNewCellAddr.c_str(), GetEntityID());

	//! 首次设addr,表示连接第一次ok
	if(m_bIsCreateCellPending)
	{
		m_bIsCreateCellPending = false;

		if(IsProxy())
		{
			((FvProxy*)this)->CellChannelOpen();
		}

		//! 在连接ok前Destroy
		if(IsDestroy())
		{
			FV_ASSERT(!m_bIsDestroyCellPending);
			DestroyCellEntity();
		}
	}
}

void FvEntity::BackupCellEntity( FvBinaryIStream & data )
{

}

void FvEntity::WriteToDB( FvBinaryIStream & data )
{
	//ReadStreamToEntity(data, FvEntityDescription::FV_CELL_DATA|FvEntityDescription::FV_ONLY_PERSISTENT_DATA);
	//ReadStreamToPosDir(data);

	FvNetBundle& bundle = FvEntityManager::Instance().DBMgr().Bundle();
	bundle.StartRequest(DBInterface::WriteEntity, new WriteToDBReplyHandler(this, NULL));

	FV_ASSERT(m_kAttrib.GetExport()->kForm.bHasCell);
	bundle << FvInt8(WRITE_ALL_DATA) << GetEntityTypeID() << m_iDBID << m_iEntityID;
	m_kAttrib.SerializeToStreamForBaseDBData(bundle);
	bundle.AddBlob(data.Retrieve(0), data.RemainingLength());

	FvEntityManager::Instance().DBMgr().Channel().DelayedSend();
}

void FvEntity::CellEntityLost( FvBinaryIStream & data )
{
	//! FvVector3 kPos;
	//! FvVector3 kDir;
	//! FvSpaceID iSpaceID;
	//! FvUInt16 uiSpaceType;
	//! FvTimeStamp uiGameTime;
	//! Cell部分数据
	m_kAttrib.CreateCellData();
	FvCellData* pkCellData = m_kAttrib.GetCellData();
	FV_ASSERT(pkCellData);
	data >> pkCellData->m_kPos >> pkCellData->m_kDir >> pkCellData->m_iSpaceID >> pkCellData->m_uiSpaceType;
	FvTimeStamp uiGameTime;
	data >> uiGameTime;
	m_kAttrib.GetCellData()->DeserializeFromStreamForAllData(data);

	//! 无论Base端调还是Cell端调都在这里处理
	//! m_bIsDestroyCellPending为F表示Cell端主动调用了Destroy
	//if(!m_bIsDestroyCellPending)
	{
		if(IsProxy())
		{
			((FvProxy*)this)->CellChannelClose();
		}
	}
	////! m_bIsDestroyCellPending为F表示Cell端主动调用了Destroy
	//if(!m_bIsDestroyCellPending)
	//{
	//	if(IsProxy())
	//	{
	//		((FvProxy*)this)->CellChannelClose();
	//	}
	//}

	FvEntityManager::Instance().GetIdxChannelFinder().DestroyChannel(m_iEntityID);
	m_pkCellChannel = NULL;
	if(m_pkCellMB)
	{
		delete m_pkCellMB;
		m_pkCellMB = NULL;
		OnLoseCell();
	}

	//! Base端调用了Destroy
	if(IsDestroy())
	{
		m_bIsDestroyCellPending = false;
		if(m_kRefCnt.CanDestroy())
			FvEntityManager::Instance().AddDestroyEntity(this);
	}
	else//! Base端主动调用了DestroyCell
	{
		m_bIsDestroyCellPending = false;
	}
/**
	FvEntityManager::Instance().GetIdxChannelFinder().DestroyChannel(m_iEntityID);
	m_pkCellChannel = NULL;
	if(m_pkCellMB)
	{
		delete m_pkCellMB;
		m_pkCellMB = NULL;
		OnLoseCell();
	}

	//! m_bIsDestroyed为T表示Base端调用了Destroy
	//! m_bIsDestroyCellPending为F表示Cell端主动调用了Destroy
	if(m_bIsDestroyed || !m_bIsDestroyCellPending)
	{
		if(!m_bIsDestroyCellPending)
			m_bWriteToDB = true;//! Cell端主动发起,默认需要存DB
		m_bIsDestroyCellPending = false;
		DestroySelf(m_bDeleteFromDB, m_bWriteToDB);
	}
	else//! Base端主动调用了DestroyCell
	{
		m_bIsDestroyCellPending = false;
	}
**/
}

void FvEntity::StartKeepAlive( const FvNetAddress & srcAdr, const BaseAppIntInterface::StartKeepAliveArgs & args )
{

}

void FvEntity::CallBaseMethod( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FvInt32 iMethodIdx;
	data >> iMethodIdx;
	CallBaseMethod(iMethodIdx, data);
}

void FvEntity::CallCellMethod( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	FvInt32 iMethodIdx;
	data >> iMethodIdx;
	CallCellMethod(iMethodIdx, data);
}

void FvEntity::RPCCallBack( FvBinaryIStream & data )
{
	FvUInt32 uiCBID;
	data >> uiCBID;
	if(data.Error())
		return;

	//! 服务器端回调
	if(uiCBID < 0xFFFFFF00)
	{
		//! TODO: 处理回调:Base
		OnRPCResult(uiCBID, data);

		m_kRefCnt.DelSysRefCnt();
		if(m_kRefCnt.CanDestroy())
			FvEntityManager::Instance().AddDestroyEntity(this);
	}
	else//! 客户端回调
	{
		if(IsProxy())
		{
			((FvProxy*)this)->CallClientRPCCallBack(uiCBID, data);
		}
		else
		{
			FV_ERROR_MSG("Can't Do Client RPCCallBack(%d), Entity(%d) is NOT Proxy\n", uiCBID, GetEntityID());
			data.Finish();
		}
	}
}

void FvEntity::RPCCallBackException( const BaseAppIntInterface::RPCCallBackExceptionArgs& args )
{
	FvUInt32 uiCBID = args.uiCBID;

	//! 服务器端回调
	if(uiCBID < 0xFFFFFF00)
	{
		//! TODO: 处理回调异常:Base
		OnRPCException(uiCBID);

		m_kRefCnt.DelSysRefCnt();
		if(m_kRefCnt.CanDestroy())
			FvEntityManager::Instance().AddDestroyEntity(this);
	}
	else//! 客户端回调
	{
		if(IsProxy())
		{
			((FvProxy*)this)->CallClientRPCCallBackException(uiCBID);
		}
		else
		{
			FV_ERROR_MSG("Can't Do Client RPCCallBackException(%d), Entity(%d) is NOT Proxy\n", uiCBID, GetEntityID());
		}
	}
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
	//if(!m_pkEntityDes->AddDictionaryToStream(m_pkDic, stream, FvEntityDescription::FV_BASE_DATA|FvEntityDescription::FV_CELL_DATA))
	//	return false;

	////! stream>>section
	//FvXMLSectionPtr spSection = FvXMLSection::CreateSection("root");
	//if(!m_pkEntityDes->ReadStreamToSection(stream, FvEntityDescription::FV_BASE_DATA|FvEntityDescription::FV_CELL_DATA, spSection))
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

void FvEntity::SetDBID(FvDatabaseID iDBID)
{
	FV_ASSERT(!m_iDBID || m_iDBID==iDBID);
	m_iDBID = iDBID;
}

void FvEntity::CallBaseMethod( FvInt32 iMethodIdx, FvBinaryIStream & data )
{
	m_kAttrib.OnMethodFromServer(iMethodIdx, data);
}

void FvEntity::CallCellMethod( FvInt32 iMethodIdx, FvBinaryIStream & data )
{
	if(!m_pkCellChannel)
	{
		FV_ERROR_MSG("%s, cell channel is null\n", __FUNCTION__);
		CallRPCCallbackException(iMethodIdx, data);
		return;
	}

	FvNetBundle& kBundle = m_pkCellChannel->Bundle();
	kBundle.StartMessage(CellAppInterface::RunScriptMethod);
	kBundle << GetEntityID() << iMethodIdx;
	kBundle.Transfer(data, data.RemainingLength());
	m_pkCellChannel->DelayedSend();
}

FvNetChannel* FvEntity::GetChannelForCellMethod(FvInt32 iMethodIdx)
{
	if(!m_pkCellChannel)
	{
		FV_ERROR_MSG("%s, cell channel is null\n", __FUNCTION__);
		return NULL;
	}

	FvNetBundle& kBundle = m_pkCellChannel->Bundle();
	kBundle.StartMessage(CellAppInterface::RunScriptMethod);
	kBundle << GetEntityID() << iMethodIdx;
	return m_pkCellChannel;
}

void FvEntity::AfterInitialize()
{
	m_kAttrib.SetAttribEventCallBack(&m_kAttribEventCallBack);
	m_kAttrib.AttachOwner();
}

void FvEntity::OnlineLock()
{
	m_kRefCnt.AddUsrRefCnt();
}

void FvEntity::OnlineUnLock()
{
	m_kRefCnt.DelUsrRefCnt();
	if(m_kRefCnt.CanDestroy())
		FvEntityManager::Instance().AddDestroyEntity(this);
}

FvUInt32 FvEntity::NewRPCCallBackID()
{
	FV_ASSERT(m_uiRPCCallBackID<0xFFFFFF00);
	m_kRefCnt.AddSysRefCnt();
	return ++m_uiRPCCallBackID;
}

void FvEntity::DestroySelf(bool bDeleteFromDB, bool bWriteToDB)
{
	//! 清空MailBox,这之后不能调用自己的方法了
	m_kBaseMB.Clear();
	FV_ASSERT(!m_pkCellMB);

	m_kAttrib.SetAttribEventCallBack(NULL);
	UnInitialize();

	//FvNetAddress kClientAddr(FvNetAddress::NONE);
	if(IsProxy())
	{
		//if(((FvProxy*)this)->HasClient())
		//	kClientAddr = ((FvProxy*)this)->ClientChannel().addr();
		((FvProxy*)this)->DestroyClientChannel(true);
	}

	//! DBid不为0表示在DB中有Entity数据
	if(m_iDBID && bDeleteFromDB)
	{
		//! 只有DB中有数据,才可以删除
		FvNetBundle& bundle = FvEntityManager::Instance().DBMgr().Bundle();
		bundle.StartMessage(DBInterface::WriteEntity);

		//! 删除+LogOff
		bundle << FvInt8(WRITE_LOG_OFF|WRITE_DELETE_FROM_DB) << GetEntityTypeID() << m_iDBID;

		FvEntityManager::Instance().DBMgr().Channel().DelayedSend();
	}
	else if(bWriteToDB)
	{
		//! 如果要保存,则无论Entity在DB中有没有数据都可以保存
		FvNetBundle& bundle = FvEntityManager::Instance().DBMgr().Bundle();
		bundle.StartMessage(DBInterface::WriteEntity);

		FvInt8 flg;
		if(m_iDBID)		//! Entity在DB中有数据,则保存+LogOff
			flg = FvInt8(WRITE_LOG_OFF|WRITE_ALL_DATA);	
		else			//! Entity在DB中没有数据,则只保存(新建)
			flg = FvInt8(WRITE_ALL_DATA);
		bundle << flg << GetEntityTypeID() << m_iDBID << m_iEntityID;
		m_kAttrib.SerializeToStreamForDBData(bundle);
		//bundle << FvTimeStamp(0);

		FvEntityManager::Instance().DBMgr().Channel().DelayedSend();
	}
	else if(m_iDBID)
	{
		//! 既不删除也不保存,则如果Entity在DB中有数据,必须做LogOff处理
		FvNetBundle& bundle = FvEntityManager::Instance().DBMgr().Bundle();
		bundle.StartMessage(DBInterface::WriteEntity);

		bundle << FvInt8(WRITE_LOG_OFF) << GetEntityTypeID() << m_iDBID << m_iEntityID;

		FvEntityManager::Instance().DBMgr().Channel().DelayedSend();
	}

	FvGlobalBases* pkGlobalBases = FvEntityManager::Instance().GetGlobalBases();
	FV_ASSERT(pkGlobalBases);
	pkGlobalBases->OnBaseDestroyed(this);

	FvEntityManager::Instance().DelEntity(this);
}

void FvEntity::RealDestroy()
{
	if(!m_kRefCnt.CanDestroy())
		return;

	//! 先销毁cell部分
	if(m_pkCellChannel)
	{
		DestroyCellEntity();
	}
	else
	{
		DestroySelf(m_bDeleteFromDB, m_bWriteToDB);
	}
}

void BaseAttribEventCallBack::HandleEvent(FvUInt8 uiMessageID, FvBinaryIStream& kIS, bool bOwnCli, bool bGhost, bool bOthersCli, bool bDB)
{
	if(bOwnCli && m_pkEntity->IsProxy())
	{
		((FvProxy*)m_pkEntity)->SyncAttribToOwnCli(uiMessageID, kIS);
	}
}


