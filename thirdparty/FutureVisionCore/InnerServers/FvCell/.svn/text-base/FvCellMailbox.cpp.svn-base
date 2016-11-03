#include "FvCellMailBox.h"
#include "FvCellEntityManager.h"
#include <../FvBase/FvBaseAppIntInterface.h>
#include <../FvGlobal/FvGlobalAppInterface.h>
#include <FvEntityExport.h>
#include <FvEntityDefUtility.h>


FvUInt8	FvMailBox::ms_uiState = 0;
FvNetChannel* FvMailBox::ms_pkChannel = NULL;
FvNetChannel* FvMailBox::ms_pkChannelForCB = NULL;
FvUInt8 FvClientsMailBox::ms_uiMessageID = 0;
FvMemoryOStream	FvClientsMailBox::ms_kStream(4096);

enum
{
	MBST_CALL_CLI,		//! 本地调用Client方法
	MBST_CALL_BASE,		//! 本地调用Base方法
	MBST_CALL_CELL,		//! 本地调用Cell方法
	MBST_CALL_GLOBAL,	//! 本地调用Global方法
	MBST_CALL_REMOTE	//! 远端调用
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvBinaryOStream* FvMailBox::GetStream(Destination eDest, FvInt32 iMethodIdx, FvUInt8 uiCBCnt)
{
	FV_ASSERT(eDest>=ToCLIENT && eDest<=ToGLOBAL);

	//! MailBox无效
	if(!IsValid())
	{
		FV_ERROR_MSG("MailBox GetStream Fail, MailBox is Invalid\n");
		return NULL;
	}

	iMethodIdx |= (uiCBCnt << 24);

	//! Base MailBox, 发送到目标BaseApp
	if(GetComponent() == FvEntityMailBoxRef::BASE)
	{
		FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();

		if(eDest == ToCLIENT)
		{
			kBundle.StartMessage(BaseAppIntInterface::CallClientMethod);
		}
		else if(eDest == ToCELL)
		{
			kBundle.StartMessage(BaseAppIntInterface::CallCellMethod);
		}
		else if(eDest == ToBASE)
		{
			kBundle.StartMessage(BaseAppIntInterface::CallBaseMethod);
		}
		else
		{
			FV_ASSERT(0);
			return NULL;
		}

		kBundle << m_kMBRef.m_iID << iMethodIdx;

		ms_uiState = MBST_CALL_REMOTE;
		ms_pkChannel = pkChannel;
		return &kBundle;
	}
	else if(GetComponent() == FvEntityMailBoxRef::CELL)//! Cell MailBox
	{
		//! 发送到本地
		if(m_kMBRef.m_kAddr == FvEntityManager::Instance().Nub().Address())
		{
			FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(m_kMBRef.m_iID);
			if(!pkEntity)
			{
				FvNetAddress* pkAddr = FvEntityManager::Instance().GetIdxChannelFinder().FindAddr(m_kMBRef.m_iID);
				if(!pkAddr)
				{
					FV_INFO_MSG("MailBox GetStream Fail, id:%d can't found\n", m_kMBRef.m_iID);
					return NULL;
				}
				else
				{
					//! 走CellMB路由
					FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(*pkAddr);
					FV_ASSERT(pkChannel);
					FvNetBundle& kBundle = pkChannel->Bundle();

					if(eDest == ToCLIENT)
					{
						kBundle.StartMessage(CellAppInterface::CallClientMethod);
					}
					else if(eDest == ToCELL)
					{
						kBundle.StartMessage(CellAppInterface::RunScriptMethod);
					}
					else if(eDest == ToBASE)
					{
						kBundle.StartMessage(CellAppInterface::CallBaseMethod);
					}
					else
					{
						FV_ASSERT(0);
						return NULL;
					}

					kBundle << m_kMBRef.m_iID << iMethodIdx;

					ms_uiState = MBST_CALL_REMOTE;
					ms_pkChannel = pkChannel;
					return &kBundle;
				}
			}
			else
			{
				if(eDest == ToCLIENT)
				{
					FV_ASSERT(uiCBCnt == 0);
					ms_pkChannel = pkEntity->GetChannelForClientMethod(iMethodIdx);
					if(ms_pkChannel)
					{
						ms_uiState = MBST_CALL_CLI;
						return &ms_pkChannel->Bundle();
					}
					else
					{
						return NULL;
					}
				}
				else if(eDest == ToCELL)
				{
					//! TODO: 暂时用本地回环实现,以后换成帧末调用
					FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
					FV_ASSERT(pkChannel);
					FvNetBundle& kBundle = pkChannel->Bundle();

					kBundle.StartMessage(CellAppInterface::RunScriptMethod);
					kBundle << m_kMBRef.m_iID << iMethodIdx;

					ms_uiState = MBST_CALL_CELL;
					ms_pkChannel = pkChannel;
					return &kBundle;
				}
				else if(eDest == ToBASE)
				{
					ms_pkChannel = pkEntity->GetChannelForBaseMethod(iMethodIdx);
					if(ms_pkChannel)
					{
						ms_uiState = MBST_CALL_BASE;
						return &ms_pkChannel->Bundle();
					}
					else
					{
						return NULL;
					}
				}
				else
				{
					FV_ASSERT(0);
					return NULL;
				}
			}
		}
		else//! 发送到目标CellApp
		{
			FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
			FV_ASSERT(pkChannel);
			FvNetBundle& kBundle = pkChannel->Bundle();

			if(eDest == ToCLIENT)
			{
				kBundle.StartMessage(CellAppInterface::CallClientMethod);
			}
			else if(eDest == ToCELL)
			{
				kBundle.StartMessage(CellAppInterface::RunScriptMethod);
			}
			else if(eDest == ToBASE)
			{
				kBundle.StartMessage(CellAppInterface::CallBaseMethod);
			}
			else
			{
				FV_ASSERT(0);
				return NULL;
			}

			kBundle << m_kMBRef.m_iID << iMethodIdx;

			ms_uiState = MBST_CALL_REMOTE;
			ms_pkChannel = pkChannel;
			return &kBundle;
		}
	}
	else//! Global MailBox
	{
		FV_ASSERT(eDest == ToGLOBAL);

		FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();
		kBundle.StartMessage(GlobalAppInterface::CallGlobalMethod);
		kBundle << m_kMBRef.m_iID << iMethodIdx;

		ms_uiState = MBST_CALL_REMOTE;
		ms_pkChannel = pkChannel;
		return &kBundle;
	}
}

void FvMailBox::SendStream()
{
	switch(ms_uiState)
	{
	case MBST_CALL_REMOTE:
	case MBST_CALL_BASE:
	case MBST_CALL_CELL:
	case MBST_CALL_CLI:
		FV_ASSERT(ms_pkChannel);
		ms_pkChannel->DelayedSend();
		ms_pkChannel = NULL;
		break;
	}
}

FvBinaryOStream* FvMailBox::RPCCallbackInvokeGetStream(FvUInt32 uiID)
{
	//! MailBox无效
	if(!IsValid())
	{
		FV_ERROR_MSG("MailBox GetStream Fail, MailBox is Invalid\n");
		return NULL;
	}

	//! Base MailBox
	if(GetComponent() == FvEntityMailBoxRef::BASE)
	{
		FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();

		kBundle.StartMessage(BaseAppIntInterface::RPCCallBack);
		kBundle << m_kMBRef.m_iID << uiID;

		ms_pkChannelForCB = pkChannel;
		return &kBundle;
	}
	else if(GetComponent() == FvEntityMailBoxRef::CELL)//! Cell MailBox, 发送到目标CellApp
	{
		//! 发送到本地
		if(m_kMBRef.m_kAddr == FvEntityManager::Instance().Nub().Address())
		{
			//! TODO: 能否用本地id替代EntityID,避免查询?
			FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(m_kMBRef.m_iID);
			FvNetChannel* pkChannel = NULL;
			if(!pkEntity)
			{
				FvNetAddress* pkAddr = FvEntityManager::Instance().GetIdxChannelFinder().FindAddr(m_kMBRef.m_iID);
				if(!pkAddr)
				{
					FV_INFO_MSG("MailBox GetStream Fail, id:%d can't found\n", m_kMBRef.m_iID);
					return NULL;
				}
				else
				{
					//! 走CellMB路由
					pkChannel = FvEntityManager::Instance().FindOrCreateChannel(*pkAddr);
				}
			}
			else
			{
				//! TODO: 暂时用本地回环实现,以后换成帧末调用
				pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
			}

			FV_ASSERT(pkChannel);
			FvNetBundle& kBundle = pkChannel->Bundle();

			kBundle.StartMessage(CellAppInterface::RPCCallBack);
			kBundle << m_kMBRef.m_iID << uiID;

			ms_pkChannelForCB = pkChannel;
			return &kBundle;
		}
		else//! 发送到目标BaseApp
		{
			FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
			FV_ASSERT(pkChannel);
			FvNetBundle& kBundle = pkChannel->Bundle();

			kBundle.StartMessage(CellAppInterface::RPCCallBack);
			kBundle << m_kMBRef.m_iID << uiID;

			ms_pkChannelForCB = pkChannel;
			return &kBundle;
		}
	}
	else//! Global MailBox, 发送到GlobalApp
	{
		FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();
		kBundle.StartMessage(GlobalAppInterface::RPCCallBack);
		kBundle << m_kMBRef.m_iID << uiID;

		ms_pkChannelForCB = pkChannel;
		return &kBundle;
	}

	return NULL;
}

void FvMailBox::RPCCallbackInvokeSendStream()
{
	FV_ASSERT(ms_pkChannelForCB);
	ms_pkChannelForCB->DelayedSend();
	ms_pkChannelForCB = NULL;
}

void FvMailBox::RPCCallbackException(FvUInt32 uiID)
{
	//! MailBox无效
	if(!IsValid())
	{
		FV_ERROR_MSG("MailBox GetStream Fail, MailBox is Invalid\n");
		return;
	}

	//! Base MailBox
	if(GetComponent() == FvEntityMailBoxRef::BASE)
	{
		FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();

		kBundle.StartMessage(BaseAppIntInterface::RPCCallBackException);
		BaseAppIntInterface::RPCCallBackExceptionArgs kArgs;
		kArgs.uiCBID = uiID;
		kBundle << m_kMBRef.m_iID << kArgs;

		pkChannel->DelayedSend();
		return;
	}
	else if(GetComponent() == FvEntityMailBoxRef::CELL)//! Cell MailBox, 发送到目标CellApp
	{
		//! 发送到本地
		if(m_kMBRef.m_kAddr == FvEntityManager::Instance().Nub().Address())
		{
			//! TODO: 能否用本地id替代EntityID,避免查询?
			FvEntity* pkEntity = FvEntityManager::Instance().FindEntity(m_kMBRef.m_iID);
			FvNetChannel* pkChannel = NULL;
			if(!pkEntity)
			{
				FvNetAddress* pkAddr = FvEntityManager::Instance().GetIdxChannelFinder().FindAddr(m_kMBRef.m_iID);
				if(!pkAddr)
				{
					FV_INFO_MSG("MailBox GetStream Fail, id:%d can't found\n", m_kMBRef.m_iID);
					return;
				}
				else
				{
					//! 走CellMB路由
					pkChannel = FvEntityManager::Instance().FindOrCreateChannel(*pkAddr);
				}
			}
			else
			{
				//! TODO: 暂时用本地回环实现,以后换成帧末调用
				pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
			}

			FV_ASSERT(pkChannel);
			FvNetBundle& kBundle = pkChannel->Bundle();

			kBundle.StartMessage(CellAppInterface::RPCCallBackException);
			CellAppInterface::RPCCallBackExceptionArgs kArgs;
			kArgs.uiCBID = uiID;
			kBundle << m_kMBRef.m_iID << kArgs;

			pkChannel->DelayedSend();
			return;
		}
		else//! 发送到目标BaseApp
		{
			FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
			FV_ASSERT(pkChannel);
			FvNetBundle& kBundle = pkChannel->Bundle();

			kBundle.StartMessage(CellAppInterface::RPCCallBackException);
			CellAppInterface::RPCCallBackExceptionArgs kArgs;
			kArgs.uiCBID = uiID;
			kBundle << m_kMBRef.m_iID << kArgs;

			pkChannel->DelayedSend();
			return;
		}
	}
	else//! Global MailBox, 发送到GlobalApp
	{
		FvNetChannel* pkChannel = FvEntityManager::Instance().FindOrCreateChannel(m_kMBRef.m_kAddr);
		FV_ASSERT(pkChannel);
		FvNetBundle& kBundle = pkChannel->Bundle();
		kBundle.StartMessage(GlobalAppInterface::RPCCallBackException);
		GlobalAppInterface::RPCCallBackExceptionArgs kArgs;
		kArgs.uiCBID = uiID;
		kBundle << m_kMBRef.m_iID << kArgs;

		pkChannel->DelayedSend();
		return;
	}
}

FvMailBox& FvMailBox::operator=(const FvMailBox& val)
{
	FV_ASSERT(!HasOwner());
	if(this == &val || this->operator ==(val))
		return *this;
	m_kMBRef = val.m_kMBRef;
	return *this;
}

void FvMailBox::Set(FvEntityID iEntityID, FvEntityTypeID uiEntityType, const FvNetAddress& kAddr, FvEntityMailBoxRef::Component eComponent)
{
	FV_ASSERT(!HasOwner());
	m_kMBRef.Init(iEntityID, kAddr, eComponent, uiEntityType);
}

void FvMailBox::Set(const FvEntityMailBoxRef& kMBRef)
{
	FV_ASSERT(!HasOwner());
	m_kMBRef = kMBRef;
}

void FvMailBox::SetComponent(FvEntityMailBoxRef::Component eComponent)
{
	FV_ASSERT(!HasOwner());
	m_kMBRef.SetComponent(eComponent);
}

void FvMailBox::SetType(FvEntityTypeID uiEntityType)
{
	FV_ASSERT(!HasOwner());
	m_kMBRef.SetType(uiEntityType);
}

void FvMailBox::SetID(FvEntityID iEntityID)
{
	FV_ASSERT(!HasOwner());
	m_kMBRef.m_iID = iEntityID;
}

void FvMailBox::SetAddr(const FvNetAddress& kAddr)
{
	FV_ASSERT(!HasOwner());
	m_kMBRef.m_kAddr = kAddr;
}

template<>
FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvMailBox& kMB)
{
	return operator>><FvObjMailBox>(kIS, kMB);
}
template<>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvMailBox& kMB)
{
	return operator<<<FvObjMailBox>(kOS, kMB);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvEntityID FvClientsMailBox::GetEntityID() const
{
	if(m_pkEntity)
		return m_pkEntity->GetEntityID();
	else
		return FV_NULL_ENTITY;
}

FvBinaryOStream* FvClientsMailBox::GetStream(FvInt32 iMethodIdx)
{
	FV_ASSERT(!ms_kStream.Size());

	if(!IsValid())
	{
		FV_ERROR_MSG("%s, MB is Invalid\n", __FUNCTION__);
		return NULL;
	}

	FvCellEntityExport* pkExport = m_pkEntity->GetAttrib().GetExport();
	if(m_pkEntity->IsReal() && pkExport->kMethodCnts.uiClientCnt)
	{
		//! 压缩消息id
		FV_ASSERT(iMethodIdx < pkExport->kMethodCnts.uiClientCnt);

		int baseId, extId;
		FvDataCompressID<6>(pkExport->kMethodCnts.uiClientCnt, iMethodIdx, baseId, extId);
		if(extId != -1)
			ms_kStream << FvUInt8(extId);

		ms_uiMessageID = FvUInt8(baseId);
		return &ms_kStream;
	}
	else
	{
		FV_ERROR_MSG("%s, Entity isn't Real\n", __FUNCTION__);
		return NULL;
	}
}

void FvClientsMailBox::SendStream()
{
	m_pkEntity->CallClientsMethodEvent(ms_uiMessageID, ms_kStream, m_bAllClients);
	ms_kStream.Reset();
}


