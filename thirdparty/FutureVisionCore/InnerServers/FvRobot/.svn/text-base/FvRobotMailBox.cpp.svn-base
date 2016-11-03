#include "FvRobotMailBox.h"
#include "FvServerConnection.h"
#include "FvRobotEntity.h"
#include <FvEntityDefUtility.h>
#include <FvEntityExport.h>


FvMemoryOStream FvMailBox::ms_kStream(4096);
FvUInt8 FvMailBox::ms_uiState = 0;
FvUInt16 FvMailBox::ms_uiMethodID = 0;


void FvMailBox::Init(FvEntity* pkEntity, FvServerConnection* pkServer)
{
	m_pkEntity = pkEntity;
	m_pkServer = pkServer;
}

bool FvMailBox::IsValid() const
{
	if(m_pkEntity && m_pkServer)
		return true;
	else
		return false;
}

FvEntityID FvMailBox::GetEntityID() const
{
	if(m_pkEntity)
		return m_pkEntity->GetEntityID();
	else
		return FV_NULL_ENTITY;
}

FvBinaryOStream* FvMailBox::GetStream(Destination eDest, FvInt32 iMethodIdx, FvUInt8 uiCBCnt)
{
	FV_ASSERT(eDest>=ToCLIENT && eDest<=ToBASE);
	FV_ASSERT(!ms_uiState && !ms_kStream.Size());

	if(!IsValid())
	{
		FV_ERROR_MSG("%s, MB is Invalid\n", __FUNCTION__);
		return NULL;
	}

	if(eDest == ToCLIENT)
	{
		FvRobotEntityExport* pkExport = m_pkEntity->GetAttrib().GetExport();
		FV_ASSERT(pkExport);
		FV_ASSERT(iMethodIdx < pkExport->kMethodCnts.uiClientCnt);
		FV_ASSERT(uiCBCnt == 0);//! 客户端不能有带回调的函数

		ms_uiState = 1;
		ms_uiMethodID = FvUInt16(iMethodIdx);
		return &ms_kStream;
	}
	else if(eDest == ToCELL)
	{
		//! 压缩消息id
		FvRobotEntityExport* pkExport = m_pkEntity->GetAttrib().GetExport();
		FV_ASSERT(pkExport);
		FV_ASSERT(iMethodIdx < pkExport->kMethodCnts.uiCellExpCnt);
		FV_ASSERT(iMethodIdx < 0x1F00);

		FvUInt8 uiBaseID(iMethodIdx >> 8);
		FvUInt8 uiExtID(iMethodIdx);
		if(m_pkEntity->GetEntityID() == m_pkServer->ConnectedID())
		{
			if(uiCBCnt)
			{
				FvBinaryOStream* pkOS = m_pkServer->StartAvatarMessage(uiBaseID | 0x20);
				*pkOS << uiExtID << uiCBCnt;
				return pkOS;
			}
			else
			{
				FvBinaryOStream* pkOS = m_pkServer->StartAvatarMessage(uiBaseID);
				*pkOS << uiExtID;
				return pkOS;
			}
		}
		else
		{
			if(uiCBCnt)
			{
				FvBinaryOStream* pkOS = m_pkServer->StartEntityMessage(uiBaseID | 0x20, m_pkEntity->GetEntityID());
				*pkOS << uiExtID << uiCBCnt;
				return pkOS;
			}
			else
			{
				FvBinaryOStream* pkOS = m_pkServer->StartEntityMessage(uiBaseID, m_pkEntity->GetEntityID());
				*pkOS << uiExtID;
				return pkOS;
			}
		}
	}
	else
	{
		if(m_pkEntity->GetEntityID() == m_pkServer->ConnectedID())
		{
			//! 压缩消息id
			FvRobotEntityExport* pkExport = m_pkEntity->GetAttrib().GetExport();
			FV_ASSERT(pkExport);
			FV_ASSERT(iMethodIdx < pkExport->kMethodCnts.uiBaseExpCnt);
			FV_ASSERT(iMethodIdx < 0x1F00);

			FvUInt8 uiBaseID(iMethodIdx >> 8);
			FvUInt8 uiExtID(iMethodIdx);
			if(uiCBCnt)
			{
				FvBinaryOStream* pkOS = m_pkServer->StartProxyMessage(uiBaseID | 0x20);
				*pkOS << uiExtID << uiCBCnt;
				return pkOS;
			}
			else
			{
				FvBinaryOStream* pkOS = m_pkServer->StartProxyMessage(uiBaseID);
				*pkOS << uiExtID;
				return pkOS;
			}
		}
		else
		{
			FV_ERROR_MSG("ClientMB, Can't call other entities Base Method\n");
			return NULL;
		}
	}
}

void FvMailBox::SendStream()
{
	if(ms_uiState == 1)
	{
		FvRobotEntityExport* pkExport = m_pkEntity->GetAttrib().GetExport();
		FV_ASSERT(pkExport);
		pkExport->pFunEntityMethodsEntry(m_pkEntity, ms_uiMethodID, ms_kStream);
		ms_uiState = 0;
		ms_kStream.Reset();
	}
}

template<>
FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvMailBox& kMailBox)
{
	FvEntityMailBoxRef kRef;
	return kIS >> kRef;
}

template<>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvMailBox& kMailBox)
{
	FvEntityMailBoxRef kRef;
	kRef.Init();
	return kOS << kRef;
}




