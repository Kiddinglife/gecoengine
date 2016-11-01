#include "FvGlobalEntity.h"
#include "FvGlobalEntityManager.h"
#include "FvGlobalMailbox.h"
#include <FvEntityExport.h>



FvEntity::FvEntity()
:m_iEntityID(0)
,m_iDBID(0)
,m_pkExport(NULL)
,m_uiRPCCallBackID(0)
{
}

FvEntity::~FvEntity()
{
}

bool FvEntity::Init(FvEntityID id, FvGlobalEntityExport* pkExport)
{
	m_iEntityID = id;
	//m_iDBID = dbID;
	m_pkExport = pkExport;
	m_kGlobalMB.Set(m_iEntityID, m_pkExport->uiTypeID, FvEntityManager::Instance().Address(), FvEntityMailBoxRef::GLOBAL);
	FvEntityManager::Instance().AddEntity(this);
	return true;
}

FvEntityTypeID FvEntity::GetEntityTypeID() const
{
	return FvEntityTypeID(m_pkExport->uiTypeID);
}

void FvEntity::Destroy()
{
	if(m_kRefCnt.IsSetDestroy())
		return;

	m_kRefCnt.SetDestroy();
	OnDestroy();
	if(!m_kRefCnt.CanDestroy())
		return;

	FvEntityManager::Instance().AddDestroyEntity(this);
}

bool FvEntity::RegisterGlobally(const char* pcName)
{
	return FvEntityManager::Instance().AddGlobalMailBox(pcName, m_kGlobalMB);
}

bool FvEntity::DeregisterGlobally(const char* pcName)
{
	return FvEntityManager::Instance().DelGlobalMailBox(pcName, m_kGlobalMB);
}

void FvEntity::CallGlobalMethod(FvBinaryIStream& kData)
{
	FvInt32 iMethodIdx;
	kData >> iMethodIdx;
	CallGlobalMethod(iMethodIdx, kData);
}

void FvEntity::RPCCallBack(FvBinaryIStream& kData)
{
	FvUInt32 uiCBID;
	kData >> uiCBID;
	//! TODO: 处理回调:Global
	OnRPCResult(uiCBID, kData);

	m_kRefCnt.DelSysRefCnt();
	if(m_kRefCnt.CanDestroy())
		FvEntityManager::Instance().AddDestroyEntity(this);
}

void FvEntity::RPCCallBackException(const GlobalAppInterface::RPCCallBackExceptionArgs& kArgs)
{
	FvUInt32 uiCBID = kArgs.uiCBID;
	//! TODO: 处理回调异常:Global
	OnRPCException(uiCBID);

	m_kRefCnt.DelSysRefCnt();
	if(m_kRefCnt.CanDestroy())
		FvEntityManager::Instance().AddDestroyEntity(this);
}

void FvEntity::CallGlobalMethod( FvInt32 iMethodIdx, FvBinaryIStream & data )
{
	m_pkExport->pFunEntityMethodsEntry(this, FvUInt16(iMethodIdx), data);
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

void FvEntity::RealDestroy()
{
	if(!m_kRefCnt.CanDestroy())
		return;

	//! 清空MailBox,这之后不能调用自己的方法了
	m_kGlobalMB.Clear();

	UnInitialize();

	FvEntityManager::Instance().OnEntityDestroyed(this);
	FvEntityManager::Instance().DelEntity(this);
}

