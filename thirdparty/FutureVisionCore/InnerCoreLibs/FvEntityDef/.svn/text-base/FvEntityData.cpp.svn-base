#include "FvEntityData.h"
#include "FvEntityExport.h"
#include "FvEntityDefUtility.h"
#include "FvDataObj.h"
#include "FvDataScanf.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvUInt32 FvClientData::Size(FvUInt32*& pAddr) const
{
	FvUInt32 uiCnt(0);
	GetDataAddr(uiCnt, pAddr);
	return uiCnt;
}

FvDataObj* FvClientData::Get(FvUInt32* pAddr, FvUInt32 uiIdx) const
{
	FV_ASSERT(pAddr);
	return (FvDataObj*)((char*)this + pAddr[uiIdx]);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvClientAttrib::FvClientAttrib()
:m_pkEntityExport(NULL)
,m_pkClientData(NULL)
#ifndef FV_SHIPPING
,m_uiEntityID(FV_NULL_ENTITY)
#endif
{

}

FvClientAttrib::~FvClientAttrib()
{
	if(m_pkClientData)
		delete m_pkClientData;
}

bool FvClientAttrib::CreateOwnData(FvEntityID uiEntityID, FvEntity* pkEntity, FvClientEntityExport* pkEntityExport)
{
	FV_ASSERT(pkEntity && pkEntityExport && !m_pkClientData);

#ifndef FV_SHIPPING
	m_uiEntityID = uiEntityID;
#endif

	m_pkEntityExport = pkEntityExport;
	if(m_pkEntityExport->pFunCreateOwnData)
	{
		m_pkClientData = m_pkEntityExport->pFunCreateOwnData();
		BindData(pkEntity, 1);
	}
	return true;
}

bool FvClientAttrib::CreateOthersData(FvEntityID uiEntityID, FvEntity* pkEntity, FvClientEntityExport*	pkEntityExport)
{
	FV_ASSERT(pkEntity && pkEntityExport && !m_pkClientData);

#ifndef FV_SHIPPING
	m_uiEntityID = uiEntityID;
#endif

	m_pkEntityExport = pkEntityExport;
	if(m_pkEntityExport->pFunCreateOthersData)
	{
		m_pkClientData = m_pkEntityExport->pFunCreateOthersData();
		BindData(pkEntity, 0);
	}
	return true;
}

bool FvClientAttrib::DeserializeFromStreamForBaseData(FvEntity* pkEntity, FvBinaryIStream& kIS)
{
	FV_ASSERT(pkEntity && m_pkEntityExport && m_pkClientData);

	FvUInt16* pkIdx = m_pkEntityExport->kBaseToCli.pkAttribIdx;
	FvUInt16* pkEnd = pkIdx + m_pkEntityExport->kBaseToCli.uiCnt;
	FvUInt32* pkRefAddr = m_pkEntityExport->kAttribAddr.pkRefAddr;
	for(; pkIdx!=pkEnd; ++pkIdx)
	{
		FV_ASSERT(*pkIdx < m_pkEntityExport->kAttribAddr.uiRefCnt);

		FvDataObj** ppkObj = (FvDataObj**)((char*)pkEntity + pkRefAddr[*pkIdx]);
		(*ppkObj)->DeserializeFromStream(kIS);
	}

	return true;
}

bool FvClientAttrib::DeserializeFromStreamForOwnCliData(FvEntity* pkEntity, FvBinaryIStream& kIS)
{
	FV_ASSERT(pkEntity && m_pkEntityExport && m_pkClientData);

	FvUInt16* pkIdx = m_pkEntityExport->kCellToOwnCli.pkAttribIdx;
	FvUInt16* pkEnd = pkIdx + m_pkEntityExport->kCellToOwnCli.uiCnt;
	FvUInt32* pkRefAddr = m_pkEntityExport->kAttribAddr.pkRefAddr;
	for(; pkIdx!=pkEnd; ++pkIdx)
	{
		FV_ASSERT(*pkIdx < m_pkEntityExport->kAttribAddr.uiRefCnt);

		FvDataObj** ppkObj = (FvDataObj**)((char*)pkEntity + pkRefAddr[*pkIdx]);
		(*ppkObj)->DeserializeFromStream(kIS);
	}

	return true;
}

bool FvClientAttrib::OnProperties(FvEntity* pkEntity, FvBinaryIStream& kIS)
{
	FV_ASSERT(pkEntity && m_pkEntityExport && m_pkClientData);

	FvUInt32* pkRefAddr = m_pkEntityExport->kAttribAddr.pkRefAddr;

	FvUInt16 uiCnt;
	kIS >> uiCnt;
	FV_ASSERT(!kIS.Error());

	for(FvUInt16 i=0; i<uiCnt; ++i)
	{
		FvUInt16 uiIdx;
		kIS >> uiIdx;
		FV_ASSERT(!kIS.Error());

		FV_ASSERT(uiIdx < m_pkEntityExport->kAttribAddr.uiRefCnt);
		FvDataObj** ppkObj = (FvDataObj**)((char*)pkEntity + pkRefAddr[uiIdx]);
		(*ppkObj)->DeserializeFromStream(kIS);
	}

	return true;
}

bool FvClientAttrib::OnProperty(bool bOwn, FvEntity* pkEntity, int iMessageID, FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntityExport && m_pkClientData);

#ifndef FV_SHIPPING
	//FV_INFO_MSG("PropUpdate, bOwn:%d, Entity:%d, Msg:%d, Len:%d\n", bOwn, m_uiEntityID, iMessageID, kIS.RemainingLength());
#endif

	FvUInt16 uiDataID;
	if(bOwn)
	{
		FV_ASSERT(m_pkEntityExport->kDataIDToPath.pkOwnDataPath);
		uiDataID = FvDataOwner::DoDiffRenovate(iMessageID, kIS, pkEntity, m_pkClientData,
												m_pkEntityExport->kDataIDToPath.uiDataIDCnt,
												m_pkEntityExport->kDataIDToPath.pkOwnDataPath,
												m_pkEntityExport->pkDataCallBacks);
	}
	else
	{
		FV_ASSERT(m_pkEntityExport->kDataIDToPath.pkOthersDataPath);
		uiDataID = FvDataOwner::DoDiffRenovate(iMessageID, kIS, pkEntity, m_pkClientData,
												m_pkEntityExport->kDataIDToPath.uiDataIDCnt,
												m_pkEntityExport->kDataIDToPath.pkOthersDataPath,
												m_pkEntityExport->pkDataCallBacks);
	}

	return true;
}

bool FvClientAttrib::OnMethod(FvEntity* pkEntity, int iMessageID, FvBinaryIStream& kIS)
{
	FV_ASSERT(pkEntity && m_pkEntityExport && m_pkClientData && 0<=iMessageID && iMessageID<64);

	int iID;
	bool bRet = FvDataDecompressID<6>(m_pkEntityExport->kMethodCnts.uiClientCnt, iID, iMessageID, kIS);
	FV_ASSERT(bRet);

	FV_ASSERT(m_pkEntityExport->pFunEntityMethodsEntry);
	m_pkEntityExport->pFunEntityMethodsEntry(pkEntity, FvUInt16(iID), kIS);
	return true;
}

FvCallFuncByStrHandler* FvClientAttrib::FindStrFuncHandler(const FvString& kFuncName, const FvString& kProtoType) const
{
	FV_ASSERT(m_pkEntityExport);
	FvString kCapsName = kFuncName;
	std::transform(kCapsName.begin(), kCapsName.end(), kCapsName.begin(), toupper);
	CallFuncByStrHandlerMap::const_iterator itr = m_pkEntityExport->kStrFuncHandlerMap.find(kCapsName);
	if(itr != m_pkEntityExport->kStrFuncHandlerMap.end())
	{
		const CallFuncByStrHandlerList& kList = itr->second;
		CallFuncByStrHandlerList::const_iterator itrB = kList.begin();
		CallFuncByStrHandlerList::const_iterator itrE = kList.end();
		for(; itrB!=itrE; ++itrB)
		{
			if(FvDataScanf::IsType(kProtoType, itrB->first))
				return itrB->second;
		}
		return NULL;
	}
	else
	{
		return NULL;
	}
}

bool FvClientAttrib::HasStrFuncName(const FvString& kFuncName) const
{
	FV_ASSERT(m_pkEntityExport);
	FvString kCapsName = kFuncName;
	std::transform(kCapsName.begin(), kCapsName.end(), kCapsName.begin(), toupper);
	CallFuncByStrHandlerMap::const_iterator itr = m_pkEntityExport->kStrFuncHandlerMap.find(kCapsName);
	if(itr != m_pkEntityExport->kStrFuncHandlerMap.end())
		return true;
	else
		return false;
}

void FvClientAttrib::BindData(FvEntity* pkEntity, FvUInt8 bOwn)
{
	FV_ASSERT(pkEntity && m_pkEntityExport && m_pkClientData);

	FvUInt32* pkAddr;
	FvUInt32 uiCnt = m_pkClientData->Size(pkAddr);

	FvUInt32* pkRefAddr = m_pkEntityExport->kAttribAddr.pkRefAddr;
	FvUInt16* pkIdx = bOwn ? m_pkEntityExport->kAttribAddr.pkOwnIdx : m_pkEntityExport->kAttribAddr.pkOthersIdx;

	for(FvUInt32 i=0; i<uiCnt; ++i, ++pkIdx)
	{
		FvUInt16 uiIdx = *pkIdx;
		FV_ASSERT(i < m_pkEntityExport->kAttribAddr.uiRefCnt && 
			uiIdx < m_pkEntityExport->kAttribAddr.uiRefCnt);

		FvDataObj** ppkObj = (FvDataObj**)((char*)pkEntity + pkRefAddr[uiIdx]);
		*ppkObj = m_pkClientData->Get(pkAddr, i);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvRobotAttrib::FvRobotAttrib()
:m_pkEntityExport(NULL)
,m_pkClientData(NULL)
#ifndef FV_SHIPPING
,m_uiEntityID(FV_NULL_ENTITY)
#endif
{

}

FvRobotAttrib::~FvRobotAttrib()
{
	if(m_pkClientData)
		delete m_pkClientData;
}

bool FvRobotAttrib::CreateOwnData(FvEntityID uiEntityID, FvEntity* pkEntity, FvRobotEntityExport* pkEntityExport)
{
	FV_ASSERT(pkEntity && pkEntityExport && !m_pkClientData);

#ifndef FV_SHIPPING
	m_uiEntityID = uiEntityID;
#endif

	m_pkEntityExport = pkEntityExport;
	if(m_pkEntityExport->pFunCreateOwnData)
	{
		m_pkClientData = m_pkEntityExport->pFunCreateOwnData();
		BindData(pkEntity, 1);
	}
	return true;
}

bool FvRobotAttrib::CreateOthersData(FvEntityID uiEntityID, FvEntity* pkEntity, FvRobotEntityExport* pkEntityExport)
{
	FV_ASSERT(pkEntity && pkEntityExport && !m_pkClientData);

#ifndef FV_SHIPPING
	m_uiEntityID = uiEntityID;
#endif

	m_pkEntityExport = pkEntityExport;
	if(m_pkEntityExport->pFunCreateOthersData)
	{
		m_pkClientData = m_pkEntityExport->pFunCreateOthersData();
		BindData(pkEntity, 0);
	}
	return true;
}

bool FvRobotAttrib::DeserializeFromStreamForBaseData(FvEntity* pkEntity, FvBinaryIStream& kIS)
{
	FV_ASSERT(pkEntity && m_pkEntityExport && m_pkClientData);

	FvUInt16* pkIdx = m_pkEntityExport->kBaseToCli.pkAttribIdx;
	FvUInt16* pkEnd = pkIdx + m_pkEntityExport->kBaseToCli.uiCnt;
	FvUInt32* pkRefAddr = m_pkEntityExport->kAttribAddr.pkRefAddr;
	for(; pkIdx!=pkEnd; ++pkIdx)
	{
		FV_ASSERT(*pkIdx < m_pkEntityExport->kAttribAddr.uiRefCnt);

		FvDataObj** ppkObj = (FvDataObj**)((char*)pkEntity + pkRefAddr[*pkIdx]);
		(*ppkObj)->DeserializeFromStream(kIS);
	}

	return true;
}

bool FvRobotAttrib::DeserializeFromStreamForOwnCliData(FvEntity* pkEntity, FvBinaryIStream& kIS)
{
	FV_ASSERT(pkEntity && m_pkEntityExport && m_pkClientData);

	FvUInt16* pkIdx = m_pkEntityExport->kCellToOwnCli.pkAttribIdx;
	FvUInt16* pkEnd = pkIdx + m_pkEntityExport->kCellToOwnCli.uiCnt;
	FvUInt32* pkRefAddr = m_pkEntityExport->kAttribAddr.pkRefAddr;
	for(; pkIdx!=pkEnd; ++pkIdx)
	{
		FV_ASSERT(*pkIdx < m_pkEntityExport->kAttribAddr.uiRefCnt);

		FvDataObj** ppkObj = (FvDataObj**)((char*)pkEntity + pkRefAddr[*pkIdx]);
		(*ppkObj)->DeserializeFromStream(kIS);
	}

	return true;
}

bool FvRobotAttrib::OnProperties(FvEntity* pkEntity, FvBinaryIStream& kIS)
{
	FV_ASSERT(pkEntity && m_pkEntityExport && m_pkClientData);

	FvUInt32* pkRefAddr = m_pkEntityExport->kAttribAddr.pkRefAddr;

	FvUInt16 uiCnt;
	kIS >> uiCnt;
	FV_ASSERT(!kIS.Error());

	for(FvUInt16 i=0; i<uiCnt; ++i)
	{
		FvUInt16 uiIdx;
		kIS >> uiIdx;
		FV_ASSERT(!kIS.Error());

		FV_ASSERT(uiIdx < m_pkEntityExport->kAttribAddr.uiRefCnt);
		FvDataObj** ppkObj = (FvDataObj**)((char*)pkEntity + pkRefAddr[uiIdx]);
		(*ppkObj)->DeserializeFromStream(kIS);
	}

	return true;
}

bool FvRobotAttrib::OnProperty(bool bOwn, FvEntity* pkEntity, int iMessageID, FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntityExport && m_pkClientData);

#ifndef FV_SHIPPING
	//FV_INFO_MSG("PropUpdate, bOwn:%d, Entity:%d, Msg:%d, Len:%d\n", bOwn, m_uiEntityID, iMessageID, kIS.RemainingLength());
#endif

	FvUInt16 uiDataID;
	if(bOwn)
	{
		FV_ASSERT(m_pkEntityExport->kDataIDToPath.pkOwnDataPath);
		uiDataID = FvDataOwner::DoDiffRenovate(iMessageID, kIS, pkEntity, m_pkClientData,
			m_pkEntityExport->kDataIDToPath.uiDataIDCnt,
			m_pkEntityExport->kDataIDToPath.pkOwnDataPath,
			m_pkEntityExport->pkDataCallBacks);
	}
	else
	{
		FV_ASSERT(m_pkEntityExport->kDataIDToPath.pkOthersDataPath);
		uiDataID = FvDataOwner::DoDiffRenovate(iMessageID, kIS, pkEntity, m_pkClientData,
			m_pkEntityExport->kDataIDToPath.uiDataIDCnt,
			m_pkEntityExport->kDataIDToPath.pkOthersDataPath,
			m_pkEntityExport->pkDataCallBacks);
	}

	return true;
}

bool FvRobotAttrib::OnMethod(FvEntity* pkEntity, int iMessageID, FvBinaryIStream& kIS)
{
	FV_ASSERT(pkEntity && m_pkEntityExport && m_pkClientData && 0<=iMessageID && iMessageID<64);

	int iID;
	bool bRet = FvDataDecompressID<6>(m_pkEntityExport->kMethodCnts.uiClientCnt, iID, iMessageID, kIS);
	FV_ASSERT(bRet);

	FV_ASSERT(m_pkEntityExport->pFunEntityMethodsEntry);
	m_pkEntityExport->pFunEntityMethodsEntry(pkEntity, FvUInt16(iID), kIS);
	return true;
}

void FvRobotAttrib::BindData(FvEntity* pkEntity, FvUInt8 bOwn)
{
	FV_ASSERT(pkEntity && m_pkEntityExport && m_pkClientData);

	FvUInt32* pkAddr;
	FvUInt32 uiCnt = m_pkClientData->Size(pkAddr);

	FvUInt32* pkRefAddr = m_pkEntityExport->kAttribAddr.pkRefAddr;
	FvUInt16* pkIdx = bOwn ? m_pkEntityExport->kAttribAddr.pkOwnIdx : m_pkEntityExport->kAttribAddr.pkOthersIdx;

	for(FvUInt32 i=0; i<uiCnt; ++i, ++pkIdx)
	{
		FvUInt16 uiIdx = *pkIdx;
		FV_ASSERT(i < m_pkEntityExport->kAttribAddr.uiRefCnt && 
			uiIdx < m_pkEntityExport->kAttribAddr.uiRefCnt);

		FvDataObj** ppkObj = (FvDataObj**)((char*)pkEntity + pkRefAddr[uiIdx]);
		*ppkObj = m_pkClientData->Get(pkAddr, i);
	}
}


#ifdef FV_SERVER

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvBaseAttrib::FvBaseAttrib()
:FvDataOwner()
,m_pkEntity(NULL)
,m_pkEntityExport(NULL)
,m_pkCellData(NULL)
,m_uiDataIdx(0)
,m_uiDataID(0)
,m_uiMessageID(0)
,m_pkEvtCallBack(NULL)
#ifndef FV_SHIPPING
,m_uiEntityID(FV_NULL_ENTITY)
#endif
{

}

FvBaseAttrib::~FvBaseAttrib()
{
	if(m_pkCellData)
		delete m_pkCellData;
}

bool FvBaseAttrib::Init(FvEntityID uiEntityID, FvEntity* pkEntity, FvBaseEntityExport* pkEntityExport)
{
	FV_ASSERT(pkEntity && pkEntityExport && !m_pkEntity && !m_pkEntityExport);
#ifndef FV_SHIPPING
	m_uiEntityID = uiEntityID;
#endif
	m_pkEntity = pkEntity;
	m_pkEntityExport = pkEntityExport;
	return true;
}

bool FvBaseAttrib::CreateCellData()
{
	FV_ASSERT(m_pkEntityExport && !m_pkCellData);

	if(m_pkEntityExport->kForm.bHasCell)
		m_pkCellData = m_pkEntityExport->pFunCreateCellData();
	return true;
}

void FvBaseAttrib::DestroyCellData()
{
	if(m_pkCellData)
	{
		delete m_pkCellData;
		m_pkCellData = NULL;
	}
}

void FvBaseAttrib::AttachOwner()
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kAttribInfo.uiCnt;
	AttribInfo* pkInfo = m_pkEntityExport->kAttribInfo.pkInfo;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkInfo)
	{
		FvDataOwner* pkData = (FvDataOwner*)((char*)m_pkEntity + pkInfo->uiAddr);
		pkData->Attach(this);
	}
}

bool FvBaseAttrib::SerializeToStreamForDBData(FvBinaryOStream& kOS) const
{
	SerializeToStreamForBaseDBData(kOS);
	if(m_pkCellData)
		m_pkCellData->SerializeToStreamForDBData(kOS);
	return true;
}

bool FvBaseAttrib::SerializeToStreamForCellData(FvBinaryOStream& kOS) const
{
	if(m_pkCellData)
		m_pkCellData->SerializeToStreamForAllData(kOS);
	return true;
}

bool FvBaseAttrib::SerializeToStreamForCliData(FvBinaryOStream& kOS) const
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kBaseToCli.uiCnt;
	FvUInt16* pkIdx = m_pkEntityExport->kBaseToCli.pkAttribIdx;
	AttribInfo* pkInfo = m_pkEntityExport->kAttribInfo.pkInfo;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkIdx)
	{
		FV_ASSERT(*pkIdx < m_pkEntityExport->kAttribInfo.uiCnt && pkInfo);
		((FvDataObj*)((char*)m_pkEntity + pkInfo[*pkIdx].uiAddr))->SerializeToStream(kOS);
	}

	return true;
}

bool FvBaseAttrib::SerializeToStreamForBaseDBData(FvBinaryOStream& kOS) const
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kDBToBase.uiCnt;
	FvUInt16* pkIdx = m_pkEntityExport->kDBToBase.pkAttribIdx;
	AttribInfo* pkInfo = m_pkEntityExport->kAttribInfo.pkInfo;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkIdx)
	{
		FV_ASSERT(*pkIdx < m_pkEntityExport->kAttribInfo.uiCnt && pkInfo);
		FvDataObj* pkData = (FvDataObj*)((char*)m_pkEntity + pkInfo[*pkIdx].uiAddr);
		pkData->SerializeToStream(kOS);
	}

	return true;
}

bool FvBaseAttrib::DeserializeFromStreamForDBData(FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kDBToBase.uiCnt;
	FvUInt16* pkIdx = m_pkEntityExport->kDBToBase.pkAttribIdx;
	AttribInfo* pkInfo = m_pkEntityExport->kAttribInfo.pkInfo;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkIdx)
	{
		FV_ASSERT(*pkIdx < m_pkEntityExport->kAttribInfo.uiCnt && pkInfo);
		FvDataObj* pkData = (FvDataObj*)((char*)m_pkEntity + pkInfo[*pkIdx].uiAddr);
		pkData->DeserializeFromStream(kIS);
	}

	if(m_pkCellData)
		m_pkCellData->DeserializeFromStreamForDBData(kIS);
	return true;
}

bool FvBaseAttrib::DeserializeFromStreamForAllData(FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kAllAttribInfo.uiCnt;
	BaseAttribIdxInfo* pkIdx = m_pkEntityExport->kAllAttribInfo.pkIdx;
	AttribInfo* pkInfo = m_pkEntityExport->kAttribInfo.pkInfo;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkIdx)
	{
		if(pkIdx->uiIsBaseAttrib)
		{
			FV_ASSERT(pkIdx->uiAttribIdx < m_pkEntityExport->kAttribInfo.uiCnt && pkInfo);
			FvDataObj* pkData = (FvDataObj*)((char*)m_pkEntity + pkInfo[pkIdx->uiAttribIdx].uiAddr);
			pkData->DeserializeFromStream(kIS);
		}
		else
		{
			FV_ASSERT(m_pkCellData);
			m_pkCellData->DeserializeFromStreamByIdx(pkIdx->uiAttribIdx, kIS);
		}
	}

	//! 需要反流化pos等
	if(m_pkCellData)
		kIS >> m_pkCellData->m_kPos >> m_pkCellData->m_kDir >> m_pkCellData->m_iSpaceID >> m_pkCellData->m_uiSpaceType;

	return true;
}

bool FvBaseAttrib::OnMethodFromClient(int iMessageID, FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport
		&& m_pkEntityExport->kMethodCnts.uiBaseExpCnt > 0
		&& m_pkEntityExport->kMethodCnts.pkBaseExpID
		&& 0<=iMessageID && iMessageID<63
		&& !(iMessageID & 0x20));

	int iID(iMessageID);
	FvUInt8 uiExtID;
	kIS >> uiExtID;
	iID = (iID << 8) | uiExtID;

	FV_ASSERT(iID < m_pkEntityExport->kMethodCnts.uiBaseExpCnt &&
		m_pkEntityExport->pFunEntityMethodsEntry);
	m_pkEntityExport->pFunEntityMethodsEntry(m_pkEntity, m_pkEntityExport->kMethodCnts.pkBaseExpID[iID].uiExpToIdx, kIS);
	return true;
}

bool FvBaseAttrib::OnMethodFromClientWithCallBack(int iMessageID, FvBinaryIStream& kIS, FvObjBlob& kMB)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport
		&& m_pkEntityExport->kMethodCnts.uiBaseExpCnt > 0
		&& m_pkEntityExport->kMethodCnts.pkBaseExpID
		&& 0<=iMessageID && iMessageID<63
		&& (iMessageID & 0x20));

	int iID(iMessageID);
	FvUInt8 uiExtID, uiCBCnt;
	kIS >> uiExtID >> uiCBCnt;

	if(kIS.Error())
		return false;

	static FvMemoryOStream kMS(1024);
	kMS.Reset();

	FV_ASSERT(uiCBCnt);
	FvUInt8 uiCBIDCli;
	FvUInt32 uiCBIDSvr;
	for(FvUInt8 i=0; i<uiCBCnt; ++i)
	{
		kMS.AddBlob(kMB.Data(), kMB.Size());
		kIS >> uiCBIDCli;
		uiCBIDSvr = 0xFFFFFF00 | uiCBIDCli;//! client回调id从0xFFFFFF00开始
		kMS << uiCBIDSvr;
	}

	if(kIS.Error())
		return false;

	kMS.Transfer(kIS, kIS.RemainingLength());

	iID &= ~0x20;
	iID = (iID << 8) | uiExtID;

	FV_ASSERT(iID < m_pkEntityExport->kMethodCnts.uiBaseExpCnt &&
		m_pkEntityExport->pFunEntityMethodsEntry);
	m_pkEntityExport->pFunEntityMethodsEntry(m_pkEntity, m_pkEntityExport->kMethodCnts.pkBaseExpID[iID].uiExpToIdx, kMS);
	return true;
}

bool FvBaseAttrib::OnMethodFromServer(FvUInt16 uiMethodIdx, FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport
		&& m_pkEntityExport->pFunEntityMethodsEntry);

	m_pkEntityExport->pFunEntityMethodsEntry(m_pkEntity, uiMethodIdx, kIS);
	return true;
}


FvDataOwner* FvBaseAttrib::GetRootData(OpCode uiOpCode, FvDataOwner* pkVassal, FvUInt16 uiDataID)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport && pkVassal);

	if(!m_pkEvtCallBack)
		return NULL;

	PointerToIdxToDataID& kPt2Idx2ID = m_pkEntityExport->kPtToIdxToDataID;

	FvUInt16 uiIdx, uiID;
	FvPointerToIndexAndDataID(m_pkEntity, pkVassal, uiIdx, uiID,
		kPt2Idx2ID.uiBitsCnt, kPt2Idx2ID.pkBits, kPt2Idx2ID.pkIdxes, kPt2Idx2ID.pkIdxToID);

	FV_ASSERT(uiIdx < m_pkEntityExport->kAttribInfo.uiCnt);
	AttribInfo& kAttribInfo = m_pkEntityExport->kAttribInfo.pkInfo[uiIdx];
	if(kAttribInfo.IsOwnClientData() || kAttribInfo.IsPersistent())
	{
#ifndef FV_SHIPPING
		//{
		//	static char buf[256] = {0};
		//	if(ms_kDataPath.empty())
		//	{
		//		buf[0] = 0;
		//	}
		//	else
		//	{
		//		int iSize = (int)ms_kDataPath.size();
		//		sprintf_s(buf, sizeof(buf), "%d:", iSize);
		//		for(int i=0; i<iSize; ++i)
		//		{
		//			int len = (int)strlen(buf);
		//			sprintf_s(buf+len, sizeof(buf)-len, "%d,", ms_kDataPath[i]);
		//		}
		//	}
		//	FV_INFO_MSG("PropChgS, Path[%s]\n", buf);
		//}
#endif
		//! 需要差异更新到Client/存档到DB
		uiDataID += uiID;
		m_uiMessageID = FvDataOwner::AddHeadToStream(uiOpCode, m_pkEntityExport->uiDataIDCnt, uiDataID);
		m_uiDataIdx = uiIdx;
		m_uiDataID = uiDataID;
#ifndef FV_SHIPPING
		//FV_INFO_MSG("\tHead&PathLen:%d\n", ms_kDataStream.Size());
#endif
		return this;
	}
	else
	{
		return NULL;
	}
}

void FvBaseAttrib::NotifyDataChanged()
{
	FV_ASSERT(m_pkEvtCallBack);
	FV_ASSERT(m_uiDataIdx < m_pkEntityExport->kAttribInfo.uiCnt);
	AttribInfo& kAttribInfo = m_pkEntityExport->kAttribInfo.pkInfo[m_uiDataIdx];
	FV_ASSERT(kAttribInfo.IsOwnClientData() || kAttribInfo.IsPersistent());

#ifndef FV_SHIPPING
	//FV_INFO_MSG("PropChgE, Entity:%d, Msg:%d, DataID:%d, Len:%d\n", m_uiEntityID, m_uiMessageID, m_uiDataID, ms_kDataStream.Size());
#endif

	bool bOwnCli, bGhost, bOthersCli, bDB;
	bOwnCli = bGhost = bOthersCli = bDB = false;

	//! 差异存档到DB
	if(kAttribInfo.IsPersistent())
		bDB = true;

	//! 差异更新给自己
	if(kAttribInfo.IsOwnClientData())
		bOwnCli = true;

	m_pkEvtCallBack->HandleEvent(m_uiMessageID, ms_kDataStream, bOwnCli, bGhost, bOthersCli, bDB);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvCellAttrib::FvCellAttrib()
:FvDataOwner()
,m_pkEntity(NULL)
,m_pkEntityExport(NULL)
,m_pkRealData(NULL)
,m_uiDataIdx(0)
,m_uiDataID(0)
,m_uiMessageID(0)
,m_pkEvtCallBack(NULL)
#ifndef FV_SHIPPING
,m_uiEntityID(FV_NULL_ENTITY)
#endif
{

}

FvCellAttrib::~FvCellAttrib()
{
	if(m_pkRealData)
		delete m_pkRealData;
}

bool FvCellAttrib::Init(FvEntityID uiEntityID, FvEntity* pkEntity, FvCellEntityExport* pkEntityExport)
{
	FV_ASSERT(pkEntity && pkEntityExport && !m_pkEntity && !m_pkEntityExport);
#ifndef FV_SHIPPING
	m_uiEntityID = uiEntityID;
#endif
	m_pkEntity = pkEntity;
	m_pkEntityExport = pkEntityExport;
	return true;
}

bool FvCellAttrib::CreateRealData()
{
	FV_ASSERT(m_pkEntityExport && !m_pkRealData);

	if(m_pkEntityExport->pFunCreateRealData)
	{
		m_pkRealData = m_pkEntityExport->pFunCreateRealData();
		BindRealData();
	}
	return true;
}

void FvCellAttrib::DestroyRealData()
{
	if(m_pkRealData)
	{
		UnBindRealData();
		delete m_pkRealData;
		m_pkRealData = NULL;
	}
}

void FvCellAttrib::AttachOwnerForCellData()
{
	AttachOwnerForGhostData();
	AttachOwnerForRealData();
}

void FvCellAttrib::AttachOwnerForRealData()
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	if(m_pkRealData)
		static_cast<FvDataOwner*>(m_pkRealData)->Attach(this, true);
}

void FvCellAttrib::AttachOwnerForGhostData()
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kAddrs.uiGhostDataCnt;
	FvUInt32* pkAddrs = m_pkEntityExport->kAddrs.pkGhostAddrs;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkAddrs)
	{
		((FvDataObj*)((char*)m_pkEntity + *pkAddrs))->Attach(this);
	}
}

bool FvCellAttrib::SerializeToStreamForCellData(FvBinaryOStream& kOS) const
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kAttribInfo.uiCnt;
	AttribInfo* pkInfo = m_pkEntityExport->kAttribInfo.pkInfo;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkInfo)
	{
		if(pkInfo->uiIsRef)
		{
			FV_ASSERT(m_pkRealData);
			(*(FvDataObj**)((char*)m_pkEntity + pkInfo->uiAddr))->SerializeToStream(kOS);
		}
		else
		{
			((FvDataObj*)((char*)m_pkEntity + pkInfo->uiAddr))->SerializeToStream(kOS);
		}
	}

	return true;
}

bool FvCellAttrib::SerializeToStreamForRealData(FvBinaryOStream& kOS) const
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	if(!m_pkRealData)
		return true;

	FvUInt16 uiCnt = m_pkEntityExport->kAddrs.uiRealDataCnt;
	FvUInt32* pkRefAddrs = m_pkEntityExport->kAddrs.pkRealAddrs;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkRefAddrs)
	{
		(*(FvDataObj**)((char*)m_pkEntity + *pkRefAddrs))->SerializeToStream(kOS);
	}

	return true;
}

bool FvCellAttrib::SerializeToStreamForGhostData(FvBinaryOStream& kOS) const
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kAddrs.uiGhostDataCnt;
	FvUInt32* pkAddrs = m_pkEntityExport->kAddrs.pkGhostAddrs;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkAddrs)
	{
		((FvDataObj*)((char*)m_pkEntity + *pkAddrs))->SerializeToStream(kOS);
	}

	return true;
}

bool FvCellAttrib::SerializeToStreamForCellDBData(FvBinaryOStream& kOS) const
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kDBToCellIdx.uiCnt;
	FvUInt16* pkIdx = m_pkEntityExport->kDBToCellIdx.pkAttribIdx;
	AttribInfo* pkInfo = m_pkEntityExport->kAttribInfo.pkInfo;

	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkIdx)
	{
		FV_ASSERT(*pkIdx < m_pkEntityExport->kAttribInfo.uiCnt);
		AttribInfo& kInfo = pkInfo[*pkIdx];
		if(kInfo.uiIsRef)
		{
			FV_ASSERT(m_pkRealData);
			(*(FvDataObj**)((char*)m_pkEntity + kInfo.uiAddr))->SerializeToStream(kOS);
		}
		else
		{
			((FvDataObj*)((char*)m_pkEntity + kInfo.uiAddr))->SerializeToStream(kOS);
		}
	}

	return true;
}

bool FvCellAttrib::SerializeToStreamForOwnCliData(FvBinaryOStream& kOS) const
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kCellToCli.uiCnt;
	FvUInt16* pkIdx = m_pkEntityExport->kCellToCli.pkAttribIdx;
	AttribInfo* pkInfo = m_pkEntityExport->kAttribInfo.pkInfo;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkIdx)
	{
		FV_ASSERT(*pkIdx < m_pkEntityExport->kAttribInfo.uiCnt && pkInfo);
		AttribInfo& kInfo = pkInfo[*pkIdx];
		if(kInfo.uiIsRef)
			(*(FvDataObj**)((char*)m_pkEntity + kInfo.uiAddr))->SerializeToStream(kOS);
		else
			((FvDataObj*)((char*)m_pkEntity + kInfo.uiAddr))->SerializeToStream(kOS);
	}

	return true;
}

bool FvCellAttrib::SerializeToStreamForOthersCliData(FvBinaryOStream& kOS) const
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kCellToOthersCli.uiCnt;
	kOS << uiCnt;

	SyncToClientIdx* pkIdx = m_pkEntityExport->kCellToOthersCli.pkAttribIdx;
	AttribInfo* pkInfo = m_pkEntityExport->kAttribInfo.pkInfo;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkIdx)
	{
		FV_ASSERT(pkIdx->uiServerIdx < m_pkEntityExport->kAttribInfo.uiCnt && pkInfo);
		kOS << pkIdx->uiClientIdx;

		AttribInfo& kInfo = pkInfo[pkIdx->uiServerIdx];
		if(kInfo.uiIsRef)
			(*(FvDataObj**)((char*)m_pkEntity + kInfo.uiAddr))->SerializeToStream(kOS);
		else
			((FvDataObj*)((char*)m_pkEntity + kInfo.uiAddr))->SerializeToStream(kOS);
	}

	return true;
}

bool FvCellAttrib::DeserializeFromStreamForCellData(FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kAttribInfo.uiCnt;
	AttribInfo* pkInfo = m_pkEntityExport->kAttribInfo.pkInfo;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkInfo)
	{
		if(pkInfo->uiIsRef)
		{
			FV_ASSERT(m_pkRealData);
			(*(FvDataObj**)((char*)m_pkEntity + pkInfo->uiAddr))->DeserializeFromStream(kIS);
		}
		else
		{
			((FvDataObj*)((char*)m_pkEntity + pkInfo->uiAddr))->DeserializeFromStream(kIS);
		}
	}

	return true;
}

bool FvCellAttrib::DeserializeFromStreamForRealData(FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	if(!m_pkRealData)
		return true;

	FvUInt16 uiCnt = m_pkEntityExport->kAddrs.uiRealDataCnt;
	FvUInt32* pkRefAddrs = m_pkEntityExport->kAddrs.pkRealAddrs;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkRefAddrs)
	{
		(*(FvDataObj**)((char*)m_pkEntity + *pkRefAddrs))->DeserializeFromStream(kIS);
	}

	return true;
}

bool FvCellAttrib::DeserializeFromStreamForGhostData(FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport);

	FvUInt16 uiCnt = m_pkEntityExport->kAddrs.uiGhostDataCnt;
	FvUInt32* pkAddrs = m_pkEntityExport->kAddrs.pkGhostAddrs;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkAddrs)
	{
		((FvDataObj*)((char*)m_pkEntity + *pkAddrs))->DeserializeFromStream(kIS);
	}

	return true;
}

bool FvCellAttrib::OnProperty(int iMessageID, FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport && !m_pkRealData);//! 被调用者必须是Ghost

	FV_ASSERT(m_pkEntityExport->kDataIDToPath.pkDataPath);
	FvUInt16 uiDataID = FvDataOwner::DoDiffRenovate(iMessageID, kIS, m_pkEntity, m_pkEntity,
													m_pkEntityExport->kDataIDToPath.uiDataIDCnt,
													m_pkEntityExport->kDataIDToPath.pkDataPath,
													m_pkEntityExport->pkDataCallBacks);

	return true;
}

bool FvCellAttrib::OnMethodFromClient(int iMessageID, FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport
		&& m_pkEntityExport->kMethodCnts.uiCellExpCnt > 0
		&& m_pkEntityExport->kMethodCnts.pkCellExpID
		&& 0<=iMessageID && iMessageID<63
		&& !(iMessageID & 0x20));

	int iID(iMessageID);
	FvUInt8 uiExtID;
	kIS >> uiExtID;
	iID = (iID << 8) | uiExtID;

	FV_ASSERT(iID < m_pkEntityExport->kMethodCnts.uiCellExpCnt &&
		m_pkEntityExport->pFunEntityMethodsEntry);
	m_pkEntityExport->pFunEntityMethodsEntry(m_pkEntity, m_pkEntityExport->kMethodCnts.pkCellExpID[iID].uiExpToIdx, kIS);
	return true;
}

bool FvCellAttrib::OnMethodFromClientWithCallBack(int iMessageID, FvBinaryIStream& kIS, FvObjBlob& kMB)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport
		&& m_pkEntityExport->kMethodCnts.uiCellExpCnt > 0
		&& m_pkEntityExport->kMethodCnts.pkCellExpID
		&& 0<=iMessageID && iMessageID<63
		&& (iMessageID & 0x20));

	int iID(iMessageID);
	FvUInt8 uiExtID, uiCBCnt;
	kIS >> uiExtID >> uiCBCnt;

	if(kIS.Error())
		return false;

	static FvMemoryOStream kMS(1024);
	kMS.Reset();

	FV_ASSERT(uiCBCnt);
	FvUInt8 uiCBIDCli;
	FvUInt32 uiCBIDSvr;
	for(FvUInt8 i=0; i<uiCBCnt; ++i)
	{
		kMS.AddBlob(kMB.Data(), kMB.Size());
		kIS >> uiCBIDCli;
		uiCBIDSvr = 0xFFFFFF00 | uiCBIDCli;//! client回调id从0xFFFFFF00开始
		kMS << uiCBIDSvr;
	}

	if(kIS.Error())
		return false;

	kMS.Transfer(kIS, kIS.RemainingLength());

	iID &= ~0x20;
	iID = (iID << 8) | uiExtID;

	FV_ASSERT(iID < m_pkEntityExport->kMethodCnts.uiCellExpCnt &&
		m_pkEntityExport->pFunEntityMethodsEntry);
	m_pkEntityExport->pFunEntityMethodsEntry(m_pkEntity, m_pkEntityExport->kMethodCnts.pkCellExpID[iID].uiExpToIdx, kMS);
	return true;
}

bool FvCellAttrib::OnMethodFromServer(FvUInt16 uiMethodIdx, FvBinaryIStream& kIS)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport
		&& m_pkEntityExport->pFunEntityMethodsEntry);

	m_pkEntityExport->pFunEntityMethodsEntry(m_pkEntity, uiMethodIdx, kIS);
	return true;
}

FvDataOwner* FvCellAttrib::GetRootData(OpCode uiOpCode, FvDataOwner* pkVassal, FvUInt16 uiDataID)
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport && pkVassal);

	if(!m_pkEvtCallBack)
		return NULL;

	CellDataIDToPath& kID2Path = m_pkEntityExport->kDataIDToPath;
	FvUInt16 uiIdx;

	if(pkVassal == m_pkRealData)
	{
		FV_ASSERT(0 < uiDataID && uiDataID <= kID2Path.uiDataIDCnt);
		uiIdx = kID2Path.pkDataPath[uiDataID-1].uiIdx;
	}
	else
	{
		PointerToIdxToDataID& kPt2Idx2ID = m_pkEntityExport->kPtToIdxToDataID;
		FvUInt16 uiID;
		FvPointerToIndexAndDataID(m_pkEntity, pkVassal, uiIdx, uiID,
			kPt2Idx2ID.uiBitsCnt, kPt2Idx2ID.pkBits, kPt2Idx2ID.pkIdxes, kPt2Idx2ID.pkIdxToID);
		uiDataID += uiID;
	}

	FV_ASSERT(uiIdx < m_pkEntityExport->kAttribInfo.uiCnt);
	AttribInfo& kAttribInfo = m_pkEntityExport->kAttribInfo.pkInfo[uiIdx];
	if(kAttribInfo.IsOwnClientData() || kAttribInfo.IsGhostedData() || kAttribInfo.IsPersistent())
	{
#ifndef FV_SHIPPING
		//{
		//	static char buf[256] = {0};
		//	if(ms_kDataPath.empty())
		//	{
		//		buf[0] = 0;
		//	}
		//	else
		//	{
		//		int iSize = (int)ms_kDataPath.size();
		//		sprintf_s(buf, sizeof(buf), "%d:", iSize);
		//		for(int i=0; i<iSize; ++i)
		//		{
		//			int len = (int)strlen(buf);
		//			sprintf_s(buf+len, sizeof(buf)-len, "%d,", ms_kDataPath[i]);
		//		}
		//	}
		//	FV_INFO_MSG("PropChgS, Path[%s]\n", buf);
		//}
#endif
		//! 需要差异更新/存档到DB
		m_uiMessageID = FvDataOwner::AddHeadToStream(uiOpCode, kID2Path.uiDataIDCnt, uiDataID);
		m_uiDataIdx = uiIdx;
		m_uiDataID = uiDataID;
#ifndef FV_SHIPPING
		//FV_INFO_MSG("\tHead&PathLen:%d\n", ms_kDataStream.Size());
#endif
		return this;
	}
	else
	{
		return NULL;
	}
}

void FvCellAttrib::NotifyDataChanged()
{
	FV_ASSERT(m_pkEvtCallBack);
	FV_ASSERT(m_uiDataIdx < m_pkEntityExport->kAttribInfo.uiCnt);
	AttribInfo& kAttribInfo = m_pkEntityExport->kAttribInfo.pkInfo[m_uiDataIdx];
	FV_ASSERT(kAttribInfo.IsOwnClientData() || kAttribInfo.IsGhostedData() || kAttribInfo.IsPersistent());

#ifndef FV_SHIPPING
	//FV_INFO_MSG("PropChgE, Entity:%d, Msg:%d, DataID:%d, Len:%d\n", m_uiEntityID, m_uiMessageID, m_uiDataID, ms_kDataStream.Size());
#endif

	bool bOwnCli, bGhost, bOthersCli, bDB;
	bOwnCli = bGhost = bOthersCli = bDB = false;

	//! 差异存档到DB
	if(kAttribInfo.IsPersistent())
		bDB = true;

	//! 差异更新给自己
	if(kAttribInfo.IsOwnClientData())
		bOwnCli = true;

	//! 差异更新到Ghost
	if(kAttribInfo.IsGhostedData())
		bGhost = true;

	//! 差异更新给OthersClient
	if(kAttribInfo.IsOtherClientData())
		bOthersCli = true;

	m_pkEvtCallBack->HandleEvent(m_uiMessageID, ms_kDataStream, bOwnCli, bGhost, bOthersCli, bDB);
}

void FvCellAttrib::BindRealData()
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport && m_pkRealData);

	NameAddr* pkInfo;
	FvUInt32 uiCnt = m_pkRealData->Size(pkInfo);
	FV_ASSERT(uiCnt == m_pkEntityExport->kAddrs.uiRealDataCnt &&
		m_pkEntityExport->kAddrs.pkRealAddrs);

	FvUInt32* pkRefAddrs = m_pkEntityExport->kAddrs.pkRealAddrs;
	for(FvUInt32 i=0; i<uiCnt; ++i, ++pkRefAddrs)
	{
		*(FvDataObj**)((char*)m_pkEntity + *pkRefAddrs) = m_pkRealData->Get(pkInfo, i);
	}
}

void FvCellAttrib::UnBindRealData()
{
	FV_ASSERT(m_pkEntity && m_pkEntityExport && m_pkRealData);

	FvUInt16 uiCnt = m_pkEntityExport->kAddrs.uiRealDataCnt;
	FvUInt32* pkRefAddrs = m_pkEntityExport->kAddrs.pkRealAddrs;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkRefAddrs)
	{
		*(FvDataObj**)((char*)m_pkEntity + *pkRefAddrs) = NULL;
	}
}

#endif
